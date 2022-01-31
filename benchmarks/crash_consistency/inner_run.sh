set -e

cd /pmdk_wrappers_only/src/benchmarks
echo "----- CRASH CONSISTENCY MAP BENCHMARK WITH MEMCHECK -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug valgrind --tool=memcheck ./pmembench pmembench_map.cfg
echo "----- CRASH CONSISTENCY MAP BENCHMARK WITH PMEMCHECK -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug valgrind --tool=pmemcheck ./pmembench pmembench_map.cfg

echo "----- CRASH CONSISTENCY TX BENCHMARK WITH MEMCHECK -----"
LD_LIBRARY_PATH=../debug valgrind --tool=memcheck ./pmembench pmembench_tx_safepm.cfg
echo "----- CRASH CONSISTENCY TX BENCHMARK WITH PMEMCHECK -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../debug valgrind --tool=pmemcheck ./pmembench pmembench_tx_safepm.cfg