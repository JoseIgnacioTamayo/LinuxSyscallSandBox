/*! \file trace.c
    \brief Functions in charge of capturing the Syscalls of a tracee process
  	\authors Ignacio TAMAYO and Vassanthaphrya VIJAYAN
	\date Augost 2016
	\version 1.4
	  
    Traces a PID constantly, looking for syscalls. It can also trace the Forked/Cloned process.
    * 
    The Trace is interrupted always twice, BEFORE the SYSCALL Kernel and AFTER the Syscall Kernel.
    * 
    There is a mechanism to properly catch both individually for each PID, so concurrent order of processes and calls is not a problem.
    * 
    * To offer information to the libraries, this module fills up every time the fields in the structure tracee_descriptor tracee.
    * 
    *  
    * 
	 
	\see trace.h
	
	\internal
	
	* To support child \b tracee (a \b tracee that uses fork() to create child processes), a list of structures tracee_flow_descriptor is kept. 
    * For every child there is an entry that contains the Sandbox Custom Syscall Processing state. 
    * This structure is loaded and saved every time it processes an interruption for a PID, let it be parent of child, incoming or outgoing. 
    * This is necessary to provide the libraries with information about the precise PID that is actually requesting the syscall among the several possible, and in which state is the syscall execution.
    * 
    * Luckily the PTRACE is able to get the ThreadID just like the Child PID, so for Threaded processes they are identified just like normal forked processes.
    * 
    * When calling for the custom syscall, function calling convention for the architectures is discussed [here]:( https://en.wikipedia.org/wiki/X86_calling_conventions#x86-64_calling_conventions).
    * Basically, all 6 registers are passed to the custom syscall, just as it would be for the kernel syscall. 
    * As the syscall has the same signature as the original syscall, it will take only the needed arguments via the registers.
    * 
   	
 * */

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
#include <sys/wait.h>		// For waitforpid())
#include <sys/ptrace.h>		// Trace related functions

#include <sys/reg.h>		// Registry description for the architecture
#include <sys/user.h>		// User space Registry description for the architecture
#include <string.h>			// Neede for strcpy
#include <errno.h>			// Needed for errors in PTRACE calls
#include <pthread.h>		// To support threading
		
#include "trace.h"
#include "messages.h"
#include "dynlib.h"
#include "list.h"

#ifdef __x86_64__							// Architecture of the running PC is 64 bits
		#define REG_AX_ORIG	regs.orig_rax	
		//!< Original value of the EAX register at the syscall 
		#define REG_AX	regs.rax			
		//!< Processor register 
		#define REG_BX	regs.rbx			
		//!< Processor register 
		#define REG_CX	regs.rcx			
		//!< Processor register 
		#define REG_DX	regs.rdx			
		//!< Processor register 
		#define SYSCALLS_ARGS_REGS  regs.rdi, regs.rsi, regs.rdx, regs.r10, regs.r8, regs.r9
		#define DUMMY_SYSCALL	39	
		//!< Syscall that is actually called in kernet when a library function was performed in place of the real Syscall
		
		typedef unsigned long long int cpu_reg;

#endif
#ifdef	__i386__							// Architecture of the running PC is 32 bits
		#define REG_AX_ORIG	regs.orig_eax   //Original value of the EAX register at the syscall  
		#define REG_AX	regs.eax			// Processor register  
		#define REG_BX	regs.ebx			// Processor register  
		#define REG_CX	regs.ecx			// Processor register  
		#define REG_DX	regs.edx			// Processor register  
		#define SYSCALLS_ARGS_REGS  regs.ebx, regs.ecx, regs.edx, 	regs.esi, 	regs.edi, regs.ebp
		#define DUMMY_SYSCALL	20	
		// Syscall that is actually called in kernet when a library function was performed in place of the real Syscall 
		
		typedef long int cpu_reg;

#endif

#ifdef __x86_64__	
	#define MAX_SYSCALLS 		316				
	/** Maximum ammount of syscalls supported in the architecture*/
