set -e

cd "$(dirname "$0")"
mkdir -p build
cd build
cmake ..
make
cd tests

set +e

../../should_crash.sh ./mismatched_free.exe
../../should_crash.sh ./double_free.exe
../../should_crash.sh ./use_after_free.exe
