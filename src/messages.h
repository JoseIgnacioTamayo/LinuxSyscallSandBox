/*! \file messages.h
    \brief Macros for printing messages, the messages themselves
	\authors Ignacio TAMAYO and Vassanthaphrya VIJAYAN
	\date August 2016
	\version 1.4

	\note About variable arguments in MACROS, [here](https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html)

	\remark In order to keep the MACROS for the printing functions, the messages Prefixes are set in the message definition.
	The other option would be to set them in the MACRO or a FUNCTION, but this did complicated the code.
	So set the appropriate prefix in the Message definition.
	Also this way there can be intermediate messages that will be built on the fly, so not all the messages needed the prefix (case the message CUSTOM_SYSCALL_CALLED_BEFORE)

*/

// Message Prefixes
#define		SBOX_ERR	"SBOX-ERROR:"
#define		SBOX_INFO	"SBOX-INFO:"
#define		SBOX_DEBUG	"\t\t\tSBOX-DEBUG:"


/** If defined, dprintf messages are printed to STDOUT.*/
//#define DEBUG
#ifdef DEBUG
	#define dprintf(...)	 fprintf(stdout, __VA_ARGS__)
#else
	#define	dprintf(...)
#endif

/** Prints Error messages into STDERR */
#define eprintf(...) 	fprintf(stderr, __VA_ARGS__)

/** Prints messages to STDOUT if *VERBOSE* mode is enabled.*/
#define vprintf(...)	if(explicitOutputFlag) fprintf(stdout, __VA_ARGS__)


/** Asks for an ENTER press via STDIN*/
#define PRINTF_CONTINUE() printf(CONTINUE_ENTER);getchar()


//-------------------------------------------------------------------------------------------------------------------------------------


#define UNEXPECTED_ERROR	SBOX_ERR"Unexpected error \n"
#define ERROR_S				"ERROR: %s \n"

//From sandbox.c
#define ERROR_EXEC_S			SBOX_ERR"Unable to create Process %s\n"
#define ERROR_FORK 				SBOX_ERR"Unable to fork process\n"
#define TRACEE_END_D			SBOX_INFO"Tracee terminated with return value %d\n"

//From opts.c
#define ERROR_OPT_L_MISSING_ARG 	SBOX_ERR"Option -l requires the library filename as an argument.\n"
#define ERROR_OPT_LL_MISSING_ARG 	SBOX_ERR"Option -L requires the path as an argument.\n"
#define ERROR_UNKNOWN_OPT_C 		SBOX_ERR"Unknown option `-%c'.\n"
#define ERROR_OPT_MISSING_CMD		SBOX_ERR"No Command to execute as Tracee.\n"
#define INVALID_PATH_S				SBOX_ERR"Wrong path  '%s', please provide a valid path\n"
#define LIB_IS_S					SBOX_INFO"Library entered is %s\n"
#define LIB_CHECK_IS_S				SBOX_INFO"Library %s is being verified \n"
#define LIB_FOUND_S					SBOX_INFO"Library %s loaded in array\n"
#define LIB_NOT_FOUND				SBOX_INFO"Library NOT found\n"
#define MAX_PATHS_REACHED			SBOX_INFO"Maximum no. of libraries paths. Cannot use any more paths \n."
#define MAX_LIBS_REACHED			SBOX_INFO"Maximum no. of libraries. Cannot load any more libraries \n."
#define MSG_MAX_LIBS_D 				SBOX_ERR"Max amount of libraries loaded (%d)\n"
#define NO_OPTS						SBOX_ERR"Missing Options. Please enter one or more options\n"
#define PATH_IS_S					SBOX_INFO"Path entered is %s\n"
#define TITLE_VERSION 				"SandBox. Telecom SudParis, MSc CCN 2016. Version 1.4 \n"
#define VERBOSE						SBOX_INFO"Verbose Option Enabled\n"
#define WRONG_LIB_NAME_S			SBOX_ERR"Wrong library name  '%s', please follow the naming convention\n"


