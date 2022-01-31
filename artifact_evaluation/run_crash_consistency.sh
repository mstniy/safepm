#!/bin/bash
set -e

if [ "$#" -ne 1 ]
then
    echo "Usage: run_crash_consistency.sh <path to pm>" >&2
    exit 1
fi


cd "$(dirname "$0")"
export BENCHMARK_PM_PATH="$1"

########## CRASH CONSISTENCY BENCHMARK ##########

cd ../benchmarks/crash_consistency
./run-variants.sh