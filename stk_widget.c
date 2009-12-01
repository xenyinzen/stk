#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_color.h"
#include "stk_label.h"
#include "stk_button.h"
#include "stk_slidebar.h"
#include "stk_entry.h"
#include "stk_radiobutton.h"
#include "stk_radiogroup.h"
#include "stk_progressbar.h"
#include "stk_msgbox.h"

static SDL_mutex *my_mutex;
// this is the global widget type array: IMPORTANT
static struct STK_WidgetType g_wlist[MAX_WIDGET_TYPE];

// widget initial function
// used to register some signal name into global signal list
int STK_WidgetInit()
{
	// register signal for basic widget
	STK_SignalNew("mousebuttondown");
	STK_SignalNew("mousebuttonup");
	STK_SignalNew("mousemotion");
	STK_SignalNew("keydown");
	STK_SignalNew("hide");
	STK_SignalNew("show");
	STK_SignalNew("realize");
	STK_SignalNew("margins");
	STK_SignalNew("activate");

	// for button
	STK_SignalNew("clicked");

	//========================================
	// register type for each kind of widget
	STK_LabelRegisterType();
	STK_ButtonRegisterType();
	STK_SlidebarRegisterType();
	STK_EntryRegisterType();
	STK_RadioButtonRegisterType();
	STK_RadioGroupRegisterType();
	STK_ProgressBarRegisterType();
	STK_MsgBoxRegisterType();

	return 0;
}

// draw all widgets on a window
int STK_WidgetDrawAll()
{
	F_Widget_Draw draw;
	STK_WidgetListNode *wlist;
	STK_Widget *widget;
	STK_Window *win = STK_WindowGetTop();
	
	// get the widget_list pointer
	wlist = win->widget_list;
	if (!wlist) {
		fprintf(stderr, "No widgets to draw!\n");
		return -1;
	} 
	
	SDL_mutexP(my_mutex);
	while (wlist) {
		widget = (STK_Widget *)wlist->widget;
		if (widget->flags & WIDGET_VISIBLE)
			// get the specified widget's draw function
			draw = STK_WidgetGetDraw(widget);
			if (draw) {
				// do really draw on each widgets
				draw(widget);	
			}
		// go to draw next widget
		wlist = wlist->next;
	}
	SDL_mutexV(my_mutex);
	// update whole window surface
	STK_WindowUpdateRect(0, 0, 0, 0);
	//SDL_UpdateRect(win->widget.surface, 0, 0, 0, 0);
		
	return 0;
}

// widget event dispatcher function
// when we know which widget event occur on, use this function to separate the event type 
// and emit corresponding signal to that widget.
int STK_WidgetEvent(STK_Widget *widget, SDL_Event *event)
{
	printf("Enter widget event processing function\n");
	switch (event->type) {
	case SDL_KEYDOWN:
		STK_SignalEmit(widget, "keydown", event);
		break;
	case SDL_KEYUP:
		STK_SignalEmit(widget, "keyup", event);
		break;
	case SDL_MOUSEMOTION:
		STK_SignalEmit(widget, "mousemotion", event);
		break;
	case SDL_MOUSEBUTTONDOWN:
		STK_SignalEmit(widget, "mousebuttondown", event);
		break;
	case SDL_MOUSEBUTTONUP:
		STK_SignalEmit(widget, "mousebuttonup", event);
		break;
	default:
		break;	
	}

	printf("exit widget event processing function\n");
	
	return 0;
}

// initial a widget instance
int STK_WidgetInitInstance(STK_Widget *widget)
{
	// colors
	STK_BaseColorAssign(&widget->bgcolor, STK_COLOR_GLOBAL_BACKGROUND);
	STK_BaseColorAssign(&widget->fgcolor, STK_COLOR_GLOBAL_FOREGROUND);

	return 0;
}


// set dimension rectagle for 'widget'
int STK_WidgetSetDims(STK_Widget *widget, SDL_Rect *r)
{
	STK_Window *win;
	if (!widget)
		return -1;
	win = STK_WindowGetTop();
	if (!win) 
		return -1;
	
	// fill up rect structure
	widget->rect.x = r->x;
	widget->rect.y = r->y;
	widget->rect.w = r->w;
	widget->rect.h = r->h;
	
	// widget has a surface, then free it first
 	if (widget->surface)
		SDL_FreeSurface(widget->surface);
	
	// create new sub surface on 'win' surface for 'widget'
	STK_WindowCreateWidgetSurface(widget);
	
	return 0;
}

