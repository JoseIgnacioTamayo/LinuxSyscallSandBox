/*! \file sandbox_customsyscall_descriptor.h
    \brief Structure that describes a custom syscall needed for the Sandbox to operate with the Custom Syscalls libraries
	\authors Ignacio TAMAYO and Vassanthaphriya VIJAYAN
	\date August 2016
	\version 1.4
	
	Any CustomSyscall, in order to be executed by the Sandbox, has to be described in the structures detailed here.
	
	First, the library is identified by the structure  custom_library_descriptor.
	
	There are two possible functions to execute in the library, at loading and at unloading, to be initialized and to close resources properly.	If there is nothing to do, leave the pointer to NULL.
	
	For each custom syscall to execute, create an structure custom_syscall_descriptor.
	There are two possible funtions per syscall, to be executed before and after the kernel.
	If there is nothing to do, leave the pointer to NULL
	* 
	Several options are able to configure the way the chain of custom syscalls is executed
	* 
	* Finally, the structure tracee_descriptor POINTER will be linked by the Sandobox for the library to have access
	* to information from the tracee and the execution process.
	
	\note There will have to be 2 compulsory elements, please have them in the same name as the MACRO:
	\code
	custom_library_descriptor CUSTOM_LIBRARY_DESCRIPTOR; 
	tracee_descriptor* CUSTOM_TRACEE_DESCRIPTOR; 
	\endcode
	
	 \note	Please compile the library with the following command:
	 \code
	 gcc -shared -nostdlib -o <output .so file> <input .o file>
	 \endcode

	\remarks Architecture x32: http://docs.cs.up.ac.za/programming/asm/derick_tut/syscalls.html
	\remarks Architecture x64: https://www.cs.utexas.edu/~bismith/test/syscalls/syscalls64_orig.html

	\see dynlib.c sandbox.c 
	
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
 
 
#ifndef INC_SANDBOX_CUSTOMSYSCALL_DESC	//Lock to prevent recursive inclusions
#define INC_SANDBOX_CUSTOMSYSCALL_DESC

#ifdef __x86_64__	
	#define MAX_SYSCALL_INDEX 316
#endif
#ifdef __i386__	
	#define MAX_SYSCALL_INDEX 358
#endif

/** Kernel original syscall is not executed */
#define	FLAG_DONT_CALL_KERNEL			2

/** Skip this return value. 
 * In the chain of custom syscalls, the return value of this function is not considered.
 * The last return result, either from the Kernel or another custom syscall function, is kept and passed along */
#define	FLAG_KEEP_PREVIOUS_RETURN	8

/** Abort custom execution if error. 
 * If any of the custom syscall returns a negative value, 
 * the execution of the syscall chain process is aborted for this and all next custom syscalls 
 * Then the kernel syscall is directly called or the syscall processing is returned to the tracee.*/
#define FLAG_QUIT_IF_RETURN_NEGATIVE			16


/**Max Characters for the name of the syscall and the library */
#define		NAME_LENGTH	24	

/*! \brief Structure that describes a custom Syscall */
typedef struct {	
	
	long int (*custom_syscall_before)(); //!<Pointer to the function to be executed BEFORE the kernel syscall. Null if nothing to do
	
	long int (*custom_syscall_after)();	//!<Pointer to the function to be executed AFTER the kernel syscall. Null if nothing to do
	
	char name[NAME_LENGTH];		//!<Name of the custom syscall
	
	char flags;			//!<Combination of option flags. 0 if no options. Concatenate options with | .
	} 
custom_syscall_descriptor;

/** \brief  Structure that describes a custom Library */
typedef struct {
	void (*initialize)();	  //!< Pointer to the function to be executed when the library is loaded. Null if nothing to do
	void (*terminate)();  //!< Pointer to the function to be executed when closing the sandbox. Null if nothing to do
	custom_syscall_descriptor* syscall_descriptor_array;  //!< Pointer to the Descriptor array of the library
	int syscall_descriptor_array_len;	  //!< Length of the Descriptor array of the library
	char name[NAME_LENGTH];	//!< Name of the custom library
	} 
custom_library_descriptor;

/*! \brief Structure to access some values from the TRACEE process */
typedef struct {
	
	long int return_value;		 //!< Return value of the last custom syscall. Updated every time some function is executed
	
	long int kernel_return_value;	//!< Return value of the kernel syscall or kernel syscall. Updated when Kernel is executed, -1 if it was not executed
	
	int trace_PID;		//!< PID of the tracee process

	char kernel_executed;		//!< TRUE(1) if the kernel syscall has been executed for this syscall
	} 
tracee_descriptor;
	
  	
/** Structure for an empty Custom Syscall*/
#define EMPTY_SYSCALL_STRUCT	{NULL,NULL,"",0} 	
/** Structure for an empty Custom Library*/
#define EMPTY_LIBRARY_STRUCT	{NULL,0,NULL,NULL,""} 
/** Structure for an empty Tracee*/
#define EMPTY_TRACEE_STRUCT	{0,0,0,0} 	

/*! Structure defining the library being implemented. Must be implemented in the library file. \see dynlib.c */ 
extern custom_library_descriptor CUSTOM_LIBRARY_DESCRIPTOR; 

/*! Structure containting the tracee information. Must remain a pointer in the library file. \see dynlib.c */ 
extern const tracee_descriptor* CUSTOM_TRACEE_DESCRIPTOR; 

 /** Symbols of the elements to look for in the custom library. \see dynlib.c */
#define CUSTOM_LIBRARY_DESCRIPTOR_SYMBOL 	"custom_library"
#define CUSTOM_LIBRARY_DESCRIPTOR 	custom_library

 /** Symbols of the elements to look for in the tracee descriptor. \see dynlib.c */
#define CUSTOM_TRACEE_DESCRIPTOR_SYMBOL 	"custom_tracee"
#define CUSTOM_TRACEE_DESCRIPTOR 	custom_tracee

#define RETURN_ERR -1

/** Reads from the PID's memory into a given buffer location.
 * Implemented in libSandboxHelper.c.
 * \param tracee is the PID of the tracee process, where PTRACE is attached. Use TRACEE_DESCRIPTOR->trace_PID.
 * \param addr is the address of the location to read.
 * \param dst is the destination buffer.
 * \param n bytes read.
 * \return the amount of bytes read sucessfull, RETURN_ERR if not.
 * \see libSandboxHelper.c
 * */
int read_memory_byte(pid_t tracee, void * addr, void* dst, int n);

/** Writes to the PID's memory at a given address, from a local buffer.
 * Implemented in libSandboxHelper.c.
 * \param tracee is the PID of the tracee process, where PTRACE is attached. Use TRACEE_DESCRIPTOR->trace_PID.
 * \param addr is the address of the location to write to.
 * \param src is the source buffer.
 * \param n bytes write.
 * \return the amount of bytes wrote sucessfull, RETURN_ERR if not.
 * \see libSandboxHelper.c
 * */
int write_memory_byte(pid_t tracee, void * addr, void* src, int n);


#endif
