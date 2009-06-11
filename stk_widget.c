#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "stk.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_label.h"

static SDL_mutex *my_mutex;
// this is the global widget type array: IMPORTANT
static struct STK_WidgetType g_wlist[MAX_WIDGET_TYPE];

// widget initial function
// used to register some signal name into global signal list
int stk_widget_init()
{
	// register signal for basic widget
	stk_signal_new("mousebuttondown");
	stk_signal_new("mousebuttonup");
	stk_signal_new("mousemotion");
	stk_signal_new("keydown");
	stk_signal_new("hide");
	stk_signal_new("show");
	stk_signal_new("realize");
	stk_signal_new("margins");
	stk_signal_new("activate");

	// for button
	stk_signal_new("clicked");



	stk_label_registerType();

	return 1;
}

// draw all widgets on a window
int stk_widget_drawAll()
{
	F_Widget_Draw draw;
	STK_WidgetListNode *wlist;
	STK_Widget *widget;
	STK_Window *win = stk_window_get();
	
	// get the widget_list pointer
	wlist = win->widget_list;
	if (!wlist) {
		fprintf(stderr, "No widgets to draw!\n");
		return 0;
	} 
	
	SDL_mutexP(my_mutex);
	while (wlist) {
		widget = (STK_Widget *)wlist->widget;
		if (widget->flags & WIDGET_VISIBLE)
			// get the specified widget's draw function
			draw = stk_widget_getDraw(widget);
			if (draw) {
				// do really draw on each widgets
				draw(widget, NULL);	
			}
		// go to draw next widget
		wlist = wlist->next;
	}
	SDL_mutexV(my_mutex);
	// update whole window surface
	stk_window_updateRect(0, 0, 0, 0);
	//SDL_UpdateRect(win->widget.surface, 0, 0, 0, 0);
		
	return 1;
}

// widget event dispatcher function
// when we know which widget event occur on, use this function to separate the event type 
// and emit corresponding signal to that widget.
int stk_widget_Event(STK_Widget *widget, SDL_Event *event)
{
	switch (event->type) {
	case SDL_KEYDOWN:
		stk_signal_emit(widget, "keydown", event);
		break;
	case SDL_KEYUP:
		stk_signal_emit(widget, "keyup", event);
		break;
	case SDL_MOUSEMOTION:
		stk_signal_emit(widget, "mousemotion", event);
		break;
	case SDL_MOUSEBUTTONDOWN:
		stk_signal_emit(widget, "mousebuttondown", event);
		break;
	case SDL_MOUSEBUTTONUP:
		stk_signal_emit(widget, "mousebuttonup", event);
		break;
	default:
		break;	
	}
	
	return 1;
}

// set dimension rectagle for 'widget'
int stk_widget_setDims(STK_Widget *widget, Sint16 x, Sint16 y, Sint16 w, Sint16 h)
{
	STK_Window *win;
	if (!widget)
		return 0;
	win = stk_window_get();
	if (!win) 
		return 0;
	
	// fill up rect structure
	widget->rect.x = x;
	widget->rect.y = y;
	widget->rect.w = w;
	widget->rect.h = h;
	
	// widget has a surface, then free it first
 	if (widget->surface)
		SDL_FreeSurface(widget->surface);
	
	// create new sub surface on 'win' surface for 'widget'
	stk_window_createWidgetSurface(widget);
	
	return 1;
}

