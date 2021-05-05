RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'

if [ "$#" -lt 2 ]
then
  echo "Usage: should_crash.sh <snippet to look for in the output> <command> [arguments...]" >&2
  exit 1
fi

snippet="$1"
command="$2"
shift 2

output=`{ "$command" "$@" 2>&1; }` && (echo -e "${RED}$command did not crash.${NC}"; exit 1)
(echo "$output" | grep -- "$snippet") >/dev/null && echo -e "${GREEN}$command OK.${NC}" || echo -e "${RED}Test for $command failed.${NC}"
