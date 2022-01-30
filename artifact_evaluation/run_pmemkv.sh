#!/bin/bash
set -e

if [ "$#" -ne 1 ]
then
    echo "Usage: run_pmemkv.sh <path to pm>" >&2
    exit 1
fi


cd "$(dirname "$0")"
export BENCHMARK_PM_PATH="$1"

########## PMEMKV ##########

cd ../benchmarks/pmemkv
./run-variants.sh