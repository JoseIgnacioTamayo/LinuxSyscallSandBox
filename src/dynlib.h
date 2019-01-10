/*! \file dynlib.h
    \brief Functions dedicated to loading the dynamic libraries, handling their pointers and calling functions inside

	\authors Ignacio TAMAYO and Vassanthaphrya VIJAYAN
	\date August 2016
	\version 1.4

	\see dynlib.c sandbox.c

	\internal
	The dynlib.c allocates the global variables mentioned here
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


#ifndef INC_DYNLIB	//Lock to prevent recursive inclusions
#define INC_DYNLIB

#include "sandbox_customsyscall_descriptor.h"
#include "list.h"



/** List of pointers to library descriptors */
extern list* custom_libs_list;

/*! Structure containting the tracee information. */
extern tracee_descriptor tracee;

/*! Unloads the dynamic libraries, if any. Also frees the allocated memory for the custom syscall descriptors, if any.
 *
 * While unloading, the terminate() functions of each library is called
*/
void unload_libraries();


/*! \brief Adds a given custom library to the list, if it passes the checks.
 * Checks the library pointed by the argument and, if OK, adds it to the array of libraries descriptors custom_libs_list.
 * To work, the library being added MUST have a valid structure custom_library_descriptor inside, called like the macro CUSTOM_LIBRARY_DESCRIPTOR_SYMBOL.
 * To work, the library being added MUST have a structure pointer tracee_descriptor inside, called like the MACRO CUSTOM_TRACEE_DESCRIPTOR_SYMBOL.
 * When loading the library, the initialize() method of the library is called
 * When loading the library, the internal  tracee_descriptor is pointed to the location where is information resides in the Sandbox.
 * \see  sandbox_customsyscall_descriptor.h
 * \param filename is the filename of the library to load, in FULL PATH format
 * \return RETURN_OK if loading OK, <>RESULT_OK otherwise
*/
int add_custom_library(char * filename);

/*! Initializes the structure to hold the custom libraries
*/
void init_custom_libraries();

/*! Looks if there is a custom syscall registered for execution in the library.
 * Internally checks for validity of the structure, using \c is_valid_customsyscall().
 * \param syscall_number The number of the requested syscall.
 * \param library_descriptor Library where to look for the custom syscall
 * \return the custom syscall descriptor associated with the syscall, or NULL if there is none
*/
custom_syscall_descriptor* get_valid_custom_syscall(custom_library_descriptor* library_descriptor, int syscall_number);

/*! Checks if the descriptor has a valid Syscall number, a not-null function pointer and a not-null name.
 * \param syscall_descriptor is the structure to check
 * \return RETURN_OK if structure is valid, <>RESULT_OK if not found or invalid parameters
*/
int is_valid_custom_syscall(custom_syscall_descriptor* syscall_descriptor);

/*! Checks if the descriptor has a valid Syscall number, a not-null function pointer and a not-null name.
 * \param library_descriptor is the structure to check
 * \return RETURN_OK if structure is valid, <>RESULT_OK if not found or invalid parameters
*/
int is_valid_custom_library(custom_library_descriptor* library_descriptor);

/*! Prints some values of the Custom Syscall Descriptors passed as parameter.
 * Does print in VERBOSE mode only.
 * \pre custom_syscall cannot be NULL
 * \param custom_syscall is the Descriptor to print information from
 * \internal
*/
void print_syscall_descriptor(custom_syscall_descriptor* custom_syscall );

/*! Prints some values of the Custom Library Descriptors passed as parameter, if VERBOSE
 * \pre custom_library cannot be NULL
 * \param custom_library is the Descriptor to print information from
 * \internal
*/
void print_library_descriptor(custom_library_descriptor* custom_library );




#endif
