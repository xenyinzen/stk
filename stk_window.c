#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "stk.h"
#include "stk_prim.h"
#include "stk_widget.h"
#include "stk_window.h"

// global window variable: IMPORTANT
static STK_Window *g_window = NULL;

// create a new window, with dimension parameters
STK_Window *stk_window_new( Sint16 x, Sint16 y, Sint16 width, Sint16 height)
{
	// get the main video surface
	SDL_Surface *video = SDL_GetVideoSurface();
	if (video == NULL)
		return 0;
	
	STK_Window *win = (STK_Window *)stk_malloc(sizeof(STK_Window));
	
	win->widget.rect.x = x;
	win->widget.rect.y = y;
	win->widget.rect.w = width;
	win->widget.rect.h = height;
	
	// when create, the visible flags is set to 0, means not display now
	win->visible = 0;
	// nonsense
	win->type = 0;
	win->focus_widget = NULL;
	
	// create window surface
	win->widget.surface = stk_window_createSubSurface(video, &(win->widget.rect));
	win->bgcolor = 0x00d4d4d4;
	
	// set 'g_window' to default window
	g_window = win;
	return win;
}

// open the window, means display it on the screen
int stk_window_open()
{
	STK_Window *window = stk_window_get();
	if (window == NULL)
		return 0;
	window->type = 0;
	window->bgcolor = SDL_MapRGB(window->widget.surface->format, 0xd4, 0xd4, 0xd4);
	window->visible = 1;
	
	// realize the detailed structure
	fprintf(stderr, "Ready to realize the window.\n");
	stk_window_EventRealize();
	// draw window and display on screen
	fprintf(stderr, "Ready to redraw the window.\n");
	stk_window_EventRedraw();
	
	return 1;
}

int stk_window_init()
{
	return 1;
}

int stk_window_close()
{

}


// get the only window
STK_Window *stk_window_get()
{
	return g_window;	
}

// get the widget list of a window
STK_WidgetListNode *stk_window_getWidgetList()
{
	STK_Window *win = stk_window_get();	
	if (!win)
		return NULL;

	return win->widget_list;
}

// get the focus widget of a window
STK_Widget *stk_window_getFocusWidget()
{
	STK_Window *win = stk_window_get();
	if (!win)
		return NULL;
		
	return win->focus_widget;
}

int stk_window_setWidgetList(STK_WidgetListNode *wl)
{
	STK_Window *win = stk_window_get();	
	if (!win)
		return NULL;

	win->widget_list = wl;
	
	return 1;
}

// set the focus widget of a window
int stk_window_setFocusWidget(STK_Widget *fw)
{
	STK_Window *win = stk_window_get();
	if (!win)
		return 0;
	
	win->focus_widget = fw;
	
	return 1;
}

