set -e

RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'

function should_crash {
  snippet="$1"
  command="$2"
  shift 2

  output=`{ "$command" "$@" 2>&1; }` && (echo -e "${RED}$command did not crash.${NC}"; exit 1)
  (echo "$output" | grep -- "$snippet") >/dev/null && echo -e "${GREEN}$command OK.${NC}" || echo -e "${RED}Test for $command failed.${NC}"
}

cd "$(dirname "$0")"
mkdir -p build
cd build
cmake ..
make
cd tests

set +e

should_crash "Invalid free" ./mismatched_free.exe
should_crash "Invalid free" ./double_free.exe
should_crash "\[fd\]" ./use_after_free.exe
should_crash "\[fb\]" ./overflow.exe
should_crash "\[04\]" ./int32.exe
