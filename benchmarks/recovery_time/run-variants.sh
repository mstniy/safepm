#!/bin/bash

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

docker build -t safepm-recovery .
mkdir -p results
docker run -v "$BENCHMARK_PM_PATH:/mnt/recovery" -v "$(pwd)/results":/results -t safepm-recovery bash inner_run.sh
