#!/bin/bash

# Test for ./sandbox with multiple libraries and cascaded execution
# Authors: Ignacio Tamayo
# Date: May 23th 2016
# Version: 1.3

source tests/utils.sh

if ! [ -f $SANDBOX_BIN ]
then
	echo Missing Sandbox
	exit 9
fi

echo
echo ------------------------- PID Syscalls, library libpid.so, execution tree -----------
read x

call_sandbox_press_key " -t -L bin/libs -l pid " "bin/tests/testLibPID"
call_sandbox_press_key " -v -L bin/libs -l pid " "bin/tests/testLibPID"

echo
echo ------------------------- PID Syscalls and Chatty library, execution tree -----------
read x

call_sandbox_press_key " -t -L bin/libs -l pid -l chatty " "bin/tests/testLibPID"
echo
echo ------------------------- PID Syscalls and Chatty library, execution tree, inverted order -----------
read x

call_sandbox_press_key " -t -L bin/libs  -l chatty -l pid " "bin/tests/testLibPID"

echo
echo ------------------------- PID Syscalls and Chatty library -----------
read x

call_sandbox_press_key  " -L bin/libs -l pid -l chatty " "bin/tests/testLibPID"

echo
echo ------------------------- PID Syscalls twice library -----------
read x

call_sandbox_press_key  " -v -L bin/libs -l pid -l pid " "bin/tests/testLibPID"
