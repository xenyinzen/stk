#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "stk_prim.h"
#include "stk_color.h"
#include "stk_widget.h"
#include "stk_window.h"

// global window variable: IMPORTANT
static STK_Window *g_window = NULL;

// create a new window, with dimension parameters
STK_Window *STK_WindowNew( Sint16 x, Sint16 y, Sint16 width, Sint16 height)
{
	STK_Widget *widget;
	// get the main video surface
	SDL_Surface *video = SDL_GetVideoSurface();

	if (video == NULL)
		return NULL;
	
	STK_Window *win = (STK_Window *)STK_Malloc(sizeof(STK_Window));
	widget = (STK_Widget *)win;
	
	widget->rect.x = x;
	widget->rect.y = y;
	widget->rect.w = width;
	widget->rect.h = height;
	
	// when create, the visible flags is set to 0, means not display now
	win->visible = 0;
	win->pressed = 0;
	// nonsense
	win->type = 0;
	win->focus_widget = NULL;
	
	// create window surface
	widget->surface = STK_WindowCreateSubSurface(video, &(win->widget.rect));
	win->bgcolor = STK_MCOLOR2INT(widget->surface, STK_COLOR_GLOBAL_BACKGROUND);
	
	// set 'g_window' to default window
	g_window = win;
	return win;
}

// open the window, means display it on the screen
int STK_WindowOpen()
{
	STK_Window *window = STK_WindowGetTop();
	if (window == NULL)
		return -1;
	window->type = 0;
	window->bgcolor = SDL_MapRGB(window->widget.surface->format, 0xd4, 0xd4, 0xd4);
	window->visible = 1;
	
	// realize the detailed structure
	STK_WindowEventRealize();
	// draw window and display on screen
	STK_WindowEventRedraw();
	
	return 0;
}

int STK_WindowInit()
{
	return 0;
}

int STK_WindowClose()
{

}


// get the top window
STK_Window *STK_WindowGetTop()
{
	return g_window;	
}

// get the widget list of a window
STK_WidgetListNode *STK_WindowGetWidgetList()
{
	STK_Window *win = STK_WindowGetTop();	
	if (!win)
		return NULL;

	return win->widget_list;
}

// get the focus widget of a window
STK_Widget *STK_WindowGetFocusWidget()
{
	STK_Window *win = STK_WindowGetTop();
	if (!win)
		return NULL;
		
	return win->focus_widget;
}

int STK_WindowSetWidgetList(STK_WidgetListNode *wl)
{
	STK_Window *win = STK_WindowGetTop();	
	if (!win)
		return -1;

	win->widget_list = wl;
	
	return 0;
}

// set the focus widget of a window
int STK_WindowSetFocusWidget(STK_Widget *fw)
{
	STK_Window *win = STK_WindowGetTop();
	if (!win)
		return 1;
	
	win->focus_widget = fw;
	
	return 0;
}

// create a new sub surface from the parent surface 'sur', using the 'rect' parameter
SDL_Surface *STK_WindowCreateSubSurface(SDL_Surface *sur, SDL_Rect *rect)
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
int STK_WindowGetMouseState( int *x, int *y )
{
	int abs_x, abs_y;
	STK_Window *win = STK_WindowGetTop();
	
	if (!win) 
		return -1;

	SDL_GetMouseState(&abs_x, &abs_y);
	*x = abs_x - win->widget.rect.x;
	*y = abs_y - win->widget.rect.y;	
	return 0;	
}

// create the surface for specific widget
int STK_WindowCreateWidgetSurface(STK_Widget *widget)
{
	STK_Window *win = STK_WindowGetTop();
	// if the location of widget excced the bounary of win, return directly
	if (widget->rect.x > win->widget.rect.x + win->widget.rect.w)
		return -1;
	if (widget->rect.y > win->widget.rect.y + win->widget.rect.h)
		return -1;
	
	// these statement is very important
	if (widget->rect.x + widget->rect.w > win->widget.rect.x + win->widget.rect.w )
		widget->rect.w = win->widget.rect.x + win->widget.rect.w - widget->rect.x;
	
	if (widget->rect.y + widget->rect.h > win->widget.rect.y + win->widget.rect.h )
		widget->rect.h = win->widget.rect.y + win->widget.rect.h - widget->rect.y;

	widget->surface = STK_WindowCreateSubSurface(win->widget.surface, &(widget->rect));
	
	// ?? why do this step?: to realize that widget using its private method
	// STK_SignalEmit(widget, "realize", NULL);
	
	return 0;
}

