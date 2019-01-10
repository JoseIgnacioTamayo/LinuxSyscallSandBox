#!/bin/bash


# Call as:
#	call_sandbox_exit_iferror "<arguments for sandbox>" "<executable>"
#
# Calls the sandbox locally, if it returns an error the script quits
function call_sandbox_exit_iferror {
echo ----!!!---- Run: sandbox $1 $2  ----!!!----
$SANDBOX_BIN $1 $2
ERR_CODE=$?
if ! [ $ERR_CODE -eq 0 ]
then
	echo ----!!!---- ERROR  ----!!!----
    exit $ERR_CODE
fi
}


# Test for ./sandbox with example parameters
#
# Authors: Ignacio TAMAYO and Vassanthaphriya VIJAYAN
# Date: May 2016
# Version: 1.3
#
# Call as './runExample.sh'
# Press any key to move through the sequence of examples

SANDBOX_BIN=$(dirname "$0")/bin/sandbox
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)

$SANDBOX_BIN -h
if ! [ $? -eq 0 ]
then
	exit
fi

echo
echo ------------------------- PID Syscalls, library returns Static PIDs -----------


 call_sandbox_exit_iferror " " "bin/tests/testLibPID"
 call_sandbox_exit_iferror "-L bin/libs -l pid " "bin/tests/testLibPID"

echo
echo ------------------------- IO Syscalls. Local files are created as sniffer in /tmp -----------


wc -l /tmp/localfile.txt
 call_sandbox_exit_iferror "  " "bin/tests/testLibIO /tmp/localfile.txt"
wc -l /tmp/localfile.txt
rm /tmp/Sand*
 call_sandbox_exit_iferror "-L bin/libs -l io " "bin/tests/testLibIO /tmp/localfile.txt"
wc -l /tmp/localfile.txt
ls -lh /tmp/Sand*
cat /tmp/Sand*

echo
echo ------------------------- Catching Syscalls Verbose with custom libraries-----------


 call_sandbox_exit_iferror "-v -L bin/libs -l pid " "bin/tests/testLibPID"

echo
echo ------------------------- Catching Syscalls with several libraries, execution path-----------


 call_sandbox_exit_iferror " -t -L bin/libs -l pid -l chatty " "bin/tests/testLibPID"

echo
echo ------------------------- Catching Syscalls with several libraries-----------


 call_sandbox_exit_iferror " -L bin/libs -l pid -l chatty " "bin/tests/testLibPID"

echo
echo ------------------------- PID Syscalls of parent and child processes, library returns Static PIDs to both processes -----------


 call_sandbox_exit_iferror " " "bin/tests/testFork"
 call_sandbox_exit_iferror "-p -L bin/libs -l pid " "bin/tests/testFork"
 call_sandbox_exit_iferror "-v -p -L bin/libs -l pid " "bin/tests/testFork"

echo
echo ------------------------- Running STRACE on the same commands, to compare-----------

strace -c bin/tests/testLibPID
strace -c bin/tests/testLibIO
strace -c -f bin/tests/testFork

exit 0