// create a new sub surface from the parent surface 'sur', using the 'rect' parameter
SDL_Surface *stk_window_createSubSurface(SDL_Surface *sur, SDL_Rect *rect)
{
	SDL_Surface *m = sur;
	SDL_Surface *subsur = NULL;
	
	void *pixels;
	pixels = (Uint8 *)(m->pixels) + m->pitch * rect->y
			+ m->format->BytesPerPixel * rect->x;
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

// get the window relative mouse location, fill them into 'x' and 'y'
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

// create the surface for specific widget
int stk_window_createWidgetSurface(STK_Widget *widget)
{
	STK_Window *win = stk_window_get();
	// if the location of widget excced the bounary of win, return directly
	if (widget->rect.x > win->widget.rect.x + win->widget.rect.w)
		return 0;
	if (widget->rect.y > win->widget.rect.y + win->widget.rect.h)
		return 0;
	
	if (widget->rect.x + widget->rect.w > win->widget.rect.x + win->widget.rect.w )
		widget->rect.w = win->widget.rect.x + win->widget.rect.w - widget->rect.x;
	
	if (widget->rect.y + widget->rect.h > win->widget.rect.y + win->widget.rect.h )
		widget->rect.h = win->widget.rect.y + win->widget.rect.h - widget->rect.y;

	widget->surface = stk_window_createSubSurface(win->widget.surface, &(widget->rect));
	
	// ?? why do this step?: to realize that widget using its private method
	stk_signal_emit(widget, "realize", NULL);
	
	return 1;
}

// add widget to window's widget list
int stk_window_addWidget(STK_Widget *widget)
{
	STK_WidgetListNode *item;
	
	STK_Window *win = stk_window_get();
	if (!win)
		return 0;
	
	item = (STK_WidgetListNode *)stk_malloc(sizeof(STK_WidgetListNode));
	item->widget = widget;
	if (widget->surface == NULL)
		stk_window_createWidgetSurface(widget);
	
	// if 'widget' is the first widget in the widget list
	if (win->widget_list == NULL) {
		win->widget_list = item;
		// if focus widget is NULL, and if the new widget is focusable, point this new widget
		if ((stk_window_getFocusWidget() == NULL) && (widget->flags & WIDGET_FOCUSABLE))
			win->focus_widget = widget;
	}
	else {
		STK_WidgetListNode *t;
		t = win->widget_list;
		// to the end of the widget list
		while (t->next)
			t = t->next;
		t->next = item;	
	}
	
	// show widget
	// stk_signal_emit(widget, "show", NULL);
	
	return 1;
}

STK_Widget *stk_window_removeWidget(STK_Widget *widget)
{
	// to fill later

}


// window realize event
int stk_window_EventRealize()
{
	STK_Window *window = stk_window_get();
	if (!window)
		return 0;
	
	SDL_Event event;
	event.type = STK_EVENT;
	event.user.code = STK_WINDOW_REALIZE;
	event.user.data1 = window;
	event.user.data2 = 0;
	
	SDL_PushEvent(&event);
	return 1;
}

// window redraw event
int stk_window_EventRedraw()
{
	STK_Window *window = stk_window_get();
	if (!window)
		return 0;

	SDL_Event event;
	event.type = STK_EVENT;
	event.user.code = STK_WINDOW_REDRAW;
	event.user.data1 = window;
	event.user.data2 = 0;
	
	SDL_PushEvent(&event);
	return 1;
}

// window realize function
int stk_window_realize()
{
	STK_Window *win = stk_window_get();
	if (!win)
		return 0;

	STK_WidgetListNode *item = stk_window_getWidgetList();
	
	while (item) {
		// create sub surface for each widget
		if (!item->widget->surface) {
			stk_window_createWidgetSurface(item->widget);
		}
		
		item = item->next;
	}
	
	// if win have no dimension, assign it the default value
	if (win->widget.rect.w == 0)
		win->widget.rect.w = 600;
	if (win->widget.rect.h == 0)
		win->widget.rect.h = 480;

	return 1;
}

// window redraw function
int stk_window_redraw()
{
	SDL_Surface *video = SDL_GetVideoSurface();
	STK_Window *win = stk_window_get();
	if (!win)
		return 0;
		
	win->visible = 1;
	
	// fill up background
	SDL_FillRect(win->widget.surface, NULL, win->bgcolor);
	// redraw all widget on the window
	stk_widget_drawAll();
	// update it on the screen
	// SDL_UpdateRect(win->widget.surface, 0, 0, 0, 0);  // why using this method not work?
	stk_window_updateRect(win->widget.rect.x, win->widget.rect.y, win->widget.rect.w, win->widget.rect.h);
	
	return 0;
}


// window Event dispather: IMPORTANT
int stk_window_Event( SDL_Event *event )
{
	STK_Window *win = stk_window_get();
	STK_WidgetListNode *wl = stk_window_getWidgetList();
	STK_Widget *w;
	
	int xrel, yrel;
	
	// if the event type is SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP, STK_EVENT and so on
	if (win && event->type >= SDL_MOUSEMOTION) {
		// calculate the relative coordinates
		xrel = event->motion.x - win->widget.rect.x;
		yrel = event->motion.y - win->widget.rect.y;
	}
	
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		// find out if mouse point is in certain widget 
		while (wl) {
			w = wl->widget;
			// judge whether the mouse point is on this widget area 
			if (stk_widget_isInside(w, xrel, yrel)) {
				// focusable means this widget could be clicked, inputed, and so on
				if (w->flags & WIDGET_FOCUSABLE) {
					// while mouse click, take the focus to this widget
					stk_window_setFocusWidget(w);
					// do a redraw, for change the widget's appearance to let people to see an effect.
					stk_widget_EventRedraw(w);
					// call the deeper event dispatcher function, to call the callback function of that widget
					stk_widget_Event(w, event);
					break;
				}
			
			}
			// go to next widget
			wl = wl->next;		
		}
	}

	return 1;
}

/*
int stk_window_Event0( SDL_Event *event)
{
	switch (event->type) {
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		case SDLL_TAB:
			
		
		
		}
	
	
	}
}
*/

/** =====================================================

		      Window Primitives		

    ===================================================== **/

int stk_window_drawLine(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	x1 += win->widget.rect.x;
	x2 += win->widget.rect.x;
	
	y1 += win->widget.rect.y;
	y2 += win->widget.rect.y;

	return stk_prim_drawLine(win->widget.surface, x1, y1, x2, y2, color);		
}

int stk_window_drawCircle(STK_Window *win, Sint16 x, Sint16 y, Sint16 r, Uint32 color)
{
	x += win->widget.rect.x;
	y += win->widget.rect.y + 10;
	
	return stk_prim_drawCircle(win->widget.surface, x, y, r, color);
}

int stk_window_fillRect(STK_Window *win, SDL_Rect *rect, Uint32 color)
{
	SDL_Rect r;
	r.x = rect->x + win->widget.rect.x;
	r.y = rect->y + win->widget.rect.y;
	r.w = rect->w;
	r.h = rect->h;
	
	SDL_FillRect(win->widget.surface, &r, color);
	
	return 1;
}

int stk_window_blitTo(STK_Window *win, SDL_Rect *des_r, SDL_Surface *src, SDL_Rect *src_r)
{
	SDL_Surface *dest = win->widget.surface;
	SDL_Rect r;
	
	r.x = win->widget.rect.x + des_r->x;
	r.y = win->widget.rect.y + des_r->y;
	r.w = des_r->w;
	r.h = des_r->h;
	
	SDL_BlitSurface(src, src_r, dest, &r);

	return 1;
}

int stk_window_drawBox(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	color = color << 8;
	color |= 0xff;

	x1 += win->widget.rect.x;
	x2 += win->widget.rect.x;
	y1 += win->widget.rect.y;
	y2 += win->widget.rect.y;
	
	return stk_prim_boxColor(win->widget.surface, x1, y1, x2, y2, color);
}


int stk_window_drawRect(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	color = color << 8;
	color |= 0xff;
	
	x1 += win->widget.rect.x;
	x2 += win->widget.rect.x;
	y1 += win->widget.rect.y;
	y2 += win->widget.rect.y;
	
	return stk_prim_rectColor(win->widget.surface, x1, y1, x2, y2, color);
}

int stk_window_updateRect( Sint16 x, Sint16 y, Sint16 w, Sint16 h)
{
	SDL_Surface *video = SDL_GetVideoSurface();
	
	SDL_UpdateRect(video, x, y, w, h);
	
	return 1;
}