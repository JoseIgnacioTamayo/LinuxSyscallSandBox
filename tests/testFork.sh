#!/bin/bash

# Test for ./sandbox with forked processes
# Authors: Ignacio Tamayo
# Date: June 5th 2016
# Version: 1.4

source tests/utils.sh

if ! [ -f $SANDBOX_BIN ]
then
	echo Missing Sandbox
	exit 9
fi

echo
echo ------------------------- Asking for real PIDs -----------
#echo
call_sandbox_press_key " " "bin/tests/testFork"


echo
echo ------------------------- Fake static PIDs, only on parent-----------
#echo

call_sandbox_press_key "-L bin/libs -l pid " "bin/tests/testFork"
call_sandbox_press_key "-v -L bin/libs -l pid " "bin/tests/testFork"

echo
echo ------------------------- Fake static PIDs, on parent and child -----------
#echo

call_sandbox_press_key "-p -L bin/libs -l pid " "bin/tests/testFork"
call_sandbox_press_key "-p -v -L bin/libs -l pid " "bin/tests/testFork"

echo
echo ------------------------- Running STRACE on the same commands, to compare-----------
read x

strace -c -f bin/tests/testFork
echo ----!!!---- Done, Press ENTER  ----!!!----
read a
