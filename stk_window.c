#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "stk.h"
#include "stk_widget.h"
#include "stk_window.h"

static STK_Window *g_window = NULL;

STK_Window *stk_window_new( Sint16 x, Sint16 y, Sint16 width, Sint16 height)
{
	SDL_Surface *video = SDL_GetVideoSurface();
	if (video == NULL)
		return 0;
	
	window = stk_malloc(sizeof(STK_Window));
	SDL_Rect rect;
	
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	
	window->visible = 0;
	window->type = 0;
	window->focus_widget = NULL;
	
	window->surface = stk_createSubSurface(video, &rect);
	window->bgcolor = SDL_MapRGB(window->surface->format, 0xd4, 0xd4, 0xd4);
	
	g_window = window;
	return window;
}


int stk_window_open(STK_Window *window)
{
	if (window == NULL)
		return 0;
	window->type = 0;
	window->bgcolor = SDL_MapRGB(window->surface->format, 0xd4, 0xd4, 0xd4);
	window->visible = 1;
	
	stk_window_EventRealize(window);
	stk_window_EventRedraw(window);
	
	return 1;
}

STK_Window *stk_window_get()
{
	return g_window;	
}

STK_WidgetList *stk_window_getWidgetList()
{
	STK_Window *win = stk_window_get();
	if (!win)
		return NULL;

	return win->widget_list;
}

STK_Widget *stk_window_getFocusWidget()
{
	STK_Window *win;
	win = stk_window_get();
	if (!win)
		return NULL;
		
	return win->focus_widget;
}

int stk_window_setFocusWidget(STK_Widget *fw)
{
	STK_Window *win;
	win = stk_window_get();
	win->focus_widget = fw;
	
	return 1;
}

SDL_Surface *stk_window_createSubSurface(SDL_Surface *sur, SDL_Rect *rect)
{
	SDL_Surface *m = sur;
	SDL_Surface *subsur = NULL;
	
	void *pixels;
	pixels = (char *)(m->pixels + m->pitch * rect->y
			+ m->format->BytesPerPixel * rect->x);
	if (rect->w > 0 && rect->h > 0)
		subsur = SDL_CreateRGBSurfaceFrom(
			pixels,
			rect->w,
			rect->h,
			m->format->BitsPerPixel,
			m->pitch,
			m->format->Rmask,
			m->format->Gmask,
			m->format->Bmask,
			m->format->Amask );	
	
	return subsur;
}

int stk_window_getMouseState( int *x, int *y )
{
	int abs_x, abs_y;
	STK_Window *win = stk_window_get();
	
	if (!win) 
		return 0;

	SDL_GetMouseState(&abs_x, &abs_y);
	*x = abs_x - win->widget.rect.x;
	*y = abs_y - win->widget.rect.y;	
	return 1;	
}

int stk_window_createWidgetSurface(STK_Widget *widget)
{
	STK_Window *win = stk_window_get();
	if (widget->rect.x + widget->rect.w > win->widget.rect.w )
		widget->rect.w = win->widget.rect.w - widget->rect.x;
	
	if (widget->rect.y + widget->rect.h > win->widget.rect.h )
		widget->rect.h = win->widget.rect.h - widget->rect.y;

	widget->surface = stk_window_createSubSurface(window->surface, &(widget->rect));
	
	// why do this step?
	stk_signal_emit(widget, "realize", NULL);
}

int stk_window_addWidget(STK_Widget *widget)
{
	STK_WidgetListNode *item;
	
	STK_Window *win = stk_window_get();
	if (!win)
		return 0;
	
	item = (STK_WidgetListNode *)stk_malloc(STK_WidgetListNode);
	item->widget = widget;
	if (widget->surface == NULL)
		stk_window_createWidgetSurface(widget);
	
	if (win->widget_list == NULL) {
		win->widget_list = item;
		if ((stk_window_getFocusWidget == NULL) && (widget->flags & WIDGET_FOCUSABLE))
			win->focus_widget = widget;
	}
	else {
		STK_WidgetListNode *t;
		t = win->widget_list;
		while (t->next)
			t = t->next;
		t->next = item;	
	}
	
	stk_signal_emit(widget, "show", NULL);
	
	return 1;
}

STK_Widget *stk_window_removeWidget(STK_Widget *widget)
{
	// to fill later

}

int stk_window_EventRealize(STK_Window *window)
{
	SDL_Event event;
	event.type = STK_EVENT;
	event.user.code = STK_WINDOW_REALIZE;
	event.user.data1 = window;
	event.user.data2 = 0;
	
	SDL_PushEvent(&event);
	return 1;
}

int stk_window_EventRedraw(STK_Window *window)
{
	SDL_Event event;
	event.type = STK_EVENT;
	event.user.code = STK_WINDOW_REDRAW;
	event.user.data1 = window;
	event.user.data2 = 0;
	
	SDL_PushEvent(&event);
	return 1
}


static int stk_window_realize( STK_Window *win)
{
	STK_WidgetListNode *item = stk_window_getWidgetList(win);
	SDL_Rect rect;
	
	while (item) {
		stk_widget_getSize(item->widget, &rect);
		if (!item->widget->surface) {
			stk_window_createWidgetSurface(win, item->widget);
		}
		
		item = item->next;
	}
	
	if (win->widget.rect.w == 0)
		win->widget.rect.w = 600;
	if (win->widget.rect.h == 0)
		win->widget.rect.h = 480;

	return 1;
}

static int stk_window_redraw( STK_Window *win )
{
	if (!win)
		return 0;
		
	win->visible = 1;
	
	SDL_FillRect(win->widget->surface, NULL, win->bgcolor);
	stk_widget_drawAll(win);
	SDL_UpdateRect(win->widget->surface, 0, 0, 0, 0);
}