#endif
#ifdef __i386__	
	#define MAX_SYSCALLS 		358				
	/** Maximum ammount of syscalls supported in the architecture*/
#endif

//-------------------------------------------------------------------------------------------------------------------------------------

/*! Prints to STDOUT a sigle line with the value of the register used for calling a syscall
 * \param regs is the structure of the registers, taken by PTRACE(GETREGS)
 * \deprecated
 * \remark Use for Debugging
*/
void dump_regs(struct user_regs_struct *regs)
{
#ifdef __x86_64__	
	dprintf( "AX_ORIG %ld AX %ld RDI %ld RSI %ld RDX %ld\n",
	(long int)regs->orig_rax,(long int)regs->rax, (long int)regs->rdi, (long int)regs->rsi, (long int)regs->rdx);
#endif	
#ifdef	__i386__	
	dprintf( "AX_ORIG %ld AX %ld BX %ld CX %ld DX %ld\n",
			(long int)regs->orig_rax,(long int)regs->rax,(long int)regs->rbx,(long int)regs->rcx,(long int)regs->rdx);
#endif
}

list * child_tracees_list;	//!< List of Tracees to be monitored

struct user_regs_struct regs;				//!< Structure to operate the CPU registers 


//-------------------------------------------------------------------------------------------------------------------------------------

tracee_flow_descriptor* find_child_tracee(pid_t pid)
{
	tracee_flow_descriptor* tracee_desc;	
	
	seek(child_tracees_list,0);
	while(has_next(child_tracees_list))
	{
		tracee_desc = (tracee_flow_descriptor*)get_next(child_tracees_list);
		dprintf("Looking for PID %d , checking %d\n");
		if (tracee_desc->pid == pid)
			return tracee_desc;
	}
	return NULL;
}


void add_child_tracee(pid_t pid)
{
	tracee_flow_descriptor * tracee_desc;
	
	tracee_desc = (tracee_flow_descriptor*)malloc(sizeof(tracee_flow_descriptor));
	tracee_desc->pid = pid;
	tracee_desc->return_value = DEFAULT_RETURN_VALUE ;	
	tracee_desc->kernel_return_value = DEFAULT_RETURN_VALUE;
	tracee_desc->expecting_syscall_return=FALSE;
	tracee_desc->expecting_dummy = FALSE;
	tracee_desc->is_custom_syscall=FALSE;
	tracee_desc->kernel_executed=FALSE;

	append_item(child_tracees_list,(void*)tracee_desc);
	
	dprintf("Added PID %d to list \n",pid);
}


void print_child_tracee()
{
	tracee_flow_descriptor* tracee_desc;
	int i = 0;
	seek(child_tracees_list,0);
	//printf("List Head %ld Cursor %ld \n ",child_tracees_list->head, child_tracees_list->cursor  );
	while(has_next(child_tracees_list))
	{
		i++;
		tracee_desc = get_next(child_tracees_list);
		dprintf(" Item %d : PID %d ,",i,tracee_desc->pid);
	}
	dprintf("\n");
	
}

void delete_child_tracee(pid_t pid)
{
	tracee_flow_descriptor * tracee_desc;
	
	seek(child_tracees_list,0);
	while(has_next(child_tracees_list))
	{
		tracee_desc = get_next(child_tracees_list);
		if (tracee_desc->pid == pid)
		{
			
			delete_item(child_tracees_list,(void *)tracee_desc);
			free(tracee_desc);
			dprintf("Deleted PID %d from list \n",pid);
		}
	}
	
	
}

