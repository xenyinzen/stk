#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk.h"
#include "stk_widget.h"
#include "stk_window.h"


static int STK_DispatchEvent(SDL_Event *event);
static void STK_InternalEvent(SDL_Event *event);

/* Initial the video surface and the basic structures of STK.
 *
 **/
int STK_Init()
{
	if ( SDL_GetVideoSurface() == NULL)
		return 0;
	
	// begin to initial some lower structures
	if (STK_SignalInit() == 0)
		return 0;
	if (STK_WindowInit() == 0)
		return 0;
	if (STK_WidgetInitType() == 0)
		return 0;
	if (STK_WidgetInit() == 0)
		return 0;
	if (STK_FontInit() == 0)
		return 0;

	// enable keyboard settings
	SDL_EnableUNICODE(1);
	
	return 1;
}

int STK_Main()
{
	SDL_Event event;
	
	if (SDL_GetVideoSurface() == NULL)
		return 0;
	// clear the event's initial value to zero
	memset(&event, 0, sizeof(event));
	
	while (1) {
		while (SDL_WaitEvent(&event)) {
			if (STK_DispatchEvent(&event) == 0) {
				STK_Quit();
				return 0;		
			}
		}
		// must here
		SDL_Delay(10);
	}
}

static int STK_DispatchEvent(SDL_Event *event)
{
	SDL_Event e;
	if (SDL_GetVideoSurface() == NULL)
		return 0;
	
	switch (event->type) {
	// close the window
	case SDL_QUIT:
		STK_WindowClose();
		return 0;
	// ignore, jump over
	case SDL_VIDEOEXPOSE:
	case SDL_ACTIVEEVENT:
		return 1;
	// enter internal event handler
	case STK_EVENT:
		STK_InternalEvent(event);
		break;
	default:
		break;	
	}
	
	// get rid of the mouse motion events from the event queen
	while (SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_MOUSEMOTIONMASK) > 0);
	
	// go to window event dispather
	return STK_WindowEvent(event);
} 

static void STK_InternalEvent(SDL_Event *event)
{
	STK_Widget *widget;
	SDL_Rect *rect;
	switch (event->user.code) {
	case STK_WIDGET_HIDE:
		widget = event->user.data1;
		if (STK_WidgetIsActive(widget)) {
			if (widget->flags & WIDGET_VISIBLE) {
				widget->flags &= ~WIDGET_VISIBLE;
				// need to complement later: here could not use the Event function sets.
				STK_SignalEmit(widget, "hide", NULL);
				STK_WidgetDraw(widget);
			}
		}
		break;
	case STK_WIDGET_SHOW:
		widget = event->user.data1;
		widget->flags |= WIDGET_VISIBLE;
		if (!(widget->flags & WIDGET_REALIZED)) {
			widget->flags |= WIDGET_REALIZED;
			STK_WindowAddWidget(widget);
		}
		
		if (widget->rect.w > 0 && widget->rect.h > 0) {
			// draw widget according its flags and other properties.
			STK_WidgetDraw(widget);
		}
		break;
	case STK_WIDGET_REDRAW:
		widget = event->user.data1;
		if (STK_WidgetIsActive(widget))
			STK_WidgetDraw(widget);
		break;
	case STK_WINDOW_REDRAW:
		//stk_window_redraw(event->user.data1);
		STK_WindowRedraw();
		break;
	case STK_WINDOW_REALIZE:
		//stk_window_realize(event->user.data1);
		STK_WindowRealize();
		break;
	default:
		break;
	}


}

int STK_Quit()
{
	return 0;
}