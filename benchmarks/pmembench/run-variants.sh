#!/bin/bash

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

docker build -t safepm-pmembench .
mkdir -p results/pmembench_map
mkdir -p results/pmembench_tx
mkdir -p results/pmembench_open_create
mkdir -p results/pmembench_map_partial_cov
docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/results":/results -t safepm-pmembench bash inner_run.sh
