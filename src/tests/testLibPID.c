/*! \file testLibPID.c
    \brief Test program for libpid.so
  	\authors Ignacio Tamayo
	\date April 25st 2016
	\version 1.2



    \code
	./sandbox -llibpid.so testLibPID
    \endcode

 	\see trace.h libpid.c

*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

/** Prints to STDOUT the compiled architecture
 * Shows the fake PID and PPID as per the library
 * Tries to kill pid 10, actually does nothing but thinks that the kill was ok
 *
 * */

int main()
{
	printf("My PID is %d \n",getpid());
	printf("My PPID is %d \n",getppid());
	printf("Killing PID 10, result: ");
	if (!kill(10,9)) printf("OK \n");
	else  printf("NO \n");
	return 0;
}
