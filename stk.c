#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk.h"
#include "stk_widget.h"
#include "stk_window.h"



static int stk_pushEvent(SDL_Event *event);
static void stk_InternalEvent(SDL_Event *event);


/* Initial the video surface and the basic structures of STK.
 *
 **/
int stk_init(void)
{
	
	if ( SDL_GetVideoSurface() == NULL)
		return 0;
	
	// begin to initial some lower structures
	if (stk_signal_init() == 0)
		return 0;
	if (stk_window_init() == 0)
		return 0;
	if (stk_widget_init() == 0)
		return 0;
	/*
	if (stk_font_init() == 0)
		return 0;
	*/
	// enable keyboard settings
	SDL_EnableUNICODE(1);
	
	return 1;
}

int stk_Main(void)
{
	SDL_Event event;
	
	if (SDL_GetVideoSurface() == NULL)
		return 0;
	// clear the event's initial value to zero
	memset(&event, 0, sizeof(event));
	
	while (1) {
		while (SDL_WaitEvent(&event)) {
			if (stk_pushEvent(&event) == 0) {
				stk_quit();
				return 0;		
			}
		}
		// must here
		SDL_Delay(10);
	}
}

static int stk_pushEvent(SDL_Event *event)
{
	SDL_Event e;
	if (SDL_GetVideoSurface() == NULL)
		return 0;
	
	switch (event->type) {
	case SDL_QUIT:
		stk_window_close();
		return 0;
	case SDL_VIDEOEXPOSE:
	case SDL_ACTIVEEVENT:
		return 1;
	case STK_EVENT:
		stk_InternalEvent(event);
		break;
	default:
		break;	
	}
	
	while (SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_MOUSEMOTIONMASK) > 0);
	
	return stk_window_Event(event);
} 

static void stk_InternalEvent(SDL_Event *event)
{
	STK_WIDGET *widget;
	SDL_Rect *rect;
	switch (event->user.code) {
	case STK_WIDGET_HIDE:
		widget = event->user.data1;
		if (stk_widget_isActive(widget)) {
			if (widget->flags & WIDGET_VISIBLE) {
				widget->flags &= ~WIDGET_VISIBLE;
				// need to complement later: here could not use the Event function sets.
				stk_signal_emit(widget, "hide", NULL);
				stk_widget_draw(widget);
			}
		}
		break;
	case STK_WIDGET_SHOW:
		widget = event->user.data1;
		widget->flags |= WIDGET_VISIBLE;
		if (!(widget->flags & WIDGET_REALIZED)) {
			widget->flags |= WIDGET_REALIZED;
			stk_window_addWidget(widget);
		}
		
		if (widget->rect.w > 0 && widget->rect.h > 0) {
			// draw widget according its flags and other properties.
			stk_widget_draw(widget);
		}
		break;
	case STK_WIDGET_REDRAW:
		widget = event->user.data1;
		if (stk_widget_isActive(widget))
			stk_widget_draw(widget);
		break;
	case STK_WINDOW_REDRAW:
		stk_window_redraw(event->user.data1);
		break;
	case STK_WINDOW_REALIZE:
		stk_window_realize(event->user.data1);
		break;
	default:
		break;
	}


}