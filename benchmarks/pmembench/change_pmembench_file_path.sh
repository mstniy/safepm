#!/bin/bash
sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map.cfg
sed -i 's|/dev/shm/testfile.tx|/mnt/pmembench/benchfile.map|' pmembench_tx.cfg
if [ -f pmembench_obj_gen_safepm.cfg ]
then
  sed -i 's|./testfile.obj|/mnt/pmembench/benchfile.map|' pmembench_obj_gen_safepm.cfg
  sed -i 's|/dev/shm/testfile.tx|/mnt/pmembench/benchfile.map|' pmembench_tx_safepm.cfg
fi

if [ -f pmembench_map_partial_cov.cfg ]
then
  sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map_partial_cov.cfg
fi
