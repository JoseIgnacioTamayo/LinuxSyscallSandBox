/*! \file testLibTime.c
    \brief Test program for libtime.so
  	\authors Ignacio Tamayo
	\date May 25st 2016
	\version 1.3 
	
	
	
    \code
	./sandbox -l time testLibTime
    \endcode
    
 	\see trace.h libpid.c
	
*/

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define DAY 	(60*60*24)
#define MONTH	(DAY*30)
#define YEAR	(MONTH*12)

/** Prints the current date based in calculations
 * */

int main()
{	
	time_t tm;
	struct timespec ts;
	struct timeval tv;
	int i;
	
	tm = time(NULL);
	// time(&tm);
	
	printf("Current date is %s Waiting for 5 secs\n",ctime(&tm));
	ts.tv_sec = 5;
	ts.tv_nsec = 500;
	nanosleep(&ts,NULL);
	if ( gettimeofday(&tv, NULL) ==0 )			//TZ is NULL as recommended
	{
		printf("Current date is %s",ctime(&tv.tv_sec));
		printf("Setting new time in the system...");
		if (settimeofday(&tv, NULL) == 0) printf("OK\n");
		else  printf("NOPE\n");
		nanosleep(&ts,NULL);
		printf("Current date is %s",ctime(&tv.tv_sec));
	}
	return 200;
}