int stk_widget_close(STK_Widget *widget)
{
	// here, to free the widget and its child widgets
	// later finish it
	STK_WidgetListNode *prev;
	STK_WidgetListNode *wl = stk_window_getWidgetList();
	
	prev = NULL;
	// walk along with the widget list
	while (wl) {
		if (wl->widget == widget) {
			if (prev == NULL) {
				wl = wl->next;
				// reset the widget list head
				stk_window_setWidgetList(wl);
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
	stk_widget_draw(widget);
	
	// get the widget type related close function
	F_Widget_Close close = stk_widget_getClose(widget);
	if (close) {
		fprintf(stderr, "Ready to close widget: %x.\n", widget);
		close(widget);
	}

	// free this specific widget list node
	free(wl);
}

// check whether this 'widget' is in the widget list yet
int stk_widget_isActive(STK_Widget *widget)
{
	STK_Window *win = stk_window_get();
	STK_WidgetListNode *l;
	
	if (win) {
		l = stk_window_getWidgetList(win);
		while (l) {
			if (l->widget == widget)
				return 1;
			l = l->next;
		}
	}
	
	return 0;
}


// The main drawing function to widget, all windows and widgets 
// should be drawn by this function
int stk_widget_draw(STK_Widget *widget)
{
	STK_WidgetListNode *t;
	STK_Window *win = stk_window_get();
	F_Widget_Draw draw = NULL;
	SDL_Rect inter;
	int doupdate = 1;
	
	if (!win)
		return 0;
	
	t = win->widget_list;
	if (win->visible) {
		// walk along the widget list on 'win'
		
		
		if (widget->flags & WIDGET_DESTROY) {
			SDL_FillRect(win->widget.surface, &widget->rect, win->bgcolor);
			widget->flags &= ~WIDGET_DESTROY;
		}
		else {
		
		while (t) {
			// only compare with those visible widgets
			
			if (t->widget->flags & WIDGET_VISIBLE) {
				// judge if it is intersecting relation between specific widget and other widgets
				if (stk_rect_isIntersect(&widget->rect, &t->widget->rect, &inter)) {
					// get the specific draw function of that widget
					draw = stk_widget_getDraw(widget);
					if (draw) 
						// only update the intersecting part
						draw(t->widget, &inter);
					else
						doupdate = 0;
				}
				// judge if 'widget' is inside the list widget
				else if (stk_rect_isInside(&widget->rect, &t->widget->rect)) {
					draw = stk_widget_getDraw(widget);
					if (draw)
						// only draw 'widget' rect 
						draw(t->widget, &widget->rect);
					else
						doupdate = 0;
				}
				// judge if list widget is inside the 'widget'
				else if (stk_rect_isInside(&t->widget->rect, &widget->rect)) {
					draw = stk_widget_getDraw(widget);
					if (draw)
						draw(t->widget, &widget->rect);
					else
						doupdate = 0;
				}
				// if two widget have no overlap relation, do nothing
				else {}
			}
			t = t->next;
		}
		}
		
		// after drawing, we need to update screen
		if (doupdate) {
			SDL_Rect update;
			// get the absolute coordinates of widget.
			update.x = widget->rect.x + win->widget.rect.x;
			update.y = widget->rect.y + win->widget.rect.y;
			update.w = widget->rect.w;
			update.h = widget->rect.h;
			stk_window_updateRect(update.x, update.y, update.w, update.h);			
		}
	}

	return 1;
}

/** ================================================

                     Widget Events

  =============================================  **/

// redraw event
int stk_widget_EventRedraw(STK_Widget *widget)
{
	SDL_Event event;
	if (!widget)
		return 0;
	
	if (widget->flags & WIDGET_VISIBLE) {
		event.type = STK_EVENT;
		event.user.code = STK_WIDGET_REDRAW;
		event.user.data1 = widget;
		event.user.data2 = 0;
		
		SDL_PushEvent(&event);
	}
	return 1;
}

// hide event
int stk_widget_EventHide(STK_Widget *widget)
{
	SDL_Event event;
	if (!widget)
		return 0;
	
	event.type = STK_EVENT;
	event.user.code = STK_WIDGET_HIDE;
	event.user.data1 = widget;
	event.user.data2 = 0;
		
	SDL_PushEvent(&event);
	return 1;
}

// show event
int stk_widget_EventShow(STK_Widget *widget)
{
	SDL_Event event;
	if (!widget)
		return 0;
	
	if (!(widget->flags & WIDGET_VISIBLE)) {
		event.type = STK_EVENT;
		event.user.code = STK_WIDGET_SHOW;
		event.user.data1 = widget;
		event.user.data2 = 0;
		
		SDL_PushEvent(&event);
	}
	return 1;
}

/** ================================================

                 Utilities Function

  =============================================  **/

// check the mouse pointer is in the area of 'widget'
int stk_widget_isInside(STK_Widget *widget, int x, int y)
{
	if (x > widget->rect.x)
		if (y > widget->rect.y)
			if (x < (widget->rect.x + widget->rect.w))
				if (y < (widget->rect.y + widget->rect.h)) {
					if (widget->clip.w > 0) {
						if (x > widget->clip.x)
							if (y > widget->clip.y)
								if (x < (widget->clip.x + widget->clip.w))
									if (y < (widget->clip.y + widget->clip.h))
										return 1;
					}
					else {
						return 0;
					}
				}

	return 0;
}

// check whether rect A is inside rect B
int stk_rect_isInside(const SDL_Rect *A, const SDL_Rect *B)
{
	if (A->x >= B->x)
		if (A->y >= B->y)
			if (A->y <= B->y + B->h)
				if (A->x <= B->x + B->w)
					if (A->y + A->h <= B->y + B->h)
						return 1;
	
	return 0;
}

// check whether rect A and rect B is intersecting
// if is, the intersecting part info is filled into 'in'
int stk_rect_isIntersect(const SDL_Rect *A, const SDL_Rect *B, SDL_Rect *in)
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
int stk_widget_initType()
{
	int i;
	for (i=0; i<MAX_WIDGET_TYPE; i++) {
		memset(&g_wlist[i], 0, sizeof(STK_WidgetType));
	}

	return 1;
}


// get the specified widget type index
int stk_widget_getType(STK_Widget *widget)
{
	if (!widget) {
		fprintf(stderr, "NULL widget.\n");
		return 0;
	}
	
	return widget->type;		
}

// if we know the name string of one widget, and want to know its type index 
// in the type array, use this function.
int stk_widget_getTypeByName( char *id )
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
int stk_widget_registerType( char *id, STK_WidgetFuncs **f )
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
	
	return 0;
}

// get the type name of that widget
char *stk_widget_getName( STK_Widget *widget )
{
	return g_wlist[widget->type].id;
	// or:  return widget->name;
}

// get the draw function of that specific widget
F_Widget_Draw stk_widget_getDraw( STK_Widget *widget)
{
	return g_wlist[widget->type].funcs.draw;
}

F_Widget_Close stk_widget_getClose( STK_Widget *widget)
{
	return g_wlist[widget->type].funcs.close;
}

// get the set of functions of that widget
STK_WidgetFuncs stk_widget_getFuncs(STK_Widget *widget)
{
	return g_wlist[widget->type].funcs;
}