int trace_PID(pid_t pid)
{
	
	int status;										
	int ret = DEFAULT_RETURN_VALUE;		
	pid_t a_pid, b_pid, main_pid;		
	tracee_flow_descriptor* tracee_desc; // To operate the list of Tracee Processes
	int eventId;						// To get the PTRACE Event Msg for capturing clone() and fork()
	int signal = 0;
	
	//Preparing the list of at least 1 process to trace
	main_pid = pid;
	child_tracees_list = new_list();
	add_child_tracee(pid);
	
	ptrace (PTRACE_ATTACH, pid, 0, 0);
	waitpid (pid, 0, 0 ); 			//WCONTINUED | WEXITED | WSTOPPED does not work
	
	ptrace (PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD  | PTRACE_O_EXITKILL  );
	
	if (childProcessFlag == TRUE) 
		ptrace (PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD  | PTRACE_O_EXITKILL | PTRACE_O_TRACEFORK | PTRACE_O_TRACECLONE);

	vprintf(STARTING_TRACE_D,pid);
	ptrace (PTRACE_SYSCALL, pid, 0, 0);
	
	while(1)
	{
		// Wait for the Child process to be stopped by PTRACE or by someone else
		// __WALL to be interrupted by Threaded and Forked childs. WDCONTINUED is not recomened
		// The Tracer expects events from the tracee via waitpid(). It also returns when the children processes die or send events.
    
  		if ( ( a_pid=waitpid (-1, &status,  __WALL)) == -1) 		
		{			
			eprintf(TRACEE_ERROR_D,a_pid);
			break;		
			//Get out if error at waiting for PID. 
				
		}
		if (WIFSIGNALED(status))  //The child process was terminated by a signal
		{	 
			vprintf(TRACEE_EXIT_BY_SIGNAL_D,WTERMSIG(status));
			
			if (a_pid != main_pid)
			{
				dprintf("A Child/Thread did exit, detaching \n");
				if ((childProcessFlag == TRUE) )		
					delete_child_tracee(a_pid);
				if (ptrace(PTRACE_DETACH, a_pid, 0, 0))
					dprintf("Error detaching\n");
			}
			else
			{
				dprintf("Main tracee did exit \n");
				vprintf(TRACEE_EXIT); 
				ret = WEXITSTATUS(status); 
				break;
			}
		}
		if (WIFEXITED(status))	  //Signal to a parent when a child exits normally. Execv() also caused this call to be true
		{ 
			// This is a notification to the Parent, just acknowledge the signal and let the child die in peace.

			if (a_pid != main_pid)
				dprintf("Child %d exits normally\n",a_pid);

			else
			{
				vprintf(TRACEE_EXIT); ret = WEXITSTATUS(status); break;
			}
		}	
		if  (WIFSTOPPED(status)) 	// PTRACE has several types of STOP situations, Signal-Delivery-Stops, Syscall-Stops, Group-stops, etc
		{ 
			signal = 0;  // As there is a signal that was captured, a signac can be delivered after for the tracee to continue. 0 means that the signal is just ignored
			
			if (( status>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8))) || (( status>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8))) ))  
			// Textual from man ptrace, means that the Process has forked (requires PTRACE_O_TRACEFORK option)
			// Textual from man ptrace, means that the Process has been cloned (requires PTRACE_O_TRACECLONE option)
			{
				if (childProcessFlag == TRUE) 
				{
					if (ptrace(PTRACE_GETEVENTMSG, a_pid, 0, &b_pid) == 0)
					{
						
						//Register this new child
					
						add_child_tracee(b_pid);
						ptrace (PTRACE_SYSCALL, b_pid, 0, 0);
					
						if ( status>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8)))
							vprintf(TRACKING_FORKED_D,b_pid);
						else
							vprintf(TRACKING_CLONED_D,b_pid);
					}
					else
						dprintf("Unable to read the Registers from PID %d \n", a_pid);
					//catch_exit_after_clone = 1; 	
					// When Cloned/Forked, there will be a EXECV call that would cause the Parent to Receive a WIFEXITED call that does not mean an actual exit.
				}
			}
			
			else if (WSTOPSIG(status) == (SIGTRAP | 0x80) )	//Tracee stopped by syscall
			{	
				dprintf("Syscall for pid %d\n",a_pid);
				//print_child_tracee();
				
				if ( (tracee_desc = find_child_tracee(a_pid)) != NULL) 
				{
					dprintf("Found PID in the Tracee list \n");
					if (ptrace(PTRACE_GETREGS, tracee_desc->pid, 0, &regs) == 0)  //If there was no trouble getting the Registers
						syscall_flow(REG_AX_ORIG,tracee_desc);
				}
			}
			else   			// Another reason has occurred for the Tracee to be stopped
			{
				dprintf(STOP_CAPTURED_H_D, WSTOPSIG(status), a_pid );
								
								////Do not do exit here, this STOP is signal delivery, not process being stopped
								
				////Checking for Group Stop
				
				if  (( WSTOPSIG(status) == SIGSTOP ) || (  WSTOPSIG(status) ==SIGTSTP ) || (  WSTOPSIG(status) == SIGTTIN) || (  WSTOPSIG(status) ==SIGTTOU) )
					dprintf("Group stop\n");
				else if ( WSTOPSIG(status) == SIGTRAP )
					dprintf("TRAP stop\n");
				else
				{
					dprintf("Other stop\n");
					signal = SIGCHLD;
					//signal = WSTOPSIG(status);
				}
			}
		
			//In any case, as the Process is stopped, it is restarted by a SYSCALL continue, passing the Signal
			if (ptrace (PTRACE_SYSCALL, a_pid, 0, signal))
				dprintf("Error continuing pid %d with signal %d\n", a_pid, signal);
		
		} //End If WIFSTOPPED
		
			
		if (WCOREDUMP(status))
		{
			vprintf(CORE_DUMP);
			break;
		}
		if (WIFCONTINUED(status))
		{
			vprintf(CONTINUED);
			
		}
		
	} //End of While
	return ret;
} //End of tracePID


