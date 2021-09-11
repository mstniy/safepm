#!/bin/bash
NUMBER_OF_ENTRIES=100000
NUMBER_OF_READS=1000000
DB_SIZE_GB=1

REPEATS=5

RESULT_PATH=$RESULT_PATH/randomreadwrite
mkdir -p $RESULT_PATH

# THIS BENCHMARK AIMS TO SHOW THE BEHAVIOR FOR MULTITHREADED READ WRITE RANDOM WORKLOADS
# IT SHOULD TEST IT FOR DIFFERENT NUMBER OF THREADS AND DIFFERENT READ WRITE PERCENTAGES

##### Different readpercentage #####
for (( thread=2; thread<=4; thread=thread*2 )); do
    declare -a files=( )
    bench=readrandomwriterandom
    RESULT_PATH_THREAD=$RESULT_PATH/${thread}_thread
    mkdir -p $RESULT_PATH_THREAD
    # Run multiple times the same test
    for i in $(seq 1 $REPEATS); do
        echo  "##### RUN ${i} of benchmark ${bench} readpercentage with ${thread} threads #####"
        create_file=true
        # Execute the actual benchmark operation for different values
        # Delete after each run with one value
        for (( readpercentage=80; readpercentage<=100; readpercentage=readpercentage+10 )); do
            echo "Running $bench for readpercentage $readpercentage"
            if [ $create_file = true ]; then
                # Use sed to skip the line with the values for fillseq
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$NUMBER_OF_READS --readwritepercent=$readpercentage --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=1024 --threads=$thread --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) | sed '2d' > $RESULT_PATH_THREAD/${bench}_readpercentage_$i.csv
                # Need to replace the \r at the file ending, before we can append anything
                sed -i "s/\r$//;1s/$/,Readpercentage/;2s/$/,$readpercentage/" $RESULT_PATH_THREAD/${bench}_readpercentage_$i.csv
                create_file=false
            else
                # tail needed to ignore the first two lines (header and fillseq line), and then add the readpercentage number to the line
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$NUMBER_OF_READS --readwritepercent=$readpercentage --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=1024 --threads=$thread --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) | tail -n +3 | sed "s/\r$//;1s/$/,$readpercentage/" >> $RESULT_PATH_THREAD/${bench}_readpercentage_$i.csv
            fi
            pmempool rm $MOUNT_PM/pmemkv
        done
        files+=( $RESULT_PATH_THREAD/${bench}_readpercentage_$i.csv )
    done
    echo  "##### Generating avg file for ${bench} #####"
    python3 avg.py --files ${files[@]} -r $RESULT_PATH_THREAD/${bench}_readpercentage.csv
    sleep 2
done

##### Different threads #####
# Run test for 60%, 70%, and 80% reads
for (( readpercentage=60; readpercentage<=80; readpercentage=readpercentage+10 )); do
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
        for (( threads=1; threads<=2; threads=threads+1 )); do
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
