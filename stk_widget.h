#ifndef _STK_WIDGET_H_
#define _STK_WIDGET_H_

#include "SDL.h"

// the basic void pointer type
typedef void *STK_Object;

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
	Uint32 state;
} STK_Widget;

typedef struct STK_WidgetListNode {
	STK_Widget *widget;			// point to a widget
	struct STK_Widget *parent;		// point to the parent widget of this widget
	struct STK_Widget *child;		// point to the child widget of this widget
	struct STK_WidgetListNode *next;	// point to the next widget of this widget (on the same window)
} STK_WidgetListNode;


typedef void	(*F_Widget_Draw) 	(STK_Widget *widget);
typedef void	(*F_Widget_Close)	(STK_Widget *widget);


typedef struct STK_WidgetFuncs {
	F_Widget_Draw 		draw;
	F_Widget_Close		close;
} STK_WidgetFuncs;

typedef struct STK_WidgetType {
	char *id;
	STK_WidgetFuncs	funcs;
} STK_WidgetType;

enum {
	STK_WIDGET_HIDE = 1,
	STK_WIDGET_MOVE,
	STK_WIDGET_SHOW,
	STK_WIDGET_REDRAW,
	STK_WIDGET_RESIZE,
	STK_WINDOW_REALIZE,
	STK_WINDOW_REDRAW,
	STK_IMAGE_EVENT
};

enum {
	STK_EVENT = SDL_USEREVENT
};

enum {
	STK_WIDGET_LABEL = 0,
	STK_WIDGET_BUTTON,
	STK_WIDGET_SLIDEBAR,
	STK_WIDGET_ENTRY,
	STK_WIDGET_RADIOBUTTON,
	STK_WIDGET_RADIOGROUP,
	STK_WIDGET_PROGRESSBAR,
	STK_WIDGET_MSGBOX
};

// Macros againt to the STK_Widget.flags
#define WIDGET_FOCUSABLE	0x00000001
#define WIDGET_REALIZED		0x00000002
#define WIDGET_VISIBLE		0x00000004
#define WIDGET_HIDDEN		0x00000008
#define WIDGET_DRAGABLE		0x00000010
#define WIDGET_DESTROY		0x80000000


// maximun widget kinds number
#define MAX_WIDGET_TYPE		100


// function declaration
int STK_WidgetInit();
int STK_WidgetDrawAll();
int STK_WidgetEvent(STK_Widget *widget, SDL_Event *event);
int STK_WidgetSetDims(STK_Widget *widget, Sint16 x, Sint16 y, Sint16 w, Sint16 h);
int STK_WidgetClose(STK_Widget *widget);
int STK_WidgetIsActive(STK_Widget *widget);
int STK_WidgetDraw(STK_Widget *widget);
int STK_WidgetEventRedraw(STK_Widget *widget);
int STK_WidgetEventHide(STK_Widget *widget);
int STK_WidgetEventShow(STK_Widget *widget);
int STK_WidgetIsInside(STK_Widget *widget, int x, int y);
int STK_RectIsInside(const SDL_Rect *A, const SDL_Rect *B);
int STK_RectIsIntersect(const SDL_Rect *A, const SDL_Rect *B, SDL_Rect *in);
int STK_WidgetInitType();
int STK_WidgetGetType(STK_Widget *widget);
int STK_WidgetGetTypeByName( char *id );
int STK_WidgetRegisterType( char *id, STK_WidgetFuncs **funcs );
char *STK_WidgetGetName( STK_Widget *widget );
F_Widget_Draw STK_WidgetGetDraw( STK_Widget *widget);
F_Widget_Close STK_WidgetGetClose(STK_Widget *widget);
STK_WidgetFuncs STK_WidgetGetFuncs(STK_Widget *widget);

#endif /* _STK_WIDGET_H_ */