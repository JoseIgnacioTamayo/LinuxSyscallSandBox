/*! \file list.c
    \brief Chained list used to contain several structures in the Sandbox
  	\authors Ignacio TAMAYO
	\date Jul 22nd 2016
	\version 1.0

    This List is a dual-chained list, with a void* pointer as data
    This is an unordered list, all the additions are appended at the end of the list
    This list allows for iteration by using seek(<position>), has_next() and get_next();
    This list should be wrapped by specific type functions no make it easier



*/

#include <stdlib.h>
#include "list.h"

int is_empty(list* l)
{
	return (l->head==NULL);
}

list* new_list()
{
	list* L;
	L = (list *)malloc(sizeof(list));
	L->head=L->tail=L->cursor=NULL;
	L->counter=0;
	return L;
}

int append_item(list* l, void * item)
{
	struct node * Nnode;
	if (item==NULL) return -1;
	Nnode = (struct node *)malloc(sizeof(struct node));
	Nnode->data = item;
	Nnode->next = NULL;
	Nnode->prev = NULL;
	if (l->head == NULL) //First element
	{
		l->head=Nnode;
		l->tail=Nnode;
		l->cursor=Nnode;
	}
	else //Second or n element
	{
		l->tail->next = Nnode;
		Nnode->prev = l->tail;
		l->tail=Nnode;
	}
	l->counter++;
	return 0;
}

int seek(list* l, int pos)
{
	if (pos > l->counter) //Trying to seek out of list boundaries
		return -1;
	l->cursor = l->head;
	while( pos-- && l->cursor->next) l->cursor = l->cursor->next;
	return 0;
}

void goto_first(list* l)
{
	l->cursor = l->head;

}

void goto_last(list* l)
{
	l->cursor = l->tail;
}

int has_next(list* l)
{
	return (l->cursor != NULL );
}

void* get_previous(list* l)
{

	void* data = l->cursor->data;
	l->cursor = l->cursor->prev;
	return data;
}

void* get_next(list* l)
{

	void* data = l->cursor->data;
	l->cursor = l->cursor->next;
	return data;
}

int delete_item(list* l, void *item)
{
	struct node * Nnode;

	if ( is_empty(l) ) return -1;

	Nnode = l->head;

	while(Nnode)
	{
		if (Nnode->data == item)
			break;
		else
			Nnode=Nnode->next;
	}
	if (Nnode == NULL)
		return -1;		//Not found

	if (Nnode == l->head)
	{

		l->head = l->head->next;
		if (l->head != NULL)
			l->head->prev = NULL;
		else
		{
			l->tail = NULL;
			l->cursor = NULL;
		}
	}
	else if (Nnode == l->tail)
	{

		l->tail = l->tail->prev;
		if (l->tail != NULL)
			l->tail->next = NULL;
		else
		{
			l->head = NULL;
			l->cursor = NULL;
		}
	}
	else
	{
		(Nnode->prev)->next = Nnode->next;
		(Nnode->next)->prev = Nnode->prev;
	}

	free(Nnode);
	l->counter--;
	return 0;
}
