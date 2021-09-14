set -e

cd /pmdk_safepm/src/benchmarks
echo "----- SAFEPM MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/safepm.txt
echo "----- SAFEPM TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx.cfg | tee /results/pmembench_tx/safepm.txt
cd /pmdk_wrappers_only/src/benchmarks
echo "----- WRAPPERS ONLY MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/wrappers_only.txt
echo "----- WRAPPERS ONLY TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx.cfg | tee /results/pmembench_tx/wrappers_only.txt
cd /pmdk_asan_only/src/benchmarks
echo "----- ASAN MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/asan_only.txt
echo "----- ASAN TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx.cfg | tee /results/pmembench_tx/asan_only.txt
cd /pmdk_vanilla/src/benchmarks
echo "----- VANILLA MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/vanilla.txt
echo "----- VANILLA TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx.cfg | tee /results/pmembench_tx/vanilla.txt
