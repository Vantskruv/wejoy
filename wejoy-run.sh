SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# Game launch script for STEAM
# set launch options to "/route/to/wejoy-run.sh %command%"
cleanup() {
    # kill all processes whose parent is this process
	kill $WEJOY_PID
    pkill -P $$
}
# Customize the LUA config script you want to use.
wejoy $SCRIPT_DIR/config.lua &
WEJOY_PID=$!
trap cleanup EXIT
"$@"
