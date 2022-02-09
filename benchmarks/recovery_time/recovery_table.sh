#!/bin/bash
set -e

RESULTS="./results"
declare -a log_size=("4" "128" "512" "2048" "4096")
repeats=100

printf "|                        RECOVERY TIME (ms)                          |\n"
printf "|-------------------------------------------------------------------|\n"
printf "|%17s|%9s|%9s|%9s|%9s|%9s|\n" "Log Size" "4KB" "128KB" "512KB" "2MB" "4MB"
printf "|-------------------------------------------------------------------|\n"

for FILE in $RESULTS/*; do 
    variant="$(basename -- $FILE)"
    printf "|%17s" $variant
    acc=0
    index=0
    while IFS=";", read -r number_of_objects log_size pool_size recovery_time; do
    #acc the repeats values
    acc=`echo $acc + $recovery_time | bc`
    index=$(( $index + 1 ))

    #dump the average and start over
    if [ $index -eq $repeats ]; then
        # acc="scale=6; $acc / $repeats" | bc
        acc=$( echo "scale=6; $acc/$repeats*1000" | bc -l )
        printf "|%9f" $acc
        index=0
        acc=0
    fi

    done < <(tail -n +2 $FILE)
    printf "|\n"
done

printf "|-------------------------------------------------------------------|\n"