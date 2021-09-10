set -e

cd /pmdk_safepm/src/benchmarks
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/safepm.txt
cd /pmdk_asan_only/src/benchmarks
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/asan_only.txt
cd /pmdk_vanilla/src/benchmarks
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/vanilla.txt
