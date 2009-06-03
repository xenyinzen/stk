#include <stdio.h>
#include <stdlib.h>

#include "stk.h"
#include "stk_widget.h"

static SDL_mutex *my_mutex;
// this is the global widget type array: IMPORTANT
static struct STK_WidgetType g_wlist[MAX_WIDGET_TYPE];

// widget initial function
// used to register some signal name into global signal list
int stk_widget_init()
{
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

	return 1;
}

// draw all widgets on a window
int stk_widget_drawAll(STK_Widget *window)
{
	F_Widget_Draw draw;
	STK_WidgetListNode *wlist;
	STK_Widget *widget;
	
	wlist = window->widget_list;
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
				// ?? need to redefine later
				draw(widget);	
			}
		// go to draw next widget
		wlist = wlist->next;
	}
	SDL_mutexV(my_mutex);
	SDL_UpdateRect(window->surface, 0, 0, 0, 0);
		
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

// 
int stk_widget_setDims(STK_Widget *widget, Sint16 x, Sint16 y, Sint16 w, Sint16 h)
{
	STK_Window *win;
	if (!widget)
		return 0;
	win = stk_window_get();
	if (!win) 
		return 0;
	
	widget->rect.x = x;
	widget->rect.y = y;
	widget->rect.w = w;
	widget->rect.h = h;
	
	if (widget->surface)
		SDL_FreeSurface(widget->surface);
	
	stk_window_createWidgetSurface(widget);
	
	return 1;
}

int stk_widget_close(STK_Widget *widget)
{
	// here, to free the widget and its child widgets

}

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
		while (t) {
			if (t->widget->flags & WIDGET_VISIBLE) {
				if (stk_rect_intersect(&widget->rect, &t->widget->rect, &inter)) {
					draw = stk_widget_getDraw(widget);
					if (draw) 
						draw(t->widget, &inter);
					else
						doupdate = 0;
				}
				else if (stk_rect_inside(&widget->rect, &t->widget->rect)) {
					draw = stk_widget_getDraw(widget);
					if (draw)
						draw(t->widget, &widget->rect);
					else
						doupdate = 0;
				}
				else if (stk_rect_inside(&t->widget->rect, &widget->rect)) {
					draw = stk_widget_getDraw(widget);
					if (draw)
						draw(t->widget, &widget->rect);
					else
						doupdate = 0;
				}
				else {}
			}
			t = t->next
		}
		
		if (doupdate) {
			SDL_Rect update;
			update.x = widget->rect.x + win->widget.rect.x;
			update.y = widget->rect.y + win->widget.rect.y;
			update.w = widget->rect.w;
			update.h = widget->rect.h;
			SDL_UpdateRect(win->widget.surface, update.x, update.y, update.w, update.h);			
		}
	}
	return 1;
}




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
					else 
						return 1;				
				}

	return 0;
}


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


int stk_widget_registerType( char *id, STK_WidgetFuncs *funcs )
{
	int i;
	for (i=0; i<MAX_WIDGET_TYPE; i++) {
		if (g_wlist[i].id) {
			if (!strcmp(g_wlist[i].id, id)) {
				fprintf(stderr, "Widget type [%s] has been registered.\n", id);
				return i;
			}
		}
		else {
			g_wlist[i].id = id;
			g_wlist[i].funcs = funcs;
			
			return i;			
		}
	}
	
	return 0;
}

char *stk_widget_getName( STK_Widget *widget )
{
	return g_wlist[widget->type].id;

}

F_Widget_Draw stk_widget_getDraw( STK_Widget *widget)
{
	return g_wlist[widget->type].funcs.draw;
}

STK_WidgetFuncs stk_widget_getFuncs(STK_Widget *widget)
{
	return g_wlist[widget->type].funcs;
}

