/*! \file testFork.c
    \brief Test program for test fork support
  	\authors Ignacio Tamayo
	\date June 3rd 2016
	\version 1.0

	By using STRACE we see that that getpid() is called only once
	*
	* strace -f -c tests/testFork
	*
	% time     seconds  usecs/call     calls    errors syscall
	  0.00    0.000000           0         1           getpid
	  0.00    0.000000           0         4           getppid



    \code
	./sandbox -l chatty testFork
    \endcode

 	\see trace.h

*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>		// For waitforpid())
#include <stdio.h>
#include <fcntl.h>

/** Prints to STDOUT the PID assigned by the kernel, to Child and 2 Children
 * */
int main()
{
	pid_t pid;	// The Child PID

	switch (pid=fork())
	{
	case 0:  //Child
		printf("Luke is %d \n",getpid());
		//sleep(5);
		printf("Lukes father is %d \n",getppid());
		printf("Luke is %d \n",getpid());
		printf("Lukes father is %d \n",getppid());
		return 10;
		break;
	case -1:
		perror("Fork: ");
		return 9;
		break;
	default:
		printf("DarthVader is %d \n",getpid());
		//sleep(2);
		printf("DarthVadir father is %d \n",getppid());
		printf("DarthVader is %d \n",getpid());
		printf("DarthVadir father is %d \n",getppid());
		waitpid (pid, 0, 0 ); 		//WCONTINUED | WEXITED | WSTOPPED does not work!!!!
		return 20;

		break;
	}//End of Switch
	return 0;
}
