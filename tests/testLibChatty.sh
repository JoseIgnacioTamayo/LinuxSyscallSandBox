#!/bin/bash

# Test for ./sandbox with chatty library
# Authors: Ignacio Tamayo
# Date: May 15th 2016
# Version: 1.3

#Testing with the following libraries and programs
# id ,  ps -u , ls ,  echo Lalalala

source tests/utils.sh

if ! [ -f $SANDBOX_BIN ]
then
	echo Missing Sandbox
	exit 9
fi

echo
echo ------------------------- Catching Syscalls with custom chatty library. -----------
echo ------------------------- Message is printed for each syscall captured. -----------
read x

call_sandbox_press_key "-L bin/libs -l chatty  " "echo lalalala"
call_sandbox_press_key "-L bin/libs -l chatty " "id"
call_sandbox_press_key "-L bin/libs -l chatty " "ps -u"
call_sandbox_press_key "-L bin/libs -l chatty  " "ls"


echo
echo ------------------------- Running STRACE on the same commands, to compare-----------
read x

echo  ----!!!---- echo  ----!!!----
strace -c echo lalala
echo ----!!!---- Done, Press ENTER  ----!!!----
read a
echo  ----!!!---- id  ----!!!----
strace  -c id
echo ----!!!---- Done, Press ENTER  ----!!!----
read a
echo  ----!!!---- ps -u  ----!!!----
strace -c ps -u
echo ----!!!---- Done, Press ENTER  ----!!!----
read a
echo  ----!!!---- ls  ----!!!----
strace -c ls
echo ----!!!---- Done, Press ENTER  ----!!!----
read a