// add widget to window's widget list
int STK_WindowAddWidget(STK_Widget *widget)
{
	STK_WidgetListNode *item;
	
	STK_Window *win = STK_WindowGetTop();
	if (!win)
		return -1;
	
	item = (STK_WidgetListNode *)STK_Malloc(sizeof(STK_WidgetListNode));
	item->widget = widget;
	if (widget->surface == NULL)
		STK_WindowCreateWidgetSurface(widget);
	
	// if 'widget' is the first widget in the widget list
	if (win->widget_list == NULL) {
		win->widget_list = item;
		// if focus widget is NULL, and if the new widget is focusable, point this new widget
		if ((STK_WindowGetFocusWidget() == NULL) && (widget->flags & WIDGET_FOCUSABLE))
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
//	STK_SignalEmit(widget, "show", NULL);
	
	return 0;
}

STK_Widget *STK_WindowRemoveWidget(STK_Widget *widget)
{
	// to fill later

}


// window realize event
int STK_WindowEventRealize()
{
	STK_Window *window = STK_WindowGetTop();
	if (!window)
		return -1;
	
	SDL_Event event;
	event.type = STK_EVENT;
	event.user.code = STK_WINDOW_REALIZE;
	event.user.data1 = window;
	event.user.data2 = 0;
	
	SDL_PushEvent(&event);
	return 0;
}

// window redraw event
int STK_WindowEventRedraw()
{
	STK_Window *window = STK_WindowGetTop();
	if (!window)
		return -1;

	SDL_Event event;
	event.type = STK_EVENT;
	event.user.code = STK_WINDOW_REDRAW;
	event.user.data1 = window;
	event.user.data2 = 0;
	
	SDL_PushEvent(&event);
	return 0;
}

// window realize function
int STK_WindowRealize()
{
	STK_Window *win = STK_WindowGetTop();
	if (!win)
		return -1;

	STK_WidgetListNode *item = STK_WindowGetWidgetList();
	
	while (item) {
		// create sub surface for each widget
		if (!item->widget->surface) {
			STK_WindowCreateWidgetSurface(item->widget);
		}
		
		item = item->next;
	}
	
	// if win have no dimension, assign it the default value
	if (win->widget.rect.w == 0)
		win->widget.rect.w = 600;
	if (win->widget.rect.h == 0)
		win->widget.rect.h = 480;

	return 0;
}

// window redraw function
int STK_WindowRedraw()
{
	SDL_Surface *video = SDL_GetVideoSurface();
	STK_Window *win = STK_WindowGetTop();
	if (!win)
		return -1;
		
	win->visible = 1;
	
	// fill up background
	SDL_FillRect(win->widget.surface, NULL, win->bgcolor);
	// redraw all widget on the window
	STK_WidgetDrawAll();
	// update it on the screen
	// SDL_UpdateRect(win->widget.surface, 0, 0, 0, 0);  // why using this method not work?
	STK_WindowUpdateRect(win->widget.rect.x, win->widget.rect.y, win->widget.rect.w, win->widget.rect.h);
	
	return 0;
}


// window Event dispather: IMPORTANT
int STK_WindowEvent( SDL_Event *event )
{
	STK_Window *win = STK_WindowGetTop();
	STK_WidgetListNode *wl = STK_WindowGetWidgetList();
	STK_Widget *w;
	
	int xrel, yrel;
	
	// if the event type is SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP, STK_EVENT and so on
	if (win && event->type >= SDL_MOUSEMOTION) {
		// calculate the relative coordinates
		xrel = event->motion.x - win->widget.rect.x;
		yrel = event->motion.y - win->widget.rect.y;
	}
	
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		win->pressed = 1;
		// find out if mouse point is in certain widget 
		while (wl) {
			w = wl->widget;
			
			// judge whether the mouse point is on this widget area 
			if (STK_WidgetIsInside(w, xrel, yrel)) {
				// focusable means this widget could be clicked, inputed, and so on
				if (w->flags & WIDGET_FOCUSABLE) {
					// while mouse click, take the focus to this widget
					STK_WindowSetFocusWidget(w);
					// update state
					w->state = 1;
					// call the deeper event dispatcher function, to call the callback function of that widget
					STK_WidgetEvent(w, event);
					// do a redraw, for change the widget's appearance to let people to see an effect.
					STK_WidgetEventRedraw(w);
					break;
				}
			
			}
			// go to next widget
			wl = wl->next;		
		}
	}
	else if (event->type == SDL_MOUSEBUTTONUP) {
		win->pressed = 0;
		// find out if mouse point is in certain widget 
		while (wl) {
			w = wl->widget;
			
			// judge whether the mouse point is on this widget area 
			if (STK_WidgetIsInside(w, xrel, yrel)) {
				// focusable means this widget could be clicked, inputed, and so on
				if (w->flags & WIDGET_FOCUSABLE) {
					// while mouse click, take the focus to this widget
					// STK_WindowSetFocusWidget(w);
					// update state
					w->state = 2;
					// call the deeper event dispatcher function, to call the callback function of that widget
					STK_WidgetEvent(w, event);
					// do a redraw, for change the widget's appearance to let people to see an effect.
					STK_WidgetEventRedraw(w);
					break;
				}
			
			}
			// go to next widget
			wl = wl->next;		
		}
	}
	else if (event->type == SDL_MOUSEMOTION) {
		if (win->pressed == 1 && win->focus_widget) {
			w = win->focus_widget;
			// judge whether the mouse point is on this widget area 
			if (STK_WidgetIsInside(w, xrel, yrel) && (w->flags & WIDGET_DRAGABLE)) {
				// focusable means this widget could be clicked, inputed, and so on
				w->state = 3;
//				STK_WidgetEventRedraw(w);
				STK_WidgetEvent(w, event);
			}
		}		
	}
	else if (event->type == SDL_KEYDOWN) {
		if (win->focus_widget) {
			w = win->focus_widget;
			if (w->flags & WIDGET_FOCUSABLE) {
				STK_WidgetEvent(w, event);
			}
		} 
	}
	
	return 0;
}

