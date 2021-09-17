#!/bin/bash
# Specify the parameter before running the actual benchmark
# The values be used in all subsequental benchmarking scripts
PMEMKVBENCH=/pmemkv-bench
# Use this line to run the benchmarks on DRAM. Do not forget to also modify run-variants.sh
#MOUNT_PM=/dev/shm
MOUNT_PM=/mnt/pmem0/stavrakakis/kartal
RESULT_PATH=/results

printSection (){
    echo -e "###### $1 ######"
}

export ASAN_OPTIONS=detect_leaks=0
# No arguments means execute all benchmarks
if [ $# -eq 0 ]; then
    printSection "Starting all benchmarks"
    declare -a operations=( "read" "randomreadwrite" "delete" ) # "fill" "numops" )

    for op in "${operations[@]}"; do
        printSection $op
        (. ./run-pmemkv-bench-${op}.sh)
    done
else
    for op in "$@"; do
        case "$op" in
            "fill"|"read"|"randomreadwrite"|"delete"|"numops"|"test") printSection $op; (. ./run-pmemkv-bench-${op}.sh);;
            *)
                echo "Unknown option: ${op}"
                echo "Available options are: fill, read, randomreadwrite, delete, numops"
                ;;
        esac
    done
fi

exit 0
