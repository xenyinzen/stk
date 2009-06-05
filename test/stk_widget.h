#ifndef _STK_WIDGET_H_
#define _STK_WIDGET_H_

#include "SDL.h"
#include "stk.h"

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


typedef void	(*F_Widget_Draw) 	(STK_Widget *widget, SDL_Rect *rect);
typedef int 	(*F_Widget_SizeReq)	(STK_Widget *widget, SDL_Rect *rect);
typedef void	(*F_Widget_Close)	(STK_Widget *widget);


typedef struct STK_WidgetFuncs {
	F_Widget_Draw 		draw;
//	F_Widget_SizeReq	sizereq;
//	F_Widget_Close		close;
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


// function declaration
int stk_widget_init();
int stk_widget_drawAll();
int stk_widget_Event(STK_Widget *widget, SDL_Event *event);
int stk_widget_setDims(STK_Widget *widget, Sint16 x, Sint16 y, Sint16 w, Sint16 h);
int stk_widget_close(STK_Widget *widget);
int stk_widget_isActive(STK_Widget *widget);
int stk_widget_draw(STK_Widget *widget);
int stk_widget_EventRedraw(STK_Widget *widget);
int stk_widget_EventHide(STK_Widget *widget);
int stk_widget_EventShow(STK_Widget *widget);
int stk_widget_isInside(STK_Widget *widget, int x, int y);
int stk_rect_isInside(const SDL_Rect *A, const SDL_Rect *B);
int stk_rect_isIntersect(const SDL_Rect *A, const SDL_Rect *B, SDL_Rect *in);
int stk_widget_initType();
int stk_widget_getType(STK_Widget *widget);
int stk_widget_getTypeByName( char *id );
int stk_widget_registerType( char *id, STK_WidgetFuncs *funcs );
char *stk_widget_getName( STK_Widget *widget );
F_Widget_Draw stk_widget_getDraw( STK_Widget *widget);
STK_WidgetFuncs stk_widget_getFuncs(STK_Widget *widget);

#endif /* _STK_WIDGET_H_ */