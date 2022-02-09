#!/bin/bash
set -e

REPEATS=100
declare -a log_size=("4" "128" "512" "2048" "4096")

cd /recovery-safepm/build
echo "----- SAFEPM RECOVERY -----"
rm -f /results/safepm
echo "number_of_objects;log_size(KB);pool_size(MB);recovery_time(s)" >> /results/safepm
for log_sz in "${log_size[@]}"
do
    for i in $(seq 1 $REPEATS)
    do
        LD_LIBRARY_PATH=/safepm/lib ./recovery_run $log_sz >> /results/safepm
    done
done

cd /recovery-wrappers/build
echo "----- WRAPPERS ONLY RECOVERY -----"
rm -f /results/wrappers_only
echo "number_of_objects;log_size(KB);pool_size(MB);recovery_time(s)" >> /results/wrappers_only
for log_sz in "${log_size[@]}"
do
    for i in $(seq 1 $REPEATS)
    do
        LD_LIBRARY_PATH=/wrappers/lib ./recovery_run $log_sz >> /results/wrappers_only
    done
done

cd /recovery-asan/build
echo "----- ASAN RECOVERY -----"
rm -f /results/asan_only
echo "number_of_objects;log_size(KB);pool_size(MB);recovery_time(s)" >> /results/asan_only
for log_sz in "${log_size[@]}"
do
    for i in $(seq 1 $REPEATS)
    do
        LD_LIBRARY_PATH=/asan/lib ./recovery_run $log_sz >> /results/asan_only
    done
done

cd /recovery-vanilla/build
echo "----- VANILLA RECOVERY -----"
rm -f /results/vanilla
echo "number_of_objects;log_size(KB);pool_size(MB);recovery_time(s)" >> /results/vanilla
for log_sz in "${log_size[@]}"
do
    for i in $(seq 1 $REPEATS)
    do
        LD_LIBRARY_PATH=/vanilla/lib ./recovery_run $log_sz >> /results/vanilla
    done
done