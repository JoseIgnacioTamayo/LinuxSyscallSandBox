/*! \file libSandboxHelper.c
    \brief Accesory functions offered by the Sandbox to access memory
	\authors Ignacio TAMAYO and Vassanthaphriya VIJAYAN
	\date August 2016
	\version 1.4
	
	This file contains functions that the Custom Libraries might need from the Sandbox
	* 
	* For instance, reading a chunk of memory from the tracee space, given its PID
	* 
	* Use the structure tracee_descriptor to get this kind of usefull info
	
	\see sandbox_customsyscall_descriptor.h
		
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	
#include <sys/ptrace.h>		// Trace related functions
#include <string.h>			// Neede for strcpy
#include <errno.h>			// Needed for errors in PTRACE calls

#define RETURN_ERR -1
	
int read_memory_byte(pid_t tracee, void * addr, void* dst,  int n)
{
	
	unsigned long ret;
	int read=0;
	if( (n>0) && (addr) && (dst))
	{
		// PEEKDATA read on words, but in general: n bytes to read = X(words) + Y(bytes).
		while( (n - read ) > sizeof(ret) )
		{
			// So first we read full words until possible. 
			// If the n is not a multiple of words, they the remaining chunck is read byte by byte
			ret =  ptrace(PTRACE_PEEKDATA, tracee, (void*)addr+read, NULL);
			if(errno != 0) 	//checking if there was any error while reading memory
				return RETURN_ERR;
			memcpy(dst + read, &ret, sizeof ret);
			read+=sizeof ret;
		}
		while( (n - read ) > 0)
		{
			ret =  ptrace(PTRACE_PEEKDATA, tracee, (void*)addr+read, NULL);
			if(errno != 0) 	//checking if there was any error while reading memory
				return RETURN_ERR;
			memcpy(dst + read, &ret, (n - read ));
			read+=(n - read );
		}
		return read;
	}
	else
		return RETURN_ERR;
}

int write_memory_byte(pid_t tracee, void * addr, void* src,  int n)
{
	unsigned long ret;
	int wrote=0;
	if( (n>0) && (addr) && (src))
	{
		// PEEKDATA read on words, but in general: n bytes to read = X(words) + Y(bytes).
		while( (n - wrote ) > sizeof(ret) )
		{
			// So first we read full words until possible. 
			// If the n is not a multiple of words, they the remaining chunck is read byte by byte
			memcpy( &ret,src + wrote, sizeof ret);
			ptrace(PTRACE_POKEDATA, tracee, (void*)addr+wrote, ret);
			if(errno != 0) 	//checking if there was any error while reading memory
				return RETURN_ERR;
			wrote+=sizeof ret;
		}
		while( (n - wrote ) > 0)
		{
			ret =  ptrace(PTRACE_PEEKDATA, tracee, (void*)addr+wrote, NULL);
			memcpy(&ret,src + wrote, (n - wrote ));
			ptrace(PTRACE_POKEDATA, tracee, (void*)addr+wrote, ret);
			if(errno != 0) 	//checking if there was any error while reading memory
				return RETURN_ERR;
			wrote+=(n - wrote );
		}
		return wrote;
	}
	else
		return RETURN_ERR;
}