void syscall_flow( int syscall_number, tracee_flow_descriptor* tracee_desc)
{
	if ((syscall_number < 0) || (syscall_number > MAX_SYSCALLS))
		return ;
		//Invalid Syscall number, not doing anything
	
	if (! (tracee_desc->expecting_syscall_return))	
	{
		//New Syscall
		tracee_desc->expecting_syscall_return=TRUE;
		tracee_desc->expected_syscall = syscall_number;
		tracee_desc->expecting_dummy=FALSE;
		
		dprintf("In Syscall for syscall %d for pid %d\n",syscall_number,tracee_desc->pid);
		
		processInSyscall(tracee_desc);
			
			
	} //End if New Syscall
	else 	
	{
		// Return from previous syscall
		if ((tracee_desc->expected_syscall == syscall_number) || (tracee_desc->expecting_dummy))		
		{
			//If it is the syscall we were waiting
			tracee_desc->expecting_syscall_return=FALSE;
			
			dprintf("Out Syscall for syscall %d for pid %d\n",syscall_number,tracee_desc->pid);
			
			processOutSyscall(tracee_desc);
		}
		else 
		{
			//We were not expecting this, treat as a a new syscall
			tracee_desc->expecting_syscall_return=TRUE;
			tracee_desc->expected_syscall = syscall_number;
			tracee_desc->expecting_dummy=FALSE;
			
			dprintf("In Syscall for syscall %d for pid %d\n",syscall_number,tracee_desc->pid);
			
			processInSyscall(tracee_desc);
		}
	}
}

