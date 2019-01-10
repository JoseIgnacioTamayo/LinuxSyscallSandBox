/*! \file simpleIOInteractive.c
    \brief Test program that expects some replies from STDIN and also prints to STDIN

	Uses PRINTF and SCANF

    \code
	./sandbox simpleIOInteractive
    \endcode

 	\see trace.h

	\authors Ignacio Tamayo
	\date April 4st 2016
	\version 1.0

*/

#include <stdio.h>

int main()
{
	int value;
	do {
	printf("Enter 0 to exit, enter only numbers: \n");
	scanf("%d",&value);
	} while (value !=0);
	return 0;
}