// dynlib.c
#define CONTINUE_ENTER 						"Press [ENTER] to continue..."
#define CUSTOM_SYSCALLS_LOADED_D 	 		SBOX_INFO"Custom Syscalls loaded = %d\n"
#define CUSTOM_SYSCALL_S					SBOX_INFO"reading custom syscall %s\n"
#define CUSTOM_LIBRARY_S_D					SBOX_INFO"reading custom library %s with %d array\n"
#define CUSTOM_LIBRARY_INIT_S				SBOX_INFO"executed initialize() of custom library %s\n"
#define CUSTOM_LIBRARY_END_S				SBOX_INFO"executed terminate() of custom library %s\n"
#define CUSTOM_SYSCALL_S_POINTERS_LD		"SBOX-TABLE: custom syscall %s, before %ld after %ld\n"
#define CUSTOM_LIBRARY_S_D_POINTER_LD		"SBOX-TABLE: custom library %s with %d entries in array %ld\n"
#define ERROR_DLOPEN_S						SBOX_ERR" at opening library: %s\n"
#define	ERROR_LOADING_CUSTOM_LIBRARY_S		SBOX_ERR" loading custom library (%s), descriptor is not valid \n"
#define ERROR_LOADING_CUSTOM_SYMBOL_S		"ERROR loading custom syscall, required Symbol %s not found\n"
#define ERROR_LOADING_CUSTOM_SYSCALL_COPY	SBOX_ERR" loading custom syscall, unable to copy the descriptor \n"
#define ERROR_LOADING_CUSTOM_SYSCALL_D		SBOX_ERR" loading custom syscall, error while reading the Custom Syscall Descriptor %d \n"
#define ERROR_LOADING_CUSTOM_SYSCALL_INVALID	SBOX_ERR" loading custom syscall, syscall number is not valid of function is null \n"
#define ERROR_LOADING_CUSTOM_SYSCALL_MALLOC		SBOX_ERR" loading custom syscall, unable to allocate memory \n"
#define	ERROR_LOADING_CUSTOM_SYSCALL_NULL		SBOX_ERR" loading custom syscall, descriptor is not valid \n"
#define ERROR_LOADING_CUSTOM_SYSCALL_OVERRIDE 	SBOX_ERR" loading custom syscall, there is already a custom syscall \n"
#define LIBRARIES_LOADED_D  				SBOX_INFO"Libraries loaded = %d\n"
#define LOADED_CUSTOM_SYSCALL_S				SBOX_INFO"loaded custom syscall %s \n"

//trace.c
#define CUSTOM_SYSCALL_CATCHED_D		"SystemCall (%d)\n"
#define CUSTOM_SYSCALL_S_RET_D			SBOX_INFO"Custom SystemCall (%s) returns %d \n"
#define CUSTOM_SYSCALL_CATCHED_S_FROM_S	SBOX_INFO"Custom SystemCall (%s) from Library (%s) "
#define CUSTOM_SYSCALL_CALLED_AFTER		SBOX_INFO"Custom SystemCall called AFTER kernel \n"
#define CUSTOM_SYSCALL_CALLED_BEFORE	", called BEFORE kernel \n"
#define CUSTOM_SYSCALL_NOKERNEL			", not calling Kernel "
#define	CUSTOM_SYSCALL_CALLING_BEFORE	", calling BEFORE kernel "
#define	CUSTOM_SYSCALL_CALLING_AFTER	" Calling AFTER kernel "
#define CUSTOM_SYSCALL_QUIT_ON_ERROR	", QUIT on negative return "
#define CUSTOM_SYSCALL_KEEP_RESULT		", KEEP previous return value"
#define CUSTOM_LIB_CALLED_S_S			" Custom SystemCall (%s) from Library (%s) "
#define KERNEL_SYSCALL					" KERNEL executing normal Syscall\n"
#define NO_KERNEL_SYSCALL				" Skipping KERNEL normal Syscall\n"
#define	LF_CR							"\n"
#define	LINE							"-----------------------------------------------------\n"
#define SPACE							"+"
#define	STARTING_TRACE_D				SBOX_INFO"Starting tracing main pid %d \n"
#define	TRACEE_EXIT						SBOX_INFO"Tracee process performed exit() \n"
#define	TRACEE_ERROR_D					SBOX_ERR"at tracing pid %d \n"
#define TRACEE_EXIT_BY_SIGNAL_D			SBOX_INFO"Tracee process exit by SIGNAL %d \n"
#define TRACEE_STOPPED_BY_SIGNAL_D		SBOX_INFO"Tracee process stopped by SIGNAL %d \n"
#define TRACEE_SYSCALL_D 				SBOX_INFO"SystemCall %d catched\n"
#define TRACEE_CHILD_END_D				SBOX_INFO"Tracee child terminated with return value %d\n"
#define TRACEE_CHILD_START_D			SBOX_INFO"Tracing child process %d \n"
#define TRACEE_FORKED 					SBOX_INFO"Tracee has forked as process\n"
#define TRACKING_CLONED_D				SBOX_INFO"Monitoring cloned process %d\n"
#define TRACKING_FORKED_D				SBOX_INFO"Monitoring forked process %d\n"
#define EXIT_CAPTURED_D					SBOX_INFO"Received exit for PID %d\n"
#define EXIT_EVENT_D					SBOX_INFO"Received exit event for PID %d\n"
#define STOP_CAPTURED_H_D 				SBOX_INFO"Received stop signal %d for PID %d \n"
#define CORE_DUMP						SBOX_INFO"Core Dumped \n"
#define CONTINUED						SBOX_INFO"Continued	 \n"



#define FALSE 0
#define TRUE 1

/** Use as return value in functions */
#define RETURN_OK 	0
#define RETURN_ERR 	-1

extern char explicitOutputFlag;  //!< Determines if the Verbose option was requested, to print all usefull information during execution
extern char execTreeOutputFlag;  //!< Determines if the Execution plan is printed
extern char childProcessFlag;	 //!< Determines if the \b tracee child processes are monitored
