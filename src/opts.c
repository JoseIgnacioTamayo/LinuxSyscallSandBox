/*! \file opts.c
    \brief Functions dedicated to processing the command lines arguments
	\authors Ignacio TAMAYO and Vassanthaphrya VIJAYAN
	\date 06.05.2016
	\version 1.4
	\see dynlib.h opts.h

	Functions dedicated to processing the command lines arguments

 * \internal
 * All Error messages are controlled by the messages.h header
 * All DEBUG messages are typed verbatim in code, the FLAG in the messages.h header controls if they are shown or not
 * The -h output is defined here
 * The -v output is defined here
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
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include "messages.h"
#include "dynlib.h"
#include "opts.h"


void print_options_msg()
{
		printf ("--------------------------------------------------------------------------------------------\n");
		printf (" sandbox [-v] [-p] [ -L <path> ] [ -L<Path> ... ] [ -l <library> ] [ -l <library> ... ] <tracee>\n");
		printf (" \t -v\t\tVerbose mode, many messages are printed in STDOUT to track the steps of Sandbox\n");
		printf (" \t -p\t\tTrace also the child processes of the tracee, created by fork()\n");
		printf (" \t -l\t\tName of the library, in the gcc format. If library is libXYZ.so, put -l XYZ\n");
		printf (" \t -L\t\tPath to look for the custom libraries libXYZ.so\n");
		printf (" \t <tracee>\tExecutable to be traced by Sandbox. Must not have redirection mechanisms ( |, <, >, >>) \n");
		printf (" \n");

		printf (" sandbox -t  -L <path>  [ -L<Path> ... ]  -l <library> [ -l <library> ... ]\n");
		printf (" \t -t\t\tShows the execution tree for the given custom libraries\n");
		printf (" \t -l\t\tName of the library, in the gcc format. If library is libXYZ.so, put -l XYZ\n");
		printf (" \t -L\t\tPath to look for the custom libraries libXYZ.so\n");
		printf (" \n");

		printf (" sandbox -h\t\t Help\n");
        printf("\n");

}


int check_lib_name(char *lname)
{
	int len, i;
	if (lname==NULL)
		return 99;	//Null String error
	len=strlen(lname);
	if(len==0)
		return 39; // Checks if string is not empty to proceed further
	for(i=0;i<len;i++)
		{
			if ( !  ( isalpha(lname[i]) || isdigit(lname[i]) ) )
				return 19;		//Not alpha character or number
			if ( (lname[i]=='/') || (lname[i]=='.')  )
				return 29;		//Check there is no / or . in lname
		}
	return RETURN_OK;
}


int check_path(char *path)
{
	int len;
	DIR *fd;
	if (path==NULL)
		return 99;	//Null String error
	len=strlen(path);
	if(len==0)
		return 39; // Checks if string is not empty to proceed further
	if (path[len-1]=='/')path[len-1] = '\0'; //Last char not /
	fd = opendir(path);
	if (fd == NULL)
		return 49; //Not a valid directory
	closedir(fd);
	return RETURN_OK;
}



#define BUFFER_SIZE 256		/** Buffer length used to build full paths to files*/

int process_options(int argc, char* argv[])
{
	int c;
	list* paths_list ;
	int foundLib;
	char full_lib_path[BUFFER_SIZE];
	char* a_path;

	paths_list = new_list();

	//check no options, show -u
	if (argc == 1)
	{
		printf (NO_OPTS);
		print_options_msg();
		return OPTIONS_ERROR_OPTS;
	}

	//lib_counter = 0;
	while ((c = getopt (argc, argv, "+hvtpl:L:")) != -1)
		// Valid options is -l -v -h -L
		// + is used to tell the getopt that as soon as a non-arg is found,
		//it goes out. This is because after the options, whatever comes after
		//is the options of the command to run
	{
		switch (c)
		{
				case 'L':

					if (check_path(optarg) != RETURN_OK)		//Path is ok?
					{
						eprintf(INVALID_PATH_S,optarg);
						return OPTIONS_ERROR_OPTS;
					}
					else // Path ok,
					{
						vprintf(PATH_IS_S, optarg);
						append_item(paths_list,optarg);		 //Place the paths in an array of string.
					}

				break;

				case 'l':


					vprintf(LIB_IS_S, optarg);
					if (check_lib_name(optarg) != RETURN_OK)		//Name is ok?
					{
						eprintf(WRONG_LIB_NAME_S,optarg);
						return OPTIONS_ERROR_LIBS;
					}
					else // Name ok, trying to concatenate a valid file name
					{
						foundLib=0;
						seek(paths_list,0);
						while(has_next(paths_list))

						{

							a_path = get_next(paths_list);
							memset(full_lib_path,'\0',BUFFER_SIZE); //Clearing buffer
							strcat(full_lib_path, a_path);		//Copy first path
							strcat(full_lib_path, "/lib");		//Add /lib
							strcat(full_lib_path, optarg);		//Add the lib name
							strcat(full_lib_path, ".so");		//Add .so

					  		vprintf(LIB_CHECK_IS_S, full_lib_path);

					  		if(add_custom_library(full_lib_path)==RETURN_OK)
					     		{
									foundLib = 1;
									break;  //Stop looking in the paths
					     		}

						}//end of FOR loop on the paths
				 		if (!foundLib)
				 		{
							eprintf(LIB_NOT_FOUND);
							return OPTIONS_ERROR_LIBS;
						}
						else
							vprintf(LIB_FOUND_S, full_lib_path);
					}

			break;
			case '?':								//In case there is nothing after -l
				if (optopt == 'l')
					eprintf (ERROR_OPT_L_MISSING_ARG);
				else if (optopt == 'L')
					eprintf (ERROR_OPT_LL_MISSING_ARG);
				else
					eprintf (ERROR_UNKNOWN_OPT_C, optopt);
				return OPTIONS_ERROR_OPTS;
			break;
			case 'h':								//Help
				print_options_msg();
				return OPTIONS_OK;
			break;
			case 'v':
				dprintf(VERBOSE);
				explicitOutputFlag=TRUE;
				break;
			case 'p':
				childProcessFlag=TRUE;
				break;
			case 't':
				execTreeOutputFlag=TRUE;
				break;
			default:
				return FALSE;
			break;
		} //End of Switch
	} //End of While
	if (argc == optind)
	{
		eprintf (ERROR_OPT_MISSING_CMD);
		return OPTIONS_ERROR_OPTS;
	}
	else
      return OPTIONS_OK_CONTINUE;
} //End of function
