#ifndef _STK_SIGNAL_H_
#define _STK_SIGNAL_H_

#include "stk_widget.h"

// Callback function type define
typedef void (*F_Signal_Callback)(STK_Object *object, void *signaldata, void *userdata);

// Callback list node type define
typedef struct CallbackListNode {
	STK_Object *object;
	F_Signal_Callback callback;
	void *userdata;
	struct CallbackListNode *next;
} CallbackListNode;

// Signal list node type define
typedef struct SignalListNode {
	char *signal;
	int type;
	struct CallbackListNode *call_list;
	struct SignalListNode *next;
} SignalListNode;


int STK_SignalInit();
int STK_SignalNew(char *signal);
int STK_SignalConnect(STK_Widget *widget, char *signal, F_Signal_Callback callback, void *userdata);
int STK_SignalEmit(STK_Widget *widget, char *signal, void *signaldata);


#endif /* _STK_SIGNAL_H_ */
