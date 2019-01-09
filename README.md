Linux Syscall SandBox
=======

> Sandbox - Télécom Sudparis - MSc CCN 2016
>
> Ignacio TAMAYO and Vassanthaphriya VIJAYAN
>
> August 2016
>
> Version 1.4

Check **docs** folder for instructions or run `sandbox -h`

This program was developed as a course project for class CSC7201, under supervision of Professor Eric Renault (eric.renault@telecom-sudparis.eu).
Many thanks for this guidance and assistance throughout the develpment.

This program is based on *ptrace* and *ld* C libraries.

The Sandbox tool will monitor a **tracee** executable, capturing its syscalls calls, and executing custom routines instead, which are found in dynamic libraries.

The Sandbox works entirely on user-space, which has the downside of slow speed, but is very flexible for running custom code instead of the syscalls without touching the kernel.

When calling Sandbox, several custom dynamic libraries can be loaded, containing the methods to call before/instead/after of the kernel syscall and additional options. Refer to **customSyscalls.md** for details about the posibilities.

While executing the **tracee**, if the syscall captured is implemented in any of the custom libraries loaded by Sandbox, that function is executed with the options/conditions described in the library. Otherwise, if the syscall captured is not implemented in any of the custom libraries, the normal kernel syscall function is called and executed. Several custom functions can be executed for the same syscall, giving space for extensibility.

When the **tracee** terminates (or is terminated by signals), the Sandbox also terminates.

In *VERBOSE* mode, several lines of output will help visualize the tracing of the syscalls and its execution.

The Sandbox keeps track of all the processes that are forked or clone by the **tracee**, allowing to capture the syscalls of the child processed.

> Works on i386 and x86_64 architectures

Getting started
--------------

 1. Clone the repository
 1. Review the **docs/** folder
 1. Run `make all`
 1. Run `runExample.sh`

Project Folder Structure
--------------

 * **src/** contains the source code of the sandbox
  * **src/libs/** has the source of some dynamic libraries that implement some syscalls for demo
  * **src/tests/** has the source of executable demo programs that use some syscalls, to be catched by the SandBox

 * **bin/** has the compiled executable binaries
  * **bin/obj/**  is the temporary object files are placed. This folder is used only during compilation process_options.
  * **bin/libs/** contains the available test libraries, .so files for dynamical loading.
  * **bin/tests/** has some demo programs that use syscalls to be interrupted by the SandBox

 * **docs/** folder is where the documentation is placed.

 * **tests/** folder is where the tests scripts are placed.


MIT Licence
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
