/*! \file libtime.c
    \brief Library implementing time-related functions
	\authors Ignacio Tamayo
	\date May 28th 2016
	\version 1.3  
	
    Tells the caller that we are in the past, and the sleep becomes incredibly longer
    

    201	sys_time	time_t *tloc
    35	sys_nanosleep	struct timespec *rqtp	struct timespec *rmtp				
	36	sys_getitimer	int which	struct itimerval *value

	It uses write_memory_byte(), so it has to be compiled
	\code
		gcc -c  -fPIC   libs/libio.c 
		gcc -c  -fPIC   libSandboxHelper.c 
		gcc -shared  -nostdlib  -o libio.so libio.o  libSandboxHelper.o  
	\endcode
	
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
#include <time.h>
#include <sys/time.h>

#include "sandbox_customsyscall_descriptor.h"		//Cumpolsory to interact with sandbox

/*! Tracee Descriptor*/
tracee_descriptor* CUSTOM_TRACEE_DESCRIPTOR = NULL;

const time_t YEAR = (60*60*24*30*12);
char FlipFlop = 0;

const pid_t PPID = 999;

/** Substract and add 1 year to subsequent calls of time()
 * Called after the kernel, overrides the return value
 * */
time_t mytime(time_t *tloc)
{
	time_t i = (time_t)CUSTOM_TRACEE_DESCRIPTOR->kernel_return_value;
	if (i>YEAR)
	{	
		if (FlipFlop) i = i-YEAR;
		else i = i + YEAR;		
		FlipFlop = !FlipFlop;
		if (tloc != NULL)
			write_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)tloc, (void*)i, sizeof(time_t));
	}
	return i;
}

/** Multiplies by 5 the time asked to sleep
 * Call before kernel, keep the previous result just in case
 * */
 int mynanosleep(const struct timespec *req, struct timespec *rem)
{
	int i=0;
	struct timespec tm;
	if (req != NULL)
	{
		read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)req, (void*)&tm, sizeof(tm));
		if (i == 0)
			{
				tm.tv_sec*=5;tm.tv_nsec=0;
				i = write_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)req, (void*)&tm, sizeof(tm));
				read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)req, (void*)&tm, sizeof(tm));
			}
	}
	return 0;
}

/** Divides by 5 the time asked to sleep
 * Call AFTER kernel, so the caller does not see we changed it 
 * Keep the previous result just in case
 * */
 int mynanosleep2(const struct timespec *req, struct timespec *rem)
{
	int i=0;
	struct timespec tm;
	if (req != NULL)
	{
		read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)req, (void*)&tm, sizeof(tm));
		if (i == 0)
			{
				tm.tv_sec/=5;tm.tv_nsec=0;
				i = write_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)req, (void*)&tm, sizeof(tm));
				read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)req, (void*)&tm, sizeof(tm));
			}
	}
	return 0;
}


/**Tries to set the Time of the system. OK is returned but nothing is actually done
 * Call BEFORE the Kernel, do not call the Kernel
 * */
int mysettimeofday(const struct timeval *tv, const struct timezone *tz)
{
	return 0;
}

#define STATIC_DATE_SECS  	403290

/** Returns always the same date and Time Zone
 * Call AFTER the Kernel
 * */
int mygettimeofday(const struct timeval *tv, const struct timezone *tz)
{
	struct timeval timev;
	
	if (tv != NULL)
	{
		if(read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)tv, (void*)&timev, sizeof(struct timeval)) == 0)
			{
				timev.tv_sec=STATIC_DATE_SECS;
				write_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)tv, (void*)&timev, sizeof(struct timeval));
			}
	}
	
	return 0;
}

#ifdef __x86_64__	
	
	#define NANOSLEEP_SYSCALL_NUMBER 		35
	#define GETTIME_SYSCALL_NUMBER 			96
	#define SETTIME_SYSCALL_NUMBER 			164
	#define TIME_SYSCALL_NUMBER 		201
#endif
#ifdef __i386__	
#endif


/*! Array of Structures, one per custom syscall*/
custom_syscall_descriptor custom_syscalls_array_2[] = { 
[NANOSLEEP_SYSCALL_NUMBER] = {(long int (*)())mynanosleep,(long int (*)())mynanosleep2,"nanosleep",FLAG_KEEP_PREVIOUS_RETURN},
[GETTIME_SYSCALL_NUMBER] = {NULL, (long int (*)())mygettimeofday, "gettimeofday" ,0},
[SETTIME_SYSCALL_NUMBER] = { (long int (*)())mysettimeofday,NULL, "settimeofday" ,FLAG_DONT_CALL_KERNEL},
[TIME_SYSCALL_NUMBER] = {NULL,(long int (*)())mytime,"time",0}
};

/*! Library Descriptor*/
custom_library_descriptor CUSTOM_LIBRARY_DESCRIPTOR = {
	NULL,NULL,custom_syscalls_array_2, TIME_SYSCALL_NUMBER+1,"libtime"
	};
	
