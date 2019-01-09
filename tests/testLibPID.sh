#!/bin/bash

# Test for ./sandbox with PID library
# Authors: Ignacio Tamayo
# Date: May 15th 2016
# Version: 1.3

source tests/utils.sh

if ! [ -f $SANDBOX_BIN ]
then
	echo Missing Sandbox
	exit 9
fi

echo
echo ------------------------- Asking for real PIDs, trying to kill PID 10 -----------
#echo
call_sandbox_press_key " " "bin/tests/testLibPID"

echo
echo ------------------------- Fake static PIDs, and kill accepted-----------
#echo

call_sandbox_press_key "-L bin/libs -l pid " "bin/tests/testLibPID"
