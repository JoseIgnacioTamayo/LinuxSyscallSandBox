#!/bin/bash

# Test for ./sandbox with Time library
# Authors: Ignacio Tamayo
# Date: May 29th 2016
# Version: 1.3

source tests/utils.sh

if ! [ -f $SANDBOX_BIN ]
then
	echo Missing Sandbox
	exit 9
fi

echo
echo ------------------------- Asking for real times, trying to  set the local time -----------
read a
call_sandbox_press_key " " "bin/tests/testLibTime"

echo
echo ------------------------- Sleeping for longer and pretending to set the time. Linux does not call gettimeofday  or time -----------
read a

call_sandbox_press_key "-L bin/libs -l time " "bin/tests/testLibTime"

echo
echo ------------------------- Showing STRACE for comparison -----------
read a
strace -c bin/tests/testLibTime
