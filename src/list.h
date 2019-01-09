/*! \file list.h
    \brief Chained list used to contain several structures in the Sandbox
  	\authors Ignacio TAMAYO
	\date Jul 22nd 2016
	\version 1.0
	  
    This List is a dual-chained list, with a void* pointer as data
    
    This is an unordered list, all the additions are appended at the end of the list
    
    This list allows for iteration by using seek(<position>), has_next() and get_next();
    
    This list should be wrapped by specific type functions no make it easier
    
    The list has an internal CURSOR pointer used to iterate the list. Operated by seek() and get_next()
    
    
    \code   
	list L;
	int data = 10;
	append_item(&L,&data);
	append_item(&L,&data);
	L.counter;
	is_empty(&L);
	seek(&L,0);// Go to first element
	while (has_next(&L)){
		data = *get_next(&L);
	}
	delete_item(&L,&data);	//Delete the node that has the same DATA pointer as data (pointer, not value)
	\endcode
	
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
 * 
 * */
 
 
#ifndef INC_LIST	//Lock to prevent recursive inclusions
#define INC_LIST

/*! \brief Node of the list. */
struct node
{
	void* data;  //!< Pointer to the data contained in the node. When detele_item, this pointer address is considered
	struct node* next ;	//!< Pointer to the next node in the list
	struct node* prev ; //!< Pointer to the previous node in the list
};

/*! \brief A list .
*/
typedef struct { 
	int counter ;  //!< Counts the number of elements 
	struct node* head  ;  //!< Fist of the list
	struct node* tail  ;  //!< Last of the list 
	struct node* cursor  ;  //!< Internal cursor used for iterations
	} 
list;

/**
  \return An Empty list
*/
list* new_list();

/**
 \param l is the pointer to the list to check
  \return 1 (TRUE) if the list is empty or 0 (FALSE) if not
*/
int is_empty(list* l);


/** Adding a not-NULL memory location item to the list.
 * 
 * The content of the item pointer is not copied into the list, only referenced. 
 * If the data content is freed, the node will still point to the same memory address
 * 
 \param l is the pointer to the list to operate
 \param item is the pointer to the data to be contained in the node.
  \return 0 (OK) if addition was OK, -1 (ERR) if not
  
*/
int add_item(list* l, void *item);


/** Moved the internal cursor to the X position from the head of the list
 * 
 * The position annot be bigger than the amount of items in the list
 * 
 \param l is the pointer to the list to operate
 \param pos is the position from the head of the list. Between 0 and (l.counter-1)
  \return 0 if seek was OK, -1 (ERR) if not
  
*/
int seek(list* l, int pos);


/** Starting from the current CURSOR position, check if there is a next element
 * 
 \param l is the pointer to the list to operate
  \return 0 if (true), 0 if not (FALSE)
  
*/

int has_next(list* l);

/** Starting from the current CURSOR position, get the current data item and move the cursor one step back
 * 
 \param l is the pointer to the list to operate
  \return the memory pointer to the DATA element of the current node
  
*/
void* get_previous(list* l);


/** Starting from the current CURSOR position, get the current data item and move the cursor one step
 * 
 \param l is the pointer to the list to operate
  \return the memory pointer to the DATA element of the current node
  
*/
void* get_next(list* l);


/** Deleting a not-NULL memory location item to the list.
 * 
 * The exact memory location is searched for deletion. The comparison is made on memory locations, not on values 
 * This deleted the node, does not delete the item in memory
 * 
 \param l is the pointer to the list to operate
 \param item is the pointer to the data to be searched in the node.
  \return 0 (OK) if deletion was OK, -1 (ERR) if not
  
*/
int delete_item(list* l, void *item);

/** Moves the internal cursor to the fist position of the list
*/
void goto_first(list* l);

/** Moves the internal cursor to the last position of the list
*/
void goto_last(list* l);


#endif
