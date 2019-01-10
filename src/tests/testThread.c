/*! \file testThread.c
    \brief Test program for test thread support
  	\authors Ignacio Tamayo
	\date June 3rd 2016
	\version 1.0

	By using STRACE we see that that getpid() is called only once


    \code
	./sandbox -l chatty testFork
    \endcode

 	\see trace.h

*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>	//for gettid()
#include <sys/syscall.h>  //for syscall()


void * thread_function(void * arg);

/** Prints to STDOUT the PID assigned by the kernel to 2 threads
 * */
int main(void)
{

	
	pthread_t th1, th2;	// The threads

	pthread_create(&th1, NULL, &thread_function, NULL);
    pthread_create(&th2, NULL, &thread_function, NULL);

	pthread_join( th1, NULL);
    pthread_join( th2, NULL);

	return 50;
}

void* thread_function(void * arg)
{
	printf("I am PID %d \n",getpid());
	printf("My father is  PID %d \n",getppid());
	printf("I am Thread POSIX %ld \n",pthread_self());
	printf("I am Thread %ld \n",syscall(__NR_gettid));
	return NULL;
}
