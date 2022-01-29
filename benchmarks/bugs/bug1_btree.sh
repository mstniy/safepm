set -e

cd /pmdk_safepm/src/benchmarks
echo "----- SAFEPM MAP BTREE BUG-----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../debug ./pmembench pmembench_map_bug.cfg