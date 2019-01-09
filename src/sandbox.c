/*! \file sandbox.c
    \brief Sandbox program
    \authors Ignacio TAMAYO and Vassanthaphriya VIJAYAN
	\date May 6th 2016
	\version 1.3


    1) Dynamically loads the custom libraries indicated by the command line.
       If any option is invalid, the sandbox stops executing.
       All libraries have to be loaded before execution (opts.c).
       All the custom libraries are inspected for Custom Syscalls (dynlib.c), if any error is found the execution is aborted.

   3) After a message and confirmation by the user ([ENTER]), it will start the \b tracee as a child process, and will monitor its system calls.
    In EXPLICIT/DUBUG mode, for each captured systemcall, the syscall will be printed in STDERR.   (  \link trace.c  \endlink )

    4) When the \b tracee ends, the sandbox also ends

    \see trace.c opts.c dynlib.c

    \note Working on i386 and x86_64 architectures

    \warning The \b tracee string does not support pipes or any redirection ( |, <, >, >>).
    \warning The \b tracee childs are not monitored
	\warning The \b tracee signals are not monitored, only SIGTERM|SIGSTOP. See \ref trace.c for details on SIGNALs
	
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
#include <stdlib.h>
#include <stdio.h>
#include "trace.h"		// Functions for tracing syscalls
#include "opts.h"		// Functions for treating command options
#include "dynlib.h"		// Functions for loading the dyamic libraries
#include "messages.h"	// Functions for error messages printing


/*! Main
 * Processes the options, loads the libraries, loads the CustomSyscalls, executes and monitors the process
 * \param argc number of arguments
 * \param argv array of strings of the arguments
 * \return 0 if execution was sucessful, <>0 otherwise
*/

int main (int argc, char **argv)
{
  	int c;
	pid_t pid;	// The Child PID

	printf(TITLE_VERSION);

	//Libraries array is initialized
	init_custom_libraries();

	c = (process_options(argc, argv) );
	if (c != OPTIONS_OK_CONTINUE)
		exit(c);


	//printf(LF_CR);
	printf(LIBRARIES_LOADED_D,custom_libs_list->counter );
	//printf(LF_CR);

	//Pressing ENTER is required after loading the libraries
	//PRINTF_CONTINUE();
	if (execTreeOutputFlag == TRUE)
		{
			print_execution_plan();
			unload_libraries();
			exit(0);
		}

	printf(LINE);

	switch (pid=fork())
	{
	case 0:  //Child
		execv (argv[optind], argv+optind);
		execvp (argv[optind], argv+optind);
		eprintf(ERROR_EXEC_S,argv[optind]);		//Print Error
		exit(39);		//Reaching this point means that the child failed to change the environment
		break;
	case -1:
		perror("Fork: ");
		eprintf( ERROR_FORK);		//Print Error
		exit(49);
	break;
	default:
		c = trace_PID(pid);
		//check_child_processes();
		printf(LINE);
		printf(TRACEE_END_D,c);
		// Once the tracePID return, is because the Child PID died
		unload_libraries();
	break;
	}//End of Switch

	printf("\n");
	return 0;
} //End of Function
