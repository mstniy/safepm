set -e

cd /pmdk_safepm/src/benchmarks
echo "----- SAFEPM MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/safepm.txt
echo "----- SAFEPM TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx_safepm.cfg | tee /results/pmembench_tx/safepm.txt
echo "----- SAFEPM OPEN/CREATE -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_obj_gen_safepm.cfg | tee /results/pmembench_open_create/safepm.txt
cd /pmdk_safepm_partial_coverage/src/benchmarks
echo "----- SAFEPM PARTIAL COVERAGE -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map_partial_cov.cfg | tee /results/pmembench_map_partial_cov/safepm.txt
cd /pmdk_wrappers_only/src/benchmarks
echo "----- WRAPPERS ONLY MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/wrappers_only.txt
echo "----- WRAPPERS ONLY TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx_safepm.cfg | tee /results/pmembench_tx/wrappers_only.txt
echo "----- WRAPPERS ONLY OPEN/CREATE -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_obj_gen_safepm.cfg | tee /results/pmembench_open_create/wrappers_only.txt
cd /pmdk_asan_only/src/benchmarks
echo "----- ASAN MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/asan_only.txt
echo "----- ASAN TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx_safepm.cfg | tee /results/pmembench_tx/asan_only.txt
echo "----- ASAN OPEN/CREATE -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_obj_gen_safepm.cfg | tee /results/pmembench_open_create/asan_only.txt
cd /pmdk_vanilla/src/benchmarks
echo "----- VANILLA MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/vanilla.txt
echo "----- VANILLA TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx_safepm.cfg | tee /results/pmembench_tx/vanilla.txt
echo "----- VANILLA OPEN/CREATE -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_obj_gen_safepm.cfg | tee /results/pmembench_open_create/vanilla.txt
echo "----- VANILLA PARTIAL COVERAGE -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map_partial_cov.cfg | tee /results/pmembench_map_partial_cov/vanilla.txt
