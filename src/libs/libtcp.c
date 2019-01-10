/*! \file libtcp.c
    \brief Library implementing the syscalls for the TELNET Server
	\authors Ignacio Tamayo
	\date June 11th 2016
	\version 1.0

	* When SENDTO, all the UpperCase and LowerCase are inverted
	* When RCVFROM, all numbers are replaced by '0'
	* When BIND, the Port is shifted some offset if it is >1024. This allows to fool the Server that thinks it has obtained a priviledged port

	It uses read_memory_byte()/write_memory_byte()), so it has to be compiled
	\code
		gcc -c  -fPIC   libs/libio.c
		gcc -c  -fPIC   libSandboxHelper.c
		gcc -shared  -nostdlib  -o libio.so libio.o  libSandboxHelper.o
	\endcode

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
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <ctype.h>

#include "sandbox_customsyscall_descriptor.h"

/** If the port to bind is < TCP_PORT_LIMIT, then the port value is added TCP_PORT_SHIFT and binded  */
#define TCP_PORT_SHIFT 2000
/** If the port to bind is < TCP_PORT_LIMIT, then the port value is added TCP_PORT_SHIFT and binded  */
#define TCP_PORT_LIMIT 1024

#define BUFFER_LENGTH	512


/*! Tracee Descriptor*/
tracee_descriptor* CUSTOM_TRACEE_DESCRIPTOR = NULL;

/** Inverts Upper and Lower case
 * Call BEFORE kernel, keep Kernel Result
 * */
ssize_t mywrite(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
	 //Cannot touch the buffer as it belongs to another process and it would be invading memory

	 int i = len;
	 char buffer[BUFFER_LENGTH],*c;

	if ( i >1 )
	{
		if (i > BUFFER_LENGTH ) i = BUFFER_LENGTH;
		//printf("Reading mem...");
		if (read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)buf, (void*)buffer, i) > 0)
		{
			c = buffer + i;
			while( c>=buffer )
			{
				if( isalpha(*c) )
				{
					*c = islower(*c) ? toupper(*c) : tolower(*c);
				}
			c--;
			}
			write_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)buf, (void*)buffer, i);
		}
	}
	return len;
}

/** replaces any number found by 0
 * Call AFTER kernel, keep Kernel Result
 * */
ssize_t myread(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)

 {
	//Cannot touch the buffer as it belongs to another process and it would be invading memory
	//We get the bytes actually read by the kernel
	 int i = CUSTOM_TRACEE_DESCRIPTOR->kernel_return_value;
	 char buffer[BUFFER_LENGTH],*c;

	if ( i >=0 )
	{
		if (i > BUFFER_LENGTH ) i = BUFFER_LENGTH;
		if (read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)buf, (void*)buffer, i) > 0)
		{
			c = buffer + i;
			while(c >= buffer)
			{
				if( isdigit(*c) )
				{
					*c = '0';
				}
			c--;
			}
			write_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)buf, (void*)buffer, i);
		}
	}
	return len;

}


/** When binding, the data structure is modified to shift the TCP port
 * If the port to bind is < TCP_PORT_LIMIT, then the port value is added TCP_PORT_SHIFT and binded
 * Call BEFORE kernel
 * */
int mybind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int port;
	struct sockaddr_in socket_address;
	if (read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)addr, (void*)&socket_address, sizeof(struct sockaddr_in)) > 0)
	{
		if (socket_address.sin_family == AF_INET)
		{
			port = ntohs(socket_address.sin_port);
			port = (port < TCP_PORT_LIMIT) ? port+TCP_PORT_SHIFT : port;
			socket_address.sin_port = htons(port);
			write_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)addr, (void*)&socket_address,  sizeof(struct sockaddr_in));
		}
	}
	return 0;
}

/** When binding, the data structure is modified to shift the TCP port
 * This function is executed to hide to the tracee the changes in the data
 * Call AFTER kernel
 * */
int unbind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int port;
	struct sockaddr_in socket_address;
	if (read_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)addr, (void*)&socket_address, sizeof(struct sockaddr_in)) > 0)
	{
		if (socket_address.sin_family == AF_INET)
		{
			port = ntohs(socket_address.sin_port);
			port = ( (port -TCP_PORT_SHIFT) <  TCP_PORT_LIMIT) ? port-TCP_PORT_SHIFT : port;
			socket_address.sin_port = htons(port);
			write_memory_byte( CUSTOM_TRACEE_DESCRIPTOR->trace_PID , (void*)addr, (void*)&socket_address,  sizeof(struct sockaddr_in));
		}
	}
	return 0;
}

#ifdef __x86_64__
	#define RCVFROM_SYSCALL_NUMBER 45
	#define SENDTO_SYSCALL_NUMBER 44
	#define BIND_SYSCALL_NUMBER 49
#endif


/*! Array of Structures, one per custom syscall*/
custom_syscall_descriptor custom_syscalls_array_1[] = {
[RCVFROM_SYSCALL_NUMBER] = {
		NULL,
		(long int (*)())myread,
		"NetRead",
		FLAG_KEEP_PREVIOUS_RETURN
		},
[SENDTO_SYSCALL_NUMBER] = {
		(long int (*)())mywrite,
		NULL,
		"NetWrite",
		0
		},
[BIND_SYSCALL_NUMBER] = {
		(long int (*)())mybind,
		(long int (*)())unbind,
		"mybind",
		0
		}
};

/*! Library Descriptor*/
custom_library_descriptor CUSTOM_LIBRARY_DESCRIPTOR = {
	NULL,NULL,custom_syscalls_array_1, BIND_SYSCALL_NUMBER+1,"libTCP"
	};
