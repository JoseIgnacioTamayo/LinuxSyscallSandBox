/*! \file testList.c
    \brief Unit testing for the list.c file
  	\authors Ignacio TAMAYO
	\date Jul 22nd 2016
	\version 1.0
	
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
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "trace.h"

int main()
{
	list* L = new_list();
	
	tracee_flow_descriptor * tracee_desc1,*tracee_desc2,*tracee_desc3;
	tracee_flow_descriptor * t;
	
	tracee_desc1 = (tracee_flow_descriptor*)malloc( sizeof(tracee_flow_descriptor));
	tracee_desc2 = (tracee_flow_descriptor*)malloc( sizeof(tracee_flow_descriptor));
	tracee_desc3 = (tracee_flow_descriptor*)malloc( sizeof(tracee_flow_descriptor));
	
	tracee_desc1->pid = 1;
	tracee_desc2->pid = 2;
	tracee_desc3->pid = 3;
	
	printf("Testing of List\n");
	
	if (is_empty(L)) printf("List is empty\n");
	
	append_item(L, tracee_desc1);
	
	printf("List has %d elements \n", L->counter);
	
	seek(L,0);
	while(has_next(L))
	{
		t = get_next(L);
		if (t->pid == 1)
			delete_item(L,t);
	}
	
	printf("List has %d elements \n", L->counter);
	
	seek(L,0);
	while(has_next(L))
	{
		t = get_next(L);
		if (t->pid == 1)
			delete_item(L,t);
	}
	
	append_item(L, tracee_desc2);
	append_item(L, tracee_desc1);
	append_item(L, tracee_desc3);
	append_item(L, tracee_desc2);
	append_item(L, tracee_desc3);
	
	printf("Appended to List\n");
	
	printf("List has %d elements \n", L->counter);
	
	seek(L,0);
	while(has_next(L))
	{
		t = get_next(L);
		if (t->pid == 2)
			delete_item(L,t);
	}
	seek(L,0);
	while(has_next(L))
	{
		t = get_next(L);
		printf("PID in list %d \n",t->pid);
	}
	
	append_item(L, tracee_desc1);
	append_item(L, tracee_desc2);
	append_item(L, tracee_desc1);
	
	seek(L,0);
	while(has_next(L))
	{
		t = get_next(L);
		printf("PID in list %d \n",t->pid);
	}
	
}
