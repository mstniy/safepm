#!/bin/bash
# 1 Mio entries
NUMBER_OF_ENTRIES=100000
DB_SIZE_GB=1

REPEATS=1

RESULT_PATH=$RESULT_PATH/numops
mkdir -p $RESULT_PATH

# THIS BENCHMARK AIMS TO SHOW THE DIFFERENCE BETWEEN THE OLD AND THE NEW COMMIT
# THE COMMIT INTRODUCED A DISJOINT FLAG WHICH CHANGES THE BAHVIOR OF READS AND PRODUCES A HIGHER MISS RATE

# Execute the benchmarks
(cd $PMEMKVBENCH && git checkout 4640e1981213c9df9cab9f3aa663c742b800330b && make bench)
##### Old test #####
declare -a benchmarks=(readseq readrandom)
for bench in "${benchmarks[@]}"; do
    declare -a files=( )
    # Run multiple times the same test
    for i in $(seq 1 $REPEATS); do
        echo  "##### RUN ${i} of benchmark ${bench} numops #####"
        create_file=true
        # Execute the actual benchmark operation for different values
        # Delete after each run with one value
        for (( numops=100000; numops<=10000000; numops=numops*10 )); do
            echo "Running $bench for numops $numops"
            if [ $create_file = true ]; then
                # Use sed to skip the line with the values for fillseq
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$numops --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=4096 --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) | sed '2d' > $RESULT_PATH/${bench}_numops_4640e19_$i.csv
                # Need to replace the \r at the file ending, before we can append anything
                sed -i "s/\r$//;1s/$/,NumOps/;2s/$/,$numops/" $RESULT_PATH/${bench}_numops_4640e19_$i.csv
                create_file=false
            else
                # tail needed to ignore the first two lines (header and fillseq line)
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$numops --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=4096 --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) | tail -n +3 | sed "s/\r$//;1s/$/,$numops/" >> $RESULT_PATH/${bench}_numops_4640e19_$i.csv
            fi
            pmempool rm $MOUNT_PM/pmemkv
        done
        files+=( $RESULT_PATH/${bench}_numops_4640e19_$i.csv )
    done
    echo  "##### Generating avg file for ${bench} #####"
    python3 avg.py --files ${files[@]} -r $RESULT_PATH/${bench}_numops_4640e19.csv
done

(cd $PMEMKVBENCH && git checkout 43d842b8b685dea2778390ccaf2c095f92ba7486 && make bench)
##### New test #####
for bench in "${benchmarks[@]}"; do
    declare -a files=( )
    # Run multiple times the same test
    for i in $(seq 1 $REPEATS); do
        echo  "##### RUN ${i} of benchmark ${bench} numops #####"
        create_file=true
        # Execute the actual benchmark operation for different values
        # Delete after each run with one value
        for (( numops=100000; numops<=10000000; numops=numops*10 )); do
            echo "Running $bench for numops $numops"
            if [ $create_file = true ]; then
                # Use sed to skip the line with the values for fillseq
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$numops --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=4096 --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) | sed '2d' > $RESULT_PATH/${bench}_numops_43d842b_$i.csv
                # Need to replace the \r at the file ending, before we can append anything
                sed -i "s/\r$//;1s/$/,NumOps/;2s/$/,$numops/" $RESULT_PATH/${bench}_numops_43d842b_$i.csv
                create_file=false
            else
                # tail needed to ignore the first two lines (header and fillseq line)
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$numops --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=4096 --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) | tail -n +3 | sed "s/\r$//;1s/$/,$numops/" >> $RESULT_PATH/${bench}_numops_43d842b_$i.csv
            fi
            pmempool rm $MOUNT_PM/pmemkv
        done
        files+=( $RESULT_PATH/${bench}_numops_43d842b_$i.csv )
    done
    echo  "##### Generating avg file for ${bench} #####"
    python3 avg.py --files ${files[@]} -r $RESULT_PATH/${bench}_numops_43d842b.csv
done