/** =====================================================

		      Window Primitives		

    ===================================================== **/

int STK_WindowDrawLine(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	x1 += win->widget.rect.x;
	x2 += win->widget.rect.x;
	
	y1 += win->widget.rect.y;
	y2 += win->widget.rect.y;

	return STK_PrimDrawLine(win->widget.surface, x1, y1, x2, y2, color);		
}

int STK_WindowDrawCircle(STK_Window *win, Sint16 x, Sint16 y, Sint16 r, Uint32 color)
{
	x += win->widget.rect.x;
	y += win->widget.rect.y + 10;
	
	return STK_PrimDrawCircle(win->widget.surface, x, y, r, color);
}

int STK_WindowFillRect(STK_Window *win, SDL_Rect *rect, Uint32 color)
{
	SDL_Rect r;
	r.x = rect->x + win->widget.rect.x;
	r.y = rect->y + win->widget.rect.y;
	r.w = rect->w;
	r.h = rect->h;
	
	SDL_FillRect(win->widget.surface, &r, color);
	
	return 0;
}

int STK_WindowBlitTo(STK_Window *win, SDL_Rect *des_r, SDL_Surface *src, SDL_Rect *src_r)
{
	SDL_Surface *dest = win->widget.surface;
	SDL_Rect r;
	
	r.x = win->widget.rect.x + des_r->x;
	r.y = win->widget.rect.y + des_r->y;
	r.w = des_r->w;
	r.h = des_r->h;
	
	SDL_BlitSurface(src, src_r, dest, &r);

	return 0;
}

int STK_WindowDrawBox(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	color = color << 8;
	color |= 0xff;

	x1 += win->widget.rect.x;
	x2 += win->widget.rect.x;
	y1 += win->widget.rect.y;
	y2 += win->widget.rect.y;
	
	return STK_PrimBoxColor(win->widget.surface, x1, y1, x2, y2, color);
}


int STK_WindowDrawRect(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	color = color << 8;
	color |= 0xff;
	
	x1 += win->widget.rect.x;
	x2 += win->widget.rect.x;
	y1 += win->widget.rect.y;
	y2 += win->widget.rect.y;
	
	return STK_PrimRectColor(win->widget.surface, x1, y1, x2, y2, color);
}

int STK_WindowUpdateRect( Sint16 x, Sint16 y, Sint16 w, Sint16 h)
{
	SDL_Surface *video = SDL_GetVideoSurface();
	
	SDL_UpdateRect(video, x, y, w, h);
	
	return 0;
}