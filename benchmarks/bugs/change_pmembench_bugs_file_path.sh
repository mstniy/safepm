#!/bin/bash
if [ -f pmembench_tx_bug.cfg ]
then
  sed -i 's|/dev/shm/testfile.tx|/mnt/pmembench/benchfile.map|' pmembench_tx_bug.cfg
fi

if [ -f pmembench_map_bug.cfg ]
then
  sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map_bug.cfg
fi