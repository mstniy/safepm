set -e

cd "$(dirname "$0")"
mkdir -p build
cd build
cmake ..
make
cd tests

set +e

../../should_crash.sh "Invalid free" ./mismatched_free.exe
../../should_crash.sh "Invalid free" ./double_free.exe
../../should_crash.sh "[fd]" ./use_after_free.exe
../../should_crash.sh "[fb]" ./overflow.exe
