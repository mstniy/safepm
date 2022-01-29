#!/bin/bash

if [ "$#" -ne 1 ]
then
    echo "Usage: reproduce_bugs.sh <path to pm>" >&2
    exit 1
fi

cd "$(dirname "$0")"
export BENCHMARK_PM_PATH="$1"

########## PMDK BUGS ##########

cd ../benchmarks/bugs
./safepm_bugs.sh
