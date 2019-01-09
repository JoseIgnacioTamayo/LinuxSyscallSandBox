/*! \file libpid.c
    \brief Library implementing the getPID, getPPID and kill syscall
	\authors Ignacio Tamayo
	\date May 5th 2016
	\version 1.3  
	
    Returns always the same number and kills no one
    
	\see sandbox.c sandbox_customsyscall_descriptor.h
	
*/

/*
 Licence
--------------
Copyright (c) 2016 Ignacio TAMAYO and Vassanthaphriya VIJAYAN

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 * */
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>


#include "sandbox_customsyscall_descriptor.h"		//Cumpolsory to interact with sandbox

/*! Tracee Descriptor*/
tracee_descriptor* CUSTOM_TRACEE_DESCRIPTOR = NULL;

const pid_t PID = 666;
const pid_t PPID = 999;

/** getpid simply returns a fake PID
 * EXEC_AFTER_KERNEL
 * \return a fake PID value
 * */
pid_t mygetpid()
{
	return PID;
}

/** getppid simply returns a fake PPID
 *  EXEC_AFTER_KERNEL
 * \return a fake PPID value
 * */
pid_t mygetppid(void)
{
	return PPID;
}

/** kill that does nothing
 *  EXEC_BEFORE_KERNEL | FLAG_EXEC_DONT_CALL_KERNEL
 * \return 0 as if OK
 * */
int mykill(pid_t pid, int sig)
{
	return 0;
}


#ifdef __x86_64__	
	#define GETPID_SYSCALL_NUMBER 39
	#define GETPPID_SYSCALL_NUMBER 110
	#define KILL_SYSCALL_NUMBER 62
#endif
#ifdef __i386__	
	#define GETPID_SYSCALL_NUMBER 20
	#define GETPPID_SYSCALL_NUMBER 64
	#define KILL_SYSCALL_NUMBER 37
#endif


/*! Array of Structures, one per custom syscall*/
custom_syscall_descriptor custom_syscalls_array_2[] = { 
[GETPID_SYSCALL_NUMBER] = {NULL, (long int (*)())mygetpid, "getpid" ,FLAG_DONT_CALL_KERNEL},
[KILL_SYSCALL_NUMBER] = {(long int (*)())mykill,0,"kill",FLAG_DONT_CALL_KERNEL},
[GETPPID_SYSCALL_NUMBER] = {NULL,(long int (*)())mygetppid,"getppid",FLAG_DONT_CALL_KERNEL}
};

/*! Library Descriptor*/
custom_library_descriptor CUSTOM_LIBRARY_DESCRIPTOR = {
	NULL,NULL,custom_syscalls_array_2, GETPPID_SYSCALL_NUMBER+1,"libpid"
	};
	
