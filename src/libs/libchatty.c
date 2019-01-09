/*! \file libchatty.c
    \brief Library that simply prints a message when invoked
	\authors Ignacio Tamayo
	\date May 15th 2016
	\version 1.3  
	
	For many syscalls, a silly message is printed in STDOUT
	
	All functions return 0, *keep the result from the Kernel to avoid problems*
	
	https://www.cs.utexas.edu/~bismith/test/syscalls/syscalls64_orig.html
	
	\note Compile in architectures x86_64
    
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
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>


#include "sandbox_customsyscall_descriptor.h"		//Cumpolsory to interact with sandbox

#define CHATTY_LOGO "\t\t\t\t =0 : "
/** Prefix for all CHATTY Lib messages */



pid_t mygetpid(void)
{
	printf(CHATTY_LOGO"Calling GetPID() \n");
	return 0;
}

pid_t mygetppid(void)
{
	printf(CHATTY_LOGO"Calling GetPPID() \n");
	return 0;
}

int mykill(pid_t pid, int sig)
{
	printf(CHATTY_LOGO"Killing kill(%d) with signal %d \n", (int)pid, (int)sig);
	return 0;
}

ssize_t mywrite(int fd, const void *buf, size_t count)
{
	 //Cannot touch the buffer as it belongs to another process and it would be invading memory
	 printf(CHATTY_LOGO"Writing %d bytes to file %d \n", (int)count, (int)fd);
	return 0;
}

ssize_t myread(int fd, const void *buf, size_t count)
 {
	//Cannot touch the buffer as it belongs to another process and it would be invading memory
	 printf(CHATTY_LOGO"Reading %d bytes from file %d \n", (int)count, (int)fd);
	return 0;
}

 void * mymmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
 {
	 //Cannot touch the buffer as it belongs to another process and it would be invading memory
	 printf(CHATTY_LOGO"Mapping to address %ld from file %d \n", (long)addr, fd);
	return 0;
}

 unsigned int myalarm(unsigned int seconds)
  {
	 //Cannot touch the buffer as it belongs to another process and it would be invading memory
	 printf(CHATTY_LOGO"Setting up alarm in %d seconds \n", (int)seconds);
	return 0;
}

int myopen(const char *pathname, int flags, mode_t mode)
{
	 //Cannot touch the buffer as it belongs to another process and it would be invading memory
	 printf(CHATTY_LOGO"Opening file in mode %d \n", (int)mode);
	return 0;
}

int myclose(int fd)
{
	 //Cannot touch the buffer as it belongs to another process and it would be invading memory
	 printf(CHATTY_LOGO"Closing file  %d \n", (int)fd);
	return 0;
}

 int myfstat(int fd, struct stat *buf)
 {
	 //Cannot touch the buffer as it belongs to another process and it would be invading memory
	 printf(CHATTY_LOGO"Asking status of file  %d \n", (int)fd);
	return 0;
}
 
int mystat(const char *path, struct stat *buf)
 {
	 //Cannot touch the buffer as it belongs to another process and it would be invading memory
	 printf(CHATTY_LOGO"Asking status of file  \n" );
	return 0;
}


 uid_t mygetuid(void)
 {
	printf(CHATTY_LOGO"Getting UID  \n" );
	return 0;
	}
 uid_t mygeteuid(void)
  {
	printf(CHATTY_LOGO"Getting GID  \n" );
	return 0;
	}
 uid_t mygetgid(void)
  {
	printf(CHATTY_LOGO"Getting effective UID  \n" );
	return 0;
	}
 uid_t mygetegid(void)
 {
	printf(CHATTY_LOGO"Getting effective GID  \n" );
	return 0;
	}
 
 
 int mygetdents(unsigned int fd, void *dirp,  unsigned int count)
  {
	printf(CHATTY_LOGO"Getting entries from directory file %d  \n", fd );
	return 0;
	}

 
 void before(void)
  {
	printf(CHATTY_LOGO"Chatty loaded \n" );
	}

 
 void after(void)
  {
	printf(CHATTY_LOGO"Chatty removed \n");
	}


#ifdef __x86_64__
	#define READ_SYSCALL_NUMBER 0
	#define WRITE_SYSCALL_NUMBER 1
	#define OPEN_SYSCALL_NUMBER 2
	#define CLOSE_SYSCALL_NUMBER 3
	#define STAT_SYSCALL_NUMBER 4
	#define FSTAT_SYSCALL_NUMBER 5
	#define MMAP_SYSCALL_NUMBER 9
	#define ALARM_SYSCALL_NUMBER 37
	#define GETPID_SYSCALL_NUMBER 39
	#define KILL_SYSCALL_NUMBER 62
	#define GETDENTS_SYSCALL_NUMBER 78
	#define GETUID_SYSCALL_NUMBER 102
	#define GETGID_SYSCALL_NUMBER 104
	#define GETEUID_SYSCALL_NUMBER 107
	#define GETEGID_SYSCALL_NUMBER 108
	#define GETPPID_SYSCALL_NUMBER 110
	
	
#endif


/*! Array of Structures, one per custom syscall*/
custom_syscall_descriptor custom_syscalls_array_3[] = { 
	[READ_SYSCALL_NUMBER] = {	(long int (*)())myread, 	NULL,"read()",		},
	[WRITE_SYSCALL_NUMBER] = {	(long int (*)())mywrite,	NULL,"write()",		},
	[OPEN_SYSCALL_NUMBER] = {	(long int (*)())myopen, 	NULL,"open()",		},
	[CLOSE_SYSCALL_NUMBER] = {	(long int (*)())myclose,	NULL,"close()",		},
	[STAT_SYSCALL_NUMBER] = {	(long int (*)())mystat, 	NULL,"stat()",		},
	[FSTAT_SYSCALL_NUMBER] = {	(long int (*)())myfstat,	NULL,"fstat()",		},
	[MMAP_SYSCALL_NUMBER] = {	(long int (*)())mymmap,		NULL,"mmap()",		},
	[ALARM_SYSCALL_NUMBER] = {	(long int (*)())myalarm,	NULL,"alarm()",		},
	[GETPID_SYSCALL_NUMBER] = {	(long int (*)())mygetpid,	NULL, "getpid()" ,	},
	[KILL_SYSCALL_NUMBER] = {	(long int (*)())mykill,		NULL,"kill()",		},
	[GETDENTS_SYSCALL_NUMBER] = {(long int (*)())mygetdents,NULL,"getdents()",	},
	[GETUID_SYSCALL_NUMBER] = {	(long int (*)())mygetuid,	NULL,"getuid()",	},
	[GETGID_SYSCALL_NUMBER] = {	(long int (*)())mygetgid,	NULL,"getgid()",	},
	[GETEUID_SYSCALL_NUMBER] = {(long int (*)())mygeteuid,	NULL,"geteuid()",	},
	[GETEGID_SYSCALL_NUMBER] = {(long int (*)())mygetegid,	NULL,"getegid()",	},
	[GETPPID_SYSCALL_NUMBER] = {(long int (*)())mygetppid,	NULL,"getppid()",	}
};

/*! Library Descriptor*/
custom_library_descriptor CUSTOM_LIBRARY_DESCRIPTOR = {
	before,after,custom_syscalls_array_3, GETPPID_SYSCALL_NUMBER+1,"libchatty"
	};

/*! Tracee Descriptor*/
tracee_descriptor* CUSTOM_TRACEE_DESCRIPTOR = NULL;


