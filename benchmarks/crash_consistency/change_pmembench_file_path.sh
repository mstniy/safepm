#!/bin/bash
sed -i 's|/dev/shm/testfile.map|/mnt/crash_consistency/benchfile.map|' pmembench_map.cfg
sed -i 's|ops-per-thread=1000000|ops-per-thread=10000|' pmembench_map.cfg
sed -i 's|repeats = 3|repeats = 1|' pmembench_map.cfg

sed -i 's|/dev/shm/testfile.tx|/mnt/crash_consistency/benchfile.map|' pmembench_tx_safepm.cfg
sed -i 's|ops-per-thread = 100000|ops-per-thread = 50000|' pmembench_tx_safepm.cfg
sed -i 's|repeats = 10|repeats = 1|' pmembench_tx_safepm.cfg