void processInSyscall(tracee_flow_descriptor* tracee_desc)
{
	 
	cpu_reg custom_result;
	int i ,no_kernel =FALSE;
	custom_library_descriptor* custom_library;
	custom_syscall_descriptor* custom_syscall;	
	custom_result = DEFAULT_RETURN_VALUE;
	
	//dprintf("ENTERING\n");
	
	//Fill the structure that the Library can read/write
	tracee.trace_PID = tracee_desc->pid;
	tracee.return_value = tracee_desc->return_value;
	tracee.kernel_return_value = tracee_desc->kernel_return_value;
	
	// Browse all the custom libraries
	
	goto_first(custom_libs_list);
	while(has_next(custom_libs_list))
	
	{
		
		custom_library = get_next(custom_libs_list);
			
		custom_syscall = get_valid_custom_syscall(custom_library,tracee_desc->expected_syscall);
		
		if (custom_syscall != NULL)  
		{
			//If custom syscall
			tracee_desc->is_custom_syscall=TRUE;
			vprintf(CUSTOM_SYSCALL_CATCHED_S_FROM_S,custom_syscall->name,custom_library->name );
			 
			if ((custom_syscall->custom_syscall_before) != NULL)
			{
				vprintf(CUSTOM_SYSCALL_CALLED_BEFORE );  
				custom_result = (custom_syscall->custom_syscall_before)(SYSCALLS_ARGS_REGS);  
				
				// Executing the Custom Syscall and keeping the result value
				
				if (((custom_syscall->flags) & FLAG_QUIT_IF_RETURN_NEGATIVE) && (custom_result <0) ) 
					break; //Quit of error option
					
				if (! ((custom_syscall->flags) & FLAG_KEEP_PREVIOUS_RETURN) ) 
					tracee.return_value = custom_result; // If not said the opossite, drag the return code along
					
			} // End If execute before
			
			if (((custom_syscall->flags) & FLAG_DONT_CALL_KERNEL))
			{	
				//If kernel syscall is not to be called
				vprintf(CUSTOM_SYSCALL_NOKERNEL );
				no_kernel = TRUE;
			}    
			vprintf(LF_CR);
			
		}
	} //end For each lib in custom_libs
	if (no_kernel){
		REG_AX_ORIG = (cpu_reg) DUMMY_SYSCALL; 			//REG_AX is not used, as determined from experimentation 
		ptrace (PTRACE_SETREGS, tracee_desc->pid, 0, &regs);	//Write the new syscall number for the kernel		
		tracee_desc->expecting_dummy = TRUE;					//Tell that the kernel will reply for the dummy syscall
		}
	//Storing changes
	tracee_desc->return_value = tracee.return_value;
}

void processOutSyscall(tracee_flow_descriptor* tracee_desc)
{
	int i ;
	custom_library_descriptor* custom_library;
	custom_syscall_descriptor* custom_syscall;
	cpu_reg custom_result;
	char * valid_syscall_name; 
	
	//Needed to store the valid name to print as, perhaps, we roll all the libraries and lost track of the only descriptor that had the name.
	
	if (tracee_desc->is_custom_syscall)
	{
		//If custom syscall, fill the structure that the Library can read/write
		tracee.trace_PID = tracee_desc->pid;
		tracee.return_value = tracee_desc->return_value;
		tracee.kernel_return_value = tracee_desc->kernel_return_value;
			
		if (tracee_desc->expecting_dummy) 				
		{	
			tracee.kernel_return_value = DEFAULT_RETURN_VALUE;
			tracee.kernel_executed = FALSE;
		}
		else 				
		{
			tracee.kernel_return_value = REG_AX;
			tracee.kernel_executed = TRUE;
			tracee.return_value = REG_AX;
		}
		
		goto_last(custom_libs_list);
		while(has_next(custom_libs_list))
	
		{
		
			custom_library = get_previous(custom_libs_list);
			//dprintf("Lib number %d\n",i);
			//custom_library = custom_libs[i];
			custom_syscall = get_valid_custom_syscall(custom_library,tracee_desc->expected_syscall);
			
			
			if (  custom_syscall != NULL )  
			{
				//If custom syscall found in this library
												
				valid_syscall_name = custom_syscall->name;
				if ((custom_syscall->custom_syscall_after) != NULL)
				{
					vprintf(CUSTOM_SYSCALL_CALLED_AFTER );  
								//dprintf("Calling Pointer %ld",(long) (custom_syscall->custom_syscall_after) );
					custom_result = (custom_syscall->custom_syscall_after)(SYSCALLS_ARGS_REGS);
					
					//Executing the Custom Syscall and keeping the result value
					if (((custom_syscall->flags) & FLAG_QUIT_IF_RETURN_NEGATIVE) && (custom_result <0) ) 
						break;//Quit of error option
					if (! ((custom_syscall->flags) & FLAG_KEEP_PREVIOUS_RETURN) )
						tracee.return_value = custom_result;
					
				}				
			}	
		}
		
		vprintf(CUSTOM_SYSCALL_S_RET_D,valid_syscall_name, (int) tracee.return_value  );   
		 
		REG_AX = tracee.return_value;   // REG_AX_ORIG still contains the old Syscall number, so RAX is where the Result is. Confirmed by experimentation
		ptrace(PTRACE_SETREGS, tracee.trace_PID, 0, &regs);	//Write the result value for the tracee to receive	
		tracee_desc->is_custom_syscall = FALSE;
		
		//Storing changes
		tracee_desc->return_value = tracee.return_value;
		tracee_desc->kernel_return_value = tracee.kernel_return_value;
		tracee_desc->kernel_executed = tracee.kernel_executed;
	}
	
	
}