int STK_WidgetClose(STK_Widget *widget)
{
	// here, to free the widget and its child widgets
	// later finish it
	STK_WidgetListNode *prev;
	STK_WidgetListNode *wl = STK_WindowGetWidgetList();
	
	prev = NULL;
	// walk along with the widget list
	while (wl) {
		if (wl->widget == widget) {
			if (prev == NULL) {
				wl = wl->next;
				// reset the widget list head
				STK_WindowSetWidgetList(wl);
			}
			else {
				// delete the wanted widget node
				prev->next = wl->next;
			}
			
			break;
		}
		prev = wl;
		wl = wl->next;
	}
	
	// clear the widget's drawing on the window
	// put these codes here, we feel afraid not very suitable, later we will write a DISTROY event to call
	widget->flags |= WIDGET_DESTROY;
	STK_WidgetDraw(widget);
	
	// get the widget type related close function
	F_Widget_Close close = STK_WidgetGetClose(widget);
	if (close) {
		fprintf(stderr, "Ready to close widget: %x.\n", widget);
		close(widget);
	}

	// free this specific widget list node
	free(wl);
}

// check whether this 'widget' is in the widget list yet
int STK_WidgetIsActive(STK_Widget *widget)
{
	STK_Window *win = STK_WindowGetTop();
	STK_WidgetListNode *l;
	
	if (win) {
		l = STK_WindowGetWidgetList(win);
		while (l) {
			if (l->widget == widget)
				return -1;
			l = l->next;
		}
	}
	
	return 0;
}


// The main drawing function to widget, all windows and widgets 
// should be drawn by this function
int STK_WidgetDraw(STK_Widget *widget)
{
	STK_Window *win = STK_WindowGetTop();
	F_Widget_Draw draw = NULL;
	int doupdate = 1;
	
	if (!win)
		return -1;
	
	if (win->visible) {
		// walk along the widget list on 'win'
		
		// clear widget appearance
		if (widget->flags & WIDGET_DESTROY) {
			// here, use real filling action function, rather than event generator
			SDL_FillRect(win->widget.surface, &widget->rect, win->bgcolor);
			// widget->flags &= ~WIDGET_DESTROY;
		}
		// hide
		else if (widget->flags & WIDGET_HIDDEN) {
			SDL_FillRect(win->widget.surface, &widget->rect, win->bgcolor);
			// widget->flags &= ~WIDGET_HIDE;
		}
		// showing
		else if (widget->flags & WIDGET_VISIBLE) {
			// get the specific draw function of that widget
			draw = STK_WidgetGetDraw(widget);
			if (draw)
				draw(widget);
			else
				doupdate = 0;
		}		

		// after drawing, we need to update screen
		if (doupdate) {
			SDL_Rect update;
			// get the absolute coordinates of widget.
			update.x = widget->rect.x + win->widget.rect.x;
			update.y = widget->rect.y + win->widget.rect.y;
			update.w = widget->rect.w;
			update.h = widget->rect.h;
			STK_WindowUpdateRect(update.x, update.y, update.w, update.h);			
		}
	}

	return 0;
}

/** ================================================

                     Widget Events

  =============================================  **/

// redraw event
int STK_WidgetEventRedraw(STK_Widget *widget)
{
	SDL_Event event;
	if (!widget)
		return -1;
	
	if (widget->flags & WIDGET_VISIBLE) {
		event.type = STK_EVENT;
		event.user.code = STK_WIDGET_REDRAW;
		event.user.data1 = widget;
		event.user.data2 = 0;
		
		SDL_PushEvent(&event);
	}
	return 0;
}

// hide event
int STK_WidgetEventHide(STK_Widget *widget)
{
	SDL_Event event;
	if (!widget)
		return -1;
	
	event.type = STK_EVENT;
	event.user.code = STK_WIDGET_HIDE;
	event.user.data1 = widget;
	event.user.data2 = 0;
		
	SDL_PushEvent(&event);
	return 0;
}

int STK_WidgetShow(STK_Widget *widget)
{
	return STK_WidgetEventShow(widget);
}

// show event
int STK_WidgetEventShow(STK_Widget *widget)
{
	SDL_Event event;
	if (!widget)
		return -1;
	
	if (!(widget->flags & WIDGET_VISIBLE)) {
		event.type = STK_EVENT;
		event.user.code = STK_WIDGET_SHOW;
		event.user.data1 = widget;
		event.user.data2 = 0;
		
		SDL_PushEvent(&event);
	}
	return 0;
}

/** ================================================

                 Utilities Function

  =============================================  **/

