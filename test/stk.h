#ifndef _STK_H_
#define _STK_H_

#include "SDL.h"
#include "stk_window.h"
#include "stk_widget.h"
#include "stk_signal.h"
#include "stk_label.h"

int stk_init();
int stk_Main();
int stk_quit();

enum {
	STK_EVENT = SDL_USEREVENT,
};

enum {
	STK_WIDGET_HIDE = 1,
	STK_WIDGET_MOVE,
	STK_WIDGET_SHOW,
	STK_WIDGET_REDRAW,
	STK_WIDGET_RESIZE,
	STK_WINDOW_REALIZE,
	STK_WINDOW_REDRAW,
	STK_IMAGE_EVENT,
};

enum {
	STK_WIDGET_LABEL,
	STK_WIDGET_BUTTON,



};

#endif /* _STK_H_ */