void print_execution_plan(void)
{
	custom_library_descriptor* custom_library;
	custom_syscall_descriptor* custom_syscall;
	int i,j,k;
	char syscall_number = -1;
	char no_kernel = FALSE;
	
	for(i=0;i<MAX_SYSCALLS;i++)
	{
		syscall_number = -1;
		no_kernel = FALSE;
		
		// Checking the Syscalls on the BEFORE execution
		
		goto_first(custom_libs_list);
		while(has_next(custom_libs_list))
	
		{
			custom_library = get_next(custom_libs_list);
			//custom_library = custom_libs[j];
			custom_syscall = get_valid_custom_syscall(custom_library,i);
			if (custom_syscall != NULL)
			{
				if(syscall_number == -1) 
				{
					printf(CUSTOM_SYSCALL_CATCHED_D,i);
					syscall_number = 1;
				}
				
				for(k=0;k<syscall_number;k++)printf(SPACE); //Tabs needed to make the tree like a tree
				syscall_number++;
				
				printf(CUSTOM_LIB_CALLED_S_S,custom_syscall->name ,custom_library->name );
				
				if ((custom_syscall->custom_syscall_before) != NULL)
					{
						printf(CUSTOM_SYSCALL_CALLING_BEFORE );  
						if (((custom_syscall->flags) & FLAG_QUIT_IF_RETURN_NEGATIVE) ) 
						printf(CUSTOM_SYSCALL_QUIT_ON_ERROR);
						if ( ((custom_syscall->flags) & FLAG_KEEP_PREVIOUS_RETURN) ) 
						printf(CUSTOM_SYSCALL_KEEP_RESULT);
					}
				if (((custom_syscall->flags) & FLAG_DONT_CALL_KERNEL))
				{
					printf(CUSTOM_SYSCALL_NOKERNEL );
					no_kernel = TRUE;
				}
				printf(LF_CR);
			}
			
		}
		
		// Checking the Syscalls on the AFTER execution of KERNEL
		
		if((syscall_number != -1) )
		{
			for(k=0;k<syscall_number;k++)printf(SPACE); //Tabs
			if (no_kernel)
					printf(NO_KERNEL_SYSCALL);
			else
					printf(KERNEL_SYSCALL);
		}
		goto_last(custom_libs_list);
		while(has_next(custom_libs_list))
	
		{
			custom_library = get_previous(custom_libs_list);
			//custom_library = custom_libs[j];
			custom_syscall = get_valid_custom_syscall(custom_library,i);
			if (custom_syscall != NULL)
			{
				
				syscall_number--;
				if ((custom_syscall->custom_syscall_after) != NULL)
					{
						for(k=0;k<syscall_number;k++)printf(SPACE); //Tabs
						printf(CUSTOM_SYSCALL_CALLING_AFTER );  
						if (((custom_syscall->flags) & FLAG_QUIT_IF_RETURN_NEGATIVE) ) 
						printf(CUSTOM_SYSCALL_QUIT_ON_ERROR);
						if ( ((custom_syscall->flags) & FLAG_KEEP_PREVIOUS_RETURN) )
						printf(CUSTOM_SYSCALL_KEEP_RESULT);
						printf(LF_CR);	
					}
				
			}
				
		}
	}
		
}
