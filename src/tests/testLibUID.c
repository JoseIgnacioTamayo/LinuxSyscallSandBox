/*! \file testLibUID.c
    \brief Test program for libuid.so
  	\authors Ignacio Tamayo and Vassanthaphriya VIJAYAN
	\date April 28th 2016
	\version 1.2  
	
	
	
    \code
	./sandbox -llibuid.so testLibUID
    \endcode
    
 	\see trace.h libuid.c
	
*/

#include <stdio.h>
#include <fcntl.h>

/** Prints to STDOUT the compiled architecture 
 * Shows the fake real UID and effective UID of the calling process as per the library
 * 
 * */
 
int main()
{
		
	int fd;
	char buff;
	
	//Test to see with which of the defines is this system compilated		
	#ifdef __x86_64__	
	printf("DEBUG 654:  x64 compilation\n");
	#endif
	#ifdef	__i386__
	printf("DEBUG 654:  i386 compilation\n");
	#endif

	printf("My real UID is %d \n",getuid());
	printf("My effective UID is %d \n",geteuid());
	
}
