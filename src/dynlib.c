/*! \file dynlib.c
    \brief Functions dedicated to loading the dynamic libraries and the custom syscalls
	\authors Ignacio TAMAYO and Vassanthaphrya VIJAYAN
	\date August 2016
	\version 1.4

	\see dynlib.h

	\warning The Maximum ammount of syscalls supported is architecture-dependant.
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
#include <dlfcn.h>					// For Dynamic loading of libraries
#include "list.h"
#include "messages.h"
#include "dynlib.h"					//To have MACROS for these functions

#ifdef __x86_64__
	#define MAX_SYSCALLS 		316
	/** Maximum amount of syscalls supported in the architecture*/
#endif
#ifdef __i386__
	#define MAX_SYSCALLS 		358
	/** Maximum amount of syscalls supported in the architecture*/
#endif



/** List of custom library descriptors.
 * Each custom library has, inside, a descriptor custom_library_descriptor.
 * This list contains the pointers to all the loaded libraries descriptors, to access the custom syscalls in each library.
 * This allows for multiple custom syscalls.
 *
 * \note If this was a list of pointers to structures, meaning that the actual structure is in the Library itself. This helps reducing the memory requirements.
 */
list * custom_libs_list;

/*! Structure containting the tracee information. All loaded libraries are linked to this structure so that they can access information about the \b tracee, */
tracee_descriptor tracee;

//-------------------------------------------------------------------------------------------------//

void unload_libraries()
{
	custom_library_descriptor* custom_library;
	
	seek(custom_libs_list,0);
	while (has_next(custom_libs_list))
	{
		custom_library = (custom_library_descriptor*)get_next(custom_libs_list);
		if ((custom_library->terminate) != NULL)
				{
				(custom_library->terminate)();
				vprintf(CUSTOM_LIBRARY_END_S,custom_library->name);
				}
		}
}

int add_custom_library(char * filename)
{
	custom_library_descriptor* custom_library;
	tracee_descriptor** tracee_info;
	void* handle;
	void *(funtion)(void);

		handle = dlopen(filename, RTLD_NOW  );
		if (handle==NULL)
		{
			eprintf(ERROR_DLOPEN_S, dlerror());
			return 9;
		}

		//Library file was opened

		dlerror();		//Clearing any previous error

		custom_library = (custom_library_descriptor*) dlsym(handle, CUSTOM_LIBRARY_DESCRIPTOR_SYMBOL);
		if (custom_library == NULL)
		{
			eprintf(ERROR_LOADING_CUSTOM_SYMBOL_S,CUSTOM_LIBRARY_DESCRIPTOR_SYMBOL);
			eprintf(ERROR_S,dlerror());
			return 29;
		}
		dlerror();
		tracee_info = (tracee_descriptor**) dlsym(handle, CUSTOM_TRACEE_DESCRIPTOR_SYMBOL);
		if (tracee_info == NULL)  {
			eprintf(ERROR_LOADING_CUSTOM_SYMBOL_S,CUSTOM_TRACEE_DESCRIPTOR_SYMBOL);
			eprintf(ERROR_S,dlerror());
			return 28;
		}

		//Library loaded ok, checking structure for the library

		if (is_valid_custom_library(custom_library) != RETURN_OK)
		{
			eprintf(ERROR_LOADING_CUSTOM_LIBRARY_S,filename);
			return 39;
		}
		else
		{
			// Library descriptor ok, adding to array

			append_item(custom_libs_list,(custom_library_descriptor*)custom_library);

			//Linking the Library's internal Tracee_Info to the Sandbox Tracee structure
			*(tracee_info) = (tracee_descriptor*) &tracee;


			if ((custom_library->initialize) != NULL)
				{
				(custom_library->initialize)(); //Run the Init function
				vprintf(CUSTOM_LIBRARY_INIT_S,custom_library->name);
				}
		}
		return RETURN_OK;

} //End of funtion

void init_custom_libraries()
{
	custom_libs_list = new_list();
} //End of funtion

custom_syscall_descriptor* get_valid_custom_syscall(custom_library_descriptor* library_descriptor, int syscall_number)
{
	custom_syscall_descriptor* syscall_desc = NULL;
	//cond1 = The syscall index must fit in the array of the library
	if 	(library_descriptor->syscall_descriptor_array_len <= syscall_number)   // <= Has to be, do not ask
		return NULL;
	//cond2 = There is a valid syscall descriptor for  the syscall index
	syscall_desc = &(library_descriptor->syscall_descriptor_array[syscall_number]);
	if (syscall_desc == NULL)
		return NULL;
	//cond3 = The descriptor has to be valid
	if (is_valid_custom_syscall(syscall_desc) != RETURN_OK)
		return NULL;
	return syscall_desc;
}

int is_valid_custom_syscall(custom_syscall_descriptor* syscall_descriptor)
{
	return (
		    //cond 2 = At least one of the functions is implemented, either BEFORE or AFTER
				(syscall_descriptor->custom_syscall_before != NULL) || (syscall_descriptor->custom_syscall_after != NULL)
	) ? RETURN_OK: 9;
}

void print_syscall_descriptor(custom_syscall_descriptor* custom_syscall )
{
		if (custom_syscall == NULL) return;

		printf(CUSTOM_SYSCALL_S_POINTERS_LD,custom_syscall->name,(long)custom_syscall->custom_syscall_before,(long)custom_syscall->custom_syscall_after);

}

int is_valid_custom_library(custom_library_descriptor* library_descriptor)
{
	return (	//cond 1 = Array length within boundaries
			(library_descriptor->syscall_descriptor_array_len >= 0 ) && (library_descriptor->syscall_descriptor_array_len < MAX_SYSCALLS)
		&&  //cond 1 = Array pointer is not NULL
			(library_descriptor->syscall_descriptor_array != NULL)
	) ? RETURN_OK: 9;
}

void print_library_descriptor(custom_library_descriptor* library_descriptor )
{
		if (library_descriptor == NULL) return;

		printf(CUSTOM_LIBRARY_S_D_POINTER_LD,library_descriptor->name,library_descriptor->syscall_descriptor_array_len,(long)library_descriptor->syscall_descriptor_array);

}