// check the mouse pointer is in the area of 'widget'
int STK_WidgetIsInside(STK_Widget *widget, int x, int y)
{
	if (x > widget->rect.x)
		if (y > widget->rect.y)
			if (x < (widget->rect.x + widget->rect.w))
				if (y < (widget->rect.y + widget->rect.h)) {
					//if (widget->clip.w > 0) {
					//	if (x > widget->clip.x)
					//		if (y > widget->clip.y)
					//			if (x < (widget->clip.x + widget->clip.w))
					//				if (y < (widget->clip.y + widget->clip.h))
										return -1;
				//	}
				//	else {
				//		return 0;
				//	}
				}

	return 0;
}

// check whether rect A is inside rect B
int STK_RectIsInside(const SDL_Rect *A, const SDL_Rect *B)
{
	if (A->x >= B->x)
		if (A->y >= B->y)
			if (A->y <= B->y + B->h)
				if (A->x <= B->x + B->w)
					if (A->y + A->h <= B->y + B->h)
						return -1;
	
	return 0;
}

// check whether rect A and rect B is intersecting
// if is, the intersecting part info is filled into 'in'
int STK_RectIsIntersect(const SDL_Rect *A, const SDL_Rect *B, SDL_Rect *in)
{
	Sint16 Amin, Amax, Bmin, Bmax;
	
	// horizontal
	Amin = A->x;
	Amax = Amin + A->w;
	Bmin = B->x;
	Bmax = Bmin + B->w;
	if (Bmin > Amin)
		Amin = Bmin;
	in->x = Amin;
	if (Bmax < Amax)
		Amax = Bmax;
	in->w = Amax - Amin > 0 ? Amax - Amin : 0;
	
	// vertical
	Amin = A->y;
	Amax = Amin + A->h;
	Bmin = B->x;
	Bmax = Bmin + B->h;
	if (Bmin > Amin)
		Amin = Bmin;
	in->y = Amin;
	if (Bmax < Amax)
		Amax = Bmax;
	in->h = Amax - Amin > 0 ? Amax - Amin : 0;
	
	return (in->w && in->h);
}

// widget type array initialization
int STK_WidgetInitType()
{
	int i;
	for (i=0; i<MAX_WIDGET_TYPE; i++) {
		memset(&g_wlist[i], 0, sizeof(STK_WidgetType));
	}

	return 0;
}


// get the specified widget type index
int STK_WidgetGetType(STK_Widget *widget)
{
	if (!widget) {
		fprintf(stderr, "NULL widget.\n");
		return 0;
	}
	
	return widget->type;		
}

// if we know the name string of one widget, and want to know its type index 
// in the type array, use this function.
int STK_WidgetGetTypeByName( char *id )
{
	int i;
	for (i=1; i<MAX_WIDGET_TYPE; i++) {
		if (g_wlist[i].id) {
			if (!strcmp(g_wlist[i].id, id))
				return i;
		}
		else {
			return 0;
		}
	}
		
	return 0;
}

// register a new widget type
// should supply two parameters: 
// 1. widget type name;
// 2. a set of widget functions
int STK_WidgetRegisterType( char *id, STK_WidgetFuncs **f )
{
	int i;
	for (i=0; i<MAX_WIDGET_TYPE; i++) {
		// when register a new type, should compare it to every old type name to avoid duplicating.
		if (g_wlist[i].id) {
			if (!strcmp(g_wlist[i].id, id)) {
				fprintf(stderr, "Widget type [%s] has been registered.\n", id);
				return i;
			}
		}
		// to the end of the widget type list, add new type to that global array
		else {
			g_wlist[i].id = id;
			*f = &g_wlist[i].funcs;
			return i;			
		}
	}
}

// get the type name of that widget
char *STK_WidgetGetName( STK_Widget *widget )
{
	return g_wlist[widget->type].id;
	// or:  return widget->name;
}

// get the draw function of that specific widget
F_Widget_Draw STK_WidgetGetDraw( STK_Widget *widget)
{
	return g_wlist[widget->type].funcs.draw;
}

F_Widget_Close STK_WidgetGetClose( STK_Widget *widget)
{
	return g_wlist[widget->type].funcs.close;
}

// get the set of functions of that widget
STK_WidgetFuncs STK_WidgetGetFuncs(STK_Widget *widget)
{
	return g_wlist[widget->type].funcs;
}



int STK_WidgetSetFixed(STK_Widget *widget, Uint32 flag)
{
	if (!widget)
		return -1;
		
	if (flag)
		widget->fixed = 1;
	else
		widget->fixed = 0;
		
	return 0;
}
