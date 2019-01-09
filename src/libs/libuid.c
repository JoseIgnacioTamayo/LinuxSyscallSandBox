/*! \file libuid.c
    \brief Library implementing the getuid() and geteuid() syscalls
        \b getuid() returns the real user ID of the calling process.
        \b geteuid() returns the effective user ID of the calling process.
	\authors Ignacio Tamayo and Vassanthaphriya VIJAYAN
	\date April 27th 2016
	\version 1.2
    
	\see sandbox.c
	
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
 
 
 
 #include <unistd.h>
 #include <sys/types.h>
#include "sandbox_customsyscall_descriptor.h"
#define NUMBER_SYSCALLS	2

/** getuid simply returns a fake UID
 * FLAG_EXEC_BEFORE_KERNEL
 * \return a fake UID value
 * */
uid_t mygetuid(void)
{
   return 12345;
}


/** geteuid simply returns a fake UID
 * FLAG_EXEC_BEFORE_KERNEL
 * \return a fake UID value
 * */
uid_t mygeteuid(void)
{
  return 54321;
}

#ifdef __x86_64__	
	#define GETUID_SYSCALL_NUMBER 102
	#define GETEUID_SYSCALL_NUMBER 107
#endif
#ifdef __i386__	
	#define GETUID_SYSCALL_NUMBER 24
	#define GETEUID_SYSCALL_NUMBER 49
#endif

/*! Array of Structures, one per custom syscall*/
custom_syscall_descriptor custom_syscalls_array_3[] = { 
[GETUID_SYSCALL_NUMBER] = {NULL, (long int (*)())mygetuid, "getuid" ,0},
[GETEUID_SYSCALL_NUMBER] = {NULL,(long int (*)())mygeteuid,"geteuid",0}
};

/*! Library Descriptor*/
custom_library_descriptor CUSTOM_LIBRARY_DESCRIPTOR = {
	NULL,NULL,custom_syscalls_array_3, GETEUID_SYSCALL_NUMBER+1,"libuid"
	};

