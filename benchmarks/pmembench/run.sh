set -e

cd /pmdk_safepm/src/benchmarks
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/safepm.txt
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx.cfg | tee /results/pmembench_tx/safepm.txt
cd /pmdk_asan_only/src/benchmarks
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/asan_only.txt
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx.cfg | tee /results/pmembench_tx/asan_only.txt
cd /pmdk_vanilla/src/benchmarks
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/vanilla.txt
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx.cfg | tee /results/pmembench_tx/vanilla.txt
