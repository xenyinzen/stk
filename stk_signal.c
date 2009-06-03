
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk_signal.h"



static int stk_signal_connectToObject(STK_Object *object, char *signal, F_Signal_Callback callback, void *userdata);
static int stk_signal_emitToObject(STK_Object *object, char *signal, void *signaldata);


static SignalListNode *slist_head;

int stk_signal_init()
{
	slist_head = NULL;
	return 1;
}

int stk_signal_new( char *signal )
{
	if (slist_head == NULL) {
		slist_head = (SignalListNode *)stk_malloc(sizeof(SignalListNode));
		slist_head->signal = signal;
		slist_head->type = 0;
		slist_head->call_list = NULL;
		slist_head->next = NULL;
	}
	else {
		SignalListNode *l = slist_head;
		while (l->next) {
			if (!strcmp(l->signal, signal)) {
				fprintf(stderr, "Signal [%s] already exists.\n", signal);
			}
			l = l->next;
		}
		
		l->next = (SignalListNode *)stk_malloc(sizeof(SignalListNode));
		l->next->signal = signal;
		l->next->type = 0;
		l->next->call_list = NULL;
		l->next->next = NULL;
	}
	
	return 1;
}

int stk_signal_connect(STK_Widget *widget, char *signal, F_Signal_Callback callback, void *userdata)
{
	return stk_signal_connectToObject((STK_Object *)widget, signal, callback, userdata);
}

int stk_signal_emit(STK_Widget *widget, char *signal, void *signaldata)
{
	stk_signal_emitToObject((STK_Object *)widget, signal, signaldata);
}

static int stk_signal_connectToObject(STK_Object *object, char *signal, F_Signal_Callback callback, void *userdata)
{
	if (slist_head == NULL) {
		fprintf(stderr, "Signal [%s] doesn't exist.\n", signal);
		return 0;	
	}
	else {
		SignalListNode *l = slist_head;
		while (l) {
			if (!strcmp(l->signal, signal))
				break;
			l = l->next;
		}
		
		if (l == NULl) {
			fprintf(stderr, "Signal [%s] doesn't exist.\n", signal);
			return 0;
		}
		
		if (l->call_list == NULL) {
			l->call_list = (CallbackListNode *)stk_malloc(sizeof(CallbackListNode));
			l->call_list->object = object;
			l->call_list->callback = callback;
			l->call_list->userdata = userdata;
			l->call_list->next = NULL;
			
			return 1;
		}
		else {
			CallbackListNode *cbl = l->call_list;
			while (cbl->next)
				cbl = cbl->next;
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
		if (!strcmp(l->signal, signal)) {
			if (l->call_list) {
				cbl = l->call_list;
				while (cbl) {
					if (object == cbl->object) {
						callback = cbl->callback;
						callback( object, signaldata, cbl->userdata );
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



