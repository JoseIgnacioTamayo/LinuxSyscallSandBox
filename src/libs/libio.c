/*! \file libio.c
    \brief Library implementing the READ and WRITE syscalls
	\authors Ignacio Tamayo
	\date April 24th 2016
	\version 1.2

	This is a sniffer For the READ and Write operations, all is captured and copied to the files:
	*
	* /tmp/Sandbox.read, /tmp/Sandbox.write

	It uses read_memory_byte(), so it has to be compiled
	\code
		gcc -c  -fPIC   libs/libio.c
		gcc -c  -fPIC   libSandboxHelper.c
		gcc -shared  -nostdlib  -o libio.so libio.o  libSandboxHelper.o
	\endcode

	\see sandbox.c libSandboxHelper.c

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
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


#include "sandbox_customsyscall_descriptor.h"

#define LOCAL_TEMP_FILE_READ "/tmp/Sandbox.read"
#define LOCAL_TEMP_FILE_WRITE "/tmp/Sandbox.write"

#define BUFFER_LENGTH	64


 int snifferWR;	//!< File object for the Write syscalls
 int snifferRD;	//!< File object for the Read syscalls


/*! Tracee Descriptor*/
tracee_descriptor* CUSTOM_TRACEE_DESCRIPTOR = NULL;

/** Writes to a local file all the write values, up to a certain amount of bytes.
 *
 * Call BEFORE kernel, keep Kernel Result.
 *
 * File output is LOCAL_TEMP_FILE_WRITE, BUFFER_LENGTH is the max amount of bytes to sniff.
 * */
ssize_t mywrite(int fd, const void *buf, size_t count)
{
	 //Cannot touch the buffer as it belongs to another process and it would be invading memory

	 int i;
	 char buffer[BUFFER_LENGTH];

	 if (snifferWR)
	 {
			memset(buffer, 0, BUFFER_LENGTH);
			sprintf(buffer,"Writing %d bytes to %d : ",(int)count,fd);
			write(snifferWR,buffer,BUFFER_LENGTH);
			i = count;
			if ( i >=0 )
			{
				if (i > BUFFER_LENGTH )i = BUFFER_LENGTH;
				if (read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)buf, (void*)buffer, i) > 0)
					write(snifferWR,buffer,i);
			}
			write(snifferWR,"\n",1);
	 }
	 return 0;
}


/** writes to a local file all the read values, up to a certain amount of bytes.
 *
 * Call AFTER kernel, keep Kernel Result.
 *
 * File output is LOCAL_TEMP_FILE_READ, BUFFER_LENGTH is the max amount of bytes to sniff.
 *
 * */
ssize_t myread(int fd, const void *buf, size_t count)
 {
	//Cannot touch the buffer as it belongs to another process and it would be invading memory
	int i;
	 char buffer[BUFFER_LENGTH];

	  if  ( (snifferRD)  )
	 {

								//printf("TraceePID : %d \n",44);
			memset(buffer, 0, BUFFER_LENGTH);
			sprintf(buffer,"Reading %d bytes from %d : ",(int) CUSTOM_TRACEE_DESCRIPTOR->kernel_return_value ,fd);
			write(snifferRD,buffer,BUFFER_LENGTH);
			i = CUSTOM_TRACEE_DESCRIPTOR->kernel_return_value;
			if ( i >=0 )
			{
				if (i > BUFFER_LENGTH )i = BUFFER_LENGTH;
				if (read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)buf, (void*)buffer, i) > 0)
					write(snifferRD,buffer,i);
			}
			write(snifferRD,"\n",1);
	 }
	 return 0;
}

void init(void)
{
	 snifferWR = open(LOCAL_TEMP_FILE_WRITE, O_CREAT | O_APPEND | O_RDWR , S_IRWXU | S_IROTH);
	 snifferRD = open(LOCAL_TEMP_FILE_READ, O_CREAT | O_APPEND | O_RDWR , S_IRWXU | S_IROTH);

}

void end(void)
{
	close(snifferWR);
	close(snifferRD);
}


#ifdef __x86_64__
	#define READ_SYSCALL_NUMBER 0
	#define WRITE_SYSCALL_NUMBER 1
#endif
#ifdef __i386__
	#define READ_SYSCALL_NUMBER 3
	#define WRITE_SYSCALL_NUMBER 4
#endif



/*! Array of Structures, one per custom syscall*/
custom_syscall_descriptor custom_syscalls_array_1[] = {
[READ_SYSCALL_NUMBER] = {
		NULL,
		(long int (*)())myread,
		"myread",
		FLAG_KEEP_PREVIOUS_RETURN
		},
[WRITE_SYSCALL_NUMBER] = {
		(long int (*)())mywrite,
		NULL,
		"mywrite",
		FLAG_KEEP_PREVIOUS_RETURN
		}
};

/*! Library Descriptor*/
custom_library_descriptor CUSTOM_LIBRARY_DESCRIPTOR = {
	init,end,custom_syscalls_array_1, WRITE_SYSCALL_NUMBER+1,"libIO"
	};
