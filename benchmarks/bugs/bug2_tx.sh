set -e

cd /pmdk_safepm/src/benchmarks
echo "----- SAFEPM MAP TX BUG-----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../debug ./pmembench pmembench_tx_bug.cfg