# @file makefile
## @brief Make file used to compile and create files for the SandBox Project

#All executable source files are to be left in the src/
#All library files are taken from src/libs/
#All test files are taken from src/tests/

#All Object files are left in bin/obj/

# Authors: Ignacio TAMAYO and Vassanthaphriya VIJAYAN
# Date: Jul 2016
# Version: 1.4

#Compiler arguments
GCC_INCLUDE_H = -I./src
#>>>>>>>    -I. = ./src has the .h files

GCC_COMPILE_WARNINGS = -Wall   -Wshadow
### -Wstrict-prototypes -Wconversion -Wmissing-prototypes
GCC_COMPILE_OPTIONS = -fPIC -nostdlib $(GCC_COMPILE_WARNINGS)
# Options used to compile .c files into .o files
#  -fPIC
#If supported fo r the target machine, emit position-independent code, suitable for dynamic linking and avoiding any limit on the size of the global offset
#table.  This option makes a difference on the m68k, PowerPC and SPARC.

GCC_LINK_OPTIONS =
# Options used to compile .o files into executable files
GCC_LIB_OPTIONS = -shared  -nostdlib
# Options used to compile .o files into .so libraries

#Creates an output binary for each test program
TESTS_FILES := $(wildcard src/tests/*.c)
TESTS_EXEC_FILES := $(addprefix bin/tests/,$(notdir $(TESTS_FILES:.c=)))

#Creates an output shared library for each coded library
LIBS_FILES := $(wildcard src/libs/*.c)
LIBS_SO_FILES := $(addprefix bin/libs/,$(notdir $(LIBS_FILES:.c=.so)))

PWD = $(shell pwd)

#Recepies declarations
.PHONY: clean cleanall  cleandocs  cleanlibs cleantests
.PHONY: libraries docs tests sandbox

.DEFAULT: help

help:
	@echo "make clean | cleandocs | cleantests | cleanlibs | cleanall"
	@echo make mkdirs
	@echo "make sandbox | libraries | tests | all"
	@echo make docs

all: mkdirs cleanall sandbox libraries tests

#Building the sandbox
sandbox: bin/obj/sandbox.o  bin/obj/opts.o bin/obj/trace.o  bin/obj/dynlib.o bin/obj/global.o    bin/obj/list.o
	gcc $(GCC_LINK_OPTIONS)  -o bin/$@ $?  -ldl
	rm $?

#Building the libraries
libraries:   libtcp.so libio.so  libtime.so $(LIBS_SO_FILES)

#These libraries make use of the auxiliary functions to read/write to memory of the tracee
libio.so:   bin/obj/libSandboxHelper.o  bin/obj/libio.o
	gcc $(GCC_LIB_OPTIONS) -o bin/libs/$@ $?
libtime.so:   bin/obj/libSandboxHelper.o  bin/obj/libtime.o
	gcc $(GCC_LIB_OPTIONS) -o bin/libs/$@ $?
libtcp.so:   bin/obj/libSandboxHelper.o  bin/obj/libtcp.o
	gcc $(GCC_LIB_OPTIONS) -o bin/libs/$@ $?

#Building the tests
tests: $(TESTS_EXEC_FILES)

#These tests need an aditional library for Threads
bin/tests/ECHOserverThreaded:  bin/obj/ECHOserverThreaded.o
	gcc $(GCC_LINK_OPTIONS) -o  $@ $< -lpthread
	rm $?

bin/tests/testThread:  bin/obj/testThread.o
	gcc $(GCC_LINK_OPTIONS) -o  $@ $< -lpthread
	rm $?

#Automatic rule for the tests
bin/tests/%: bin/obj/%.o
	gcc $(GCC_LINK_OPTIONS) -o  $@ $<
	rm $?

#All ./src/*.c files are built into ./obj/*.o objects
bin/obj/%.o: src/%.c
	gcc -c  $(GCC_COMPILE_OPTIONS) -o  $@ $<
bin/obj/%.o: src/libs/%.c
	gcc -c  $(GCC_COMPILE_OPTIONS)   $(GCC_INCLUDE_H) -o  $@ $<
bin/obj/%.o: src/tests/%.c
	gcc -c  $(GCC_COMPILE_OPTIONS)   $(GCC_INCLUDE_H) -o  $@ $<

#All ./libs/*.so files are built
bin/libs/%.so: bin/obj/%.o
	gcc $(GCC_LIB_OPTIONS) -o $@ $<
	rm $?

mkdirs:
	mkdir -p  bin/obj bin/libs bin/tests
	chmod u+x bin bin/tests

cleanall:
	rm -f bin/obj/* bin/libs/*  *.txt sandbox bin/tests/*
cleantests:
	rm -f bin/obj/*   *.txt  bin/tests/*
cleanlibs:
	rm -f bin/libs/*    bin/obj/*
clean:
	rm -f bin/obj/* sandbox libhelper.so
cleandocs:
	rm -Rf docs/html
docs: cleandocs
	doxygen docs/doxygen.config
