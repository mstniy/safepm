RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'

( { "$1"; } >/dev/null 2>&1 ) && echo -e "${RED}$1 did not crash.${NC}" || echo -e "${GREEN}$1 OK.${NC}"
