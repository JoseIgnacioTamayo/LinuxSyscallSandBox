/*! \file testLibIO.c
    \brief Test program that uses the File IO syscalls in libio.so
 	\authors Ignacio Tamayo
	\date April 24st 2016
	\version 1.2

    Uses both FILE* functions and fd functions
    A local temporary file is opened, a line is appended, and finally re-read

	\see trace.h libio.c

*/

#include <stdio.h>
#include <fcntl.h>

int main(int argc, char * argv[])
{
		FILE *f;
		int fd;
		char buff[64];

		//Getting the file to work on
		if(argc != 2)
		{
			printf("Wrong command\n");
			printf("testLibIO <file to open>\n");
			return 9;
		}

		f = fopen(argv[1],"a");
		if (f != NULL)
		{
				fputs("Test String in file\n",f);
				fclose(f);
				fd = open(argv[1],O_RDONLY);
				if (fd>0)
				{
					while ( read(fd,buff,sizeof(buff)) >0 );
					close(fd);
				}
				else
					perror("");

		}
}
