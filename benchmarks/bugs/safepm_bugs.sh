#!/bin/bash

if [ "$#" -ne 1 ]; then
    if [[ -z "${BENCHMARK_PM_PATH}" ]]; then
        echo "Usage: safepm_bugs.sh <path to pm>" >&2
        exit 1
    fi
else
    export BENCHMARK_PM_PATH="$1"
fi

docker build -t safepm-pmembench .

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v -it safepm-pmembench bash bug1_btree.sh

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v -it safepm-pmembench bash bug2_tx.sh