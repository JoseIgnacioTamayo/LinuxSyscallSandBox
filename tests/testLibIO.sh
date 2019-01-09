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


echo ------------------------- The test program writes some text into localfile.txt -----------
echo ---- File before:
cat /tmp/localfile.txt
echo ENTER
read x
call_sandbox_press_key " " "bin/tests/testLibIO /tmp/localfile.txt"
echo ---- File after:
cat /tmp/localfile.txt
echo ENTER
read x

echo ------------------------- The library writes a copy of all the read and written -----------
rm -f /tmp/Sandbox.*
call_sandbox_press_key "-L bin/libs -l io " "bin/tests/testLibIO /tmp/localfile.txt"
echo ---- File after:
cat /tmp/localfile.txt
echo ENTER
read x
echo ---- File sniffed:
ls -lh /tmp/Sand*
cat /tmp/Sand*
echo ENTER
read x

echo ------------------------- The library writes a copy of all the read and written -----------
#echo
rm -f /tmp/Sandbox.*
call_sandbox_press_key "-L bin/libs -l io " "bin/tests/simpleIOInteractive"
echo ---- File sniffed:
ls -lh /tmp/Sand*
cat /tmp/Sand*
echo ENTER
read x
