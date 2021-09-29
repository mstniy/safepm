#!/bin/bash
NUMBER_OF_ENTRIES=1000000
NUMBER_OF_READS=10000000
DB_SIZE_GB=10

REPEATS=3

RESULT_PATH=$RESULT_PATH/randomreadwrite
mkdir -p $RESULT_PATH

# THIS BENCHMARK AIMS TO SHOW THE BEHAVIOR FOR MULTITHREADED READ WRITE RANDOM WORKLOADS
# IT SHOULD TEST IT FOR DIFFERENT NUMBER OF THREADS AND DIFFERENT READ WRITE PERCENTAGES

##### Different threads #####
# Run test for 50%, and 95% reads
declare -a readpercentages=(50 95)
declare -a num_threads=(1 2 4 8 16 24)
for readpercentage in "${readpercentages[@]}"; do
    declare -a files=( )
    bench=readrandomwriterandom
    RESULT_PATH_READPER=$RESULT_PATH/${readpercentage}_readpercentage
    mkdir -p $RESULT_PATH_READPER
    declare -a files=( )
    bench=readrandomwriterandom
    # Run multiple times the same test
    for i in $(seq 1 $REPEATS); do
        echo  "##### RUN ${i} of benchmark ${bench} threads with ${readpercentage} readpercentage#####"
        create_file=true
        # Execute the actual benchmark operation for different values
        # Delete after each run with one value
        for threads in "${num_threads[@]}"; do
            echo "Running $bench for threads $threads"
            if [ $create_file = true ]; then
                # Use sed to skip the line with the values for fillseq
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$NUMBER_OF_READS --readwritepercent=$readpercentage --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=1024 --threads=$threads --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) | sed '2d' > $RESULT_PATH_READPER/${bench}_threads_$i.csv
                # Need to replace the \r at the file ending, before we can append anything
                sed -i "s/\r$//;1s/$/,Threads/;2s/$/,$threads/" $RESULT_PATH_READPER/${bench}_threads_$i.csv
                create_file=false
            else
                # tail needed to ignore the first two lines (header and fillseq line), and then add the thread number to the line
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$NUMBER_OF_READS --readwritepercent=$readpercentage --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=1024 --threads=$threads --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) | tail -n +3 | sed "s/\r$//;1s/$/,$threads/" >> $RESULT_PATH_READPER/${bench}_threads_$i.csv
            fi
            pmempool rm $MOUNT_PM/pmemkv
        done
        files+=( $RESULT_PATH_READPER/${bench}_threads_$i.csv )
    done
    echo  "##### Generating avg file for ${bench} #####"
    python3 avg.py --files ${files[@]} -r $RESULT_PATH_READPER/${bench}_threads.csv
done
