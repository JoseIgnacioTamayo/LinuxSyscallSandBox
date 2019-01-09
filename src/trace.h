/*! \file trace.h
    \brief Functions dedicated to tracing a process for Syscalls
	\authors Ignacio TAMAYO and Vassanthaphrya VIJAYAN
	\date August 2016
	\version 1.4
	
	 * Traces a PID constantly, looking for syscalls. It can also trace the Forked/Cloned process.
	 * 
	 * To offer information to the libraries, this module fills up every time the fields in the structure tracee_descriptor tracee.
	 * 
	 * \c trace_PID() returs when the main \b tracee has finished or because of a terminating Signal Received.
	 * 
	\see trace.c sandbox.c

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
 
 
/** When the custom libraries are called for a Syscall, this is the default Return value used through the chain of custom functions. This is related to the option  */ 
#define DEFAULT_RETURN_VALUE	-1 
 
 
 /** Structure for the Sandbox Custom Syscall Processing state, for each monitored PID */
typedef struct {
	pid_t pid;						
	long int return_value ;			//!< The return value to be delivered to the \b tracee. This is changed by the returns of the custom syscalls
	long int kernel_return_value;	//!< The return value delivered by the kernel, it it was executed. DEFAULT_RETURN_VALUE if not 
	int expected_syscall;			//!< Last Syscall number for this PID that was captured. This is used to look for libraries implementing the AFTER KERNEL of this syscall.
	char expecting_syscall_return;  //!< True if the kernel return of the syscall is expected. Makes the difference between the BEFORE and AFTER kernel.
	char expecting_dummy ;			//!< True is a dummy syscall was triggered to implement the FLAG_DONT_CALL_KERNEL option
	char is_custom_syscall;			//!< True if there is a custom library that implements the syscall just interrupted
	char kernel_executed;			//!< True if the Kernel was executed in the process of the Syscall tracing
}
tracee_flow_descriptor;

/*! Starts tracking the syscalls of the given PID. Returns when the PID dies.
 * 
 * This function performs the syscall tracing, polling the libraries when a syscall is captured for any PID being traced.
 * 
 * Once a syscall is captured, all the libraries are polled to see who has a custom function to execute BEFORE the Kernel
 * All BEFORE functions are executed sequentially, from the first library to the last.
 * 
 * Then the Kernel is called, if not overriden by a FLAG option
 * 
 * Once the Kernel finishes, all the libraries are polled to see who has a custom function to execute AFTER the kernel. 
 * All AFTER functions are executed sequentially, from the last library to the first
 * 
 * The last custom function to be executed is the one providing the retun value to the tracee, unless overrriden by FLAGS in the process. The return value is taken from the first BEFORE execution to the last AFTER execution, passing and being modified by the custom syscalls.
 * 
 * If QUIT_IF_RETURN_NEGATIVE present, in the middle of the process, if a custom syscall returns a negative value, the sequence is aborted and jumps directly either to normal kernel syscall (BEFORE path aborted) or returns to the tracee (AFTER path aborted).
 * 
 * If the main \b tracee creates children processes or threads, they are also monitored in the same way as the main process.
 * 
 * \param pid The PID of the child to be monitored
 * \return the exit value of the Tracee, or -1 if terminated by signals
*/ 
int trace_PID(pid_t pid);


/*! Performs an analysis of the loaded libraries and prints to STDOUT the execution TREE, what functions will be executed,
 * from which library and in which order.
 * 
 * Used with option -t in the command line
 * 
 * All BEFORE functions are executed sequentially, from the first library to the last.
 * 
 * Then the Kernel is called, if not overriden by a FLAG option
 * 
 * Once the Kernel finishes, all the libraries are polled to see who has a custom function to execute AFTER the kernel. 
 * All AFTER functions are executed sequentially, from the last library to the first
 * 
 * The last custom function to be executed is the one providing the retun value to the tracee, unless overrriden by FLAGS in the process. The return value is taken from the first BEFORE execution to the last AFTER execution, passing and being modified by the custom syscalls.
 * 
 * If QUIT_IF_RETURN_NEGATIVE present, in the middle of the process, if a custom syscall returns a negative value, the sequence is aborted and jumps directly either to normal kernel syscall (BEFORE path aborted) or returns to the tracee (AFTER path aborted).
 * 
*/ 
void print_execution_plan(void);


/** This function add a pid to the list of monitored pids.
 * \pre pid must be unique in the list.
 * \param pid to be added.
 * */
void add_child_tracee(pid_t pid);

/** This function returns a pointer to the tracee_flow_descriptor given the pid of the process.
 * \param pid 
 * \return the pointer to the appropriate structure tracee_flow_descriptor, or NULL if the pid is not found in the list.
 * */
tracee_flow_descriptor* find_child_tracee(pid_t pid);

/** Prints the list of Tracee processes being traced
 * \remark Use for debugging
 * */
void print_child_tracee();


/*! Process the treatment of the syscalls, when the kernel has finished and it is about to return to the tracee.
 * \param tracee_desc contains the information about the Syscall Flow state for this PID 
*/
void processOutSyscall(tracee_flow_descriptor* tracee_desc);

/*!  Process the treatment of the syscalls, just after being called by the tracee
 * 
 * \param tracee_desc contains the information about the Syscall Flow state for this PID 
*/
void processInSyscall(tracee_flow_descriptor* tracee_desc);

/** This function coordinates the Ptrace interrcuptions into the appropriate syscall calling flow.
 * This function calls either processInSyscall() or processInSyscall() acording to the state of the variables expecting_syscall_return, expected_syscall and expecting_dummy
 * \param syscall_number is the received syscall at interruption.
 * \param tracee_desc contains the information about the Syscall Flow state for this PID 
 * */
void syscall_flow( int syscall_number, tracee_flow_descriptor* tracee_desc);

/** This function deletes a pid from the array of monitored pids.
 * 
 * \pre pid must be unique in the list. Otherwise, only the first match is deleted.
 * \param pid to be removed
 * */
void delete_child_tracee(pid_t pid);
