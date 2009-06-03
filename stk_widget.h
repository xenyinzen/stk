#ifndef _STK_WIDGET_H_
#define _STK_WIDGET_H_

// the void pointer?
typedef void* STK_Object;

typedef struct STK_Widget {
	SDL_Surface *surface;	// the surface drawn on is this widget
	
	SDL_Rect rect;		// the rect area this widget occupy
	SDL_Rect absolute;	// ??
	SDL_Rect clip;		// ??
	
	int lm, rm, tm, bm;
	char *name;		// name of this widget
	int type;		// identify the type of this widget, to index this type's other characters.
	int inside;		// ??
	Uint32 flags;		// important flag value variable
} STK_Widget;

typedef struct STK_WidgetListNode {
	STK_Widget *widget;			// point to a widget
	struct STK_Widget *parent;		// point to the parent widget of this widget
	struct STK_Widget *child;		// point to the child widget of this widget
	struct STK_WidgetListNode *next;	// point to the next widget of this widget (on the same window)
} STK_WidgetListNode;

typedef struct STK_WidgetFuncs {
	F_Widget_Draw 		draw;
	F_Widget_SizeReq	sizereq;
	F_Widget_Close		close;
} STK_WidgetFuncs;

typedef struct STK_WidgetType {
	char *id;
	STK_WidgetFuncs	funcs;
} STK_WidgetType;


// Macros againt to the STK_Widget.flags
#define WIDGET_FOCUSABLE	0x00000001
#define WIDGET_REALIZED		0x00000002
#define WIDGET_VISIBLE		0x00000004

// maximun widget kinds number
#define MAX_WIDGET_TYPE		100

#endif /* _STK_WIDGET_H_ */