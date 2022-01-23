#!/bin/bash
docker build -t safepm-pmembench .
mkdir -p results/pmembench_map
mkdir -p results/pmembench_tx
mkdir -p results/pmembench_open_create
mkdir -p results/pmembench_map_partial_cov
docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/results":/results -it safepm-pmembench bash inner_run.sh
