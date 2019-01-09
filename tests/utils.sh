#!/bin/bash


SANDBOX_BIN=$(dirname "$0")/../bin/sandbox

# Call as:
#	call_sandbox_press_key "<arguments for sandbox>" "<executable>"
#
# Calls the sandbox locally and waits for keyboard input.
# Useful for scripts that call the SandBox sequentially.
#
# If ErrorCode is not 0, execution is stopped.
# Else, it requires ENTER before quiting.
function call_sandbox_press_key {
echo ----!!!---- Run: sandbox $1 $2  ----!!!----
echo Press ENTER to continue
read a
$SANDBOX_BIN $1 $2
ERR_CODE=$?
if ! [ $ERR_CODE -eq 0 ]
then
	echo ----!!!---- ERROR  ----!!!----
    exit $ERR_CODE
else
	echo ----!!!---- Done, press ENTER  ----!!!----
	read a
fi

}
