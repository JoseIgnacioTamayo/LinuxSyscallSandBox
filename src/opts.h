/*! \file opts.h
    \brief Functions dedicated to processing the command lines arguments

	Determined here the command line to call sandbox
	*
	* Several libraries can be registed with the sandbox for the same syscalls.
	* They will be called in the order they appeared in the command line


	\see opts.c sandbox.c

	\authors Ignacio TAMAYO and Vassanthaphrya VIJAYAN
	\date 06.05.2016
	\version 1.4
	\note
*/

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



#define OPTIONS_OK_CONTINUE 1
#define OPTIONS_OK 0
#define OPTIONS_ERROR_OPTS	29
#define OPTIONS_ERROR_LIBS	19
#define OPTIONS_ERROR_PATH	9

/*! Process the command line arguments when calling Sandbox.
 * \param argc number of arguments
 * \param argv array of arguments
 * \return OPTIONS_OK_CONTINUE if Sandbox can continue to trace for syscalls, OPTIONS_OK if the command was for Help, OPTIONS_ERROR_* if other errors
*/

int process_options(int argc, char* argv[]);

/** Checks if the library name is on convention  ^lib[A-Z|a-b|0-9]*.so$ .
 * (Eg: ./libIo1.so  is invalid, Io1  is valid).
 * \param lname library name to analyze
 * \return RETURN_OK if library name is valid, <> RETURN_OK if not.
 */
int check_lib_name(char *lname);

/** Printing possible options for sandbox in stdout
 * This function writes the full -h option output
 */
void print_options_msg();

/** Checks if the path is valid.
 * This removes / at the end of the path string.
 * \param path to check
 * \return RETURN_OK if path is valid, <> RETURN_OK if not.
 */
int check_path(char *path);
