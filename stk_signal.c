#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk_signal.h"
#include "stk_mm.h"

// function declaration
static int stk_signal_connectToObject(STK_Object *object, char *signal, F_Signal_Callback callback, void *userdata);
static int stk_signal_emitToObject(STK_Object *object, char *signal, void *signaldata);

// slist_head point to the head of the global signal list.
static SignalListNode *slist_head = NULL;

SignalListNode *stk_signal_getListHead()
{
	return slist_head;
}

// initial the head pointer
int stk_signal_init()
{
	slist_head = NULL;
	return 1;
}

// register a new signal into the signal list.
int stk_signal_new( char *signal )
{
	// if global signal list is blank
	if (slist_head == NULL) {
		slist_head = (SignalListNode *)stk_malloc(sizeof(SignalListNode));
		slist_head->signal = signal;
		slist_head->type = 0;
		slist_head->call_list = NULL;
		slist_head->next = NULL;
	}
	// if global signal list is not blank
	else {
		SignalListNode *l = stk_signal_getListHead();
		// walk along the signal list, until the end of it
		while (l->next) {
			// check specified 
			if (!strcmp(l->signal, signal)) {
				fprintf(stderr, "Signal [%s] already exists.\n", signal);
				return 0;
			}
			l = l->next;
		}
		
		// add new signal node to the end of the global signal list
		l->next = (SignalListNode *)stk_malloc(sizeof(SignalListNode));
		l->next->signal = signal;
		l->next->type = 0;
		l->next->call_list = NULL;
		l->next->next = NULL;
	}
	
	return 1;
}

// Connecting function: connect the 'widget' with 'callback' function on 'signal' 
int stk_signal_connect(STK_Widget *widget, char *signal, F_Signal_Callback callback, void *userdata)
{
	// force converting STK_Widget type to STK_Object type 
	return stk_signal_connectToObject((STK_Object *)widget, signal, callback, userdata);
	return 1;
}

// Emit signal function: emit the 'signal' to 'widget' to trigger the corresponding callback function
int stk_signal_emit(STK_Widget *widget, char *signal, void *signaldata)
{
	// force converting STK_Widget type to STK_Object type 
	stk_signal_emitToObject((STK_Object *)widget, signal, signaldata);
	return 1;
}


static int stk_signal_connectToObject(STK_Object *object, char *signal, F_Signal_Callback callback, void *userdata)
{
	// if global signal list is blank
	if (slist_head == NULL) {
		fprintf(stderr, "Signal [%s] doesn't exist.\n", signal);
		return 0;	
	}
	// if global signal list is not blank
	else {
		SignalListNode *l = slist_head;
		// repeat to the end of the global signal list
		while (l) {
			// if equal, means find it, then break
			if (!strcmp(l->signal, signal))
				break;
			l = l->next;
		}
		
		if (l == NULL) {
			fprintf(stderr, "Signal [%s] doesn't exist.\n", signal);
			return 0;
		}
		
		// find the signal node in the list
		// if that node's call_list is blank, insert to it as first node
		if (l->call_list == NULL) {
			l->call_list = (CallbackListNode *)stk_malloc(sizeof(CallbackListNode));
			// object is actually an address
			l->call_list->object = object;
			// fill the callback function
			l->call_list->callback = callback;
			// fill up user defined parameters
			l->call_list->userdata = userdata;
			l->call_list->next = NULL;
			
			return 1;
		}
		else {
			CallbackListNode *cbl = l->call_list;
			// to the end of the callback list
			while (cbl->next)
				cbl = cbl->next;
			// add new node to the end of the callback list
			cbl->next = (CallbackListNode *)stk_malloc(sizeof(CallbackListNode));
			cbl->next->object = object;
			cbl->next->callback = callback;
			cbl->next->userdata = userdata;
			cbl->next->next = NULL;
			
			return 1;		
		}
	}
}

static int stk_signal_emitToObject(STK_Object *object, char *signal, void *signaldata)
{
	SignalListNode *l;
	CallbackListNode *cbl;
	
	l = slist_head;
	while (l) {
		// first, find the match signal node
		if (!strcmp(l->signal, signal)) {
			// check this node whether have callback list node
			if (l->call_list) {
				cbl = l->call_list;
				// walk along the callback list node
				while (cbl) {
					// check whether object in the list match with the specific object
					if (object == cbl->object) {
						// get the callback function registered before
						F_Signal_Callback callback = cbl->callback;
						// do call it: this is the real execute point
						// two parameters: signaldata, userdata. They can play a important part when program.
						callback( object, signaldata, cbl->userdata );
					}
				}
				cbl = cbl->next;
			}
			else {
				fprintf(stderr, "No handlers for signal %s.\n", signal);
			}
		}
		l = l->next;
	}

	return 1;
}


