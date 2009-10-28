#ifndef _STK_WINDOW_H_
#define _STK_WINDOW_H_

#include "stk_widget.h"

typedef struct STK_Window {
	STK_Widget widget;
	
	Uint8 type;
	Uint32 bgcolor;
	int visible;
	int pressed;		// for global drag effects.

	STK_Widget *focus_widget;
	STK_WidgetListNode *widget_list;
} STK_Window;


STK_Window *STK_WindowNew( Sint16 x, Sint16 y, Sint16 width, Sint16 height);
int STK_WindowOpen();
int STK_WindowClose();
STK_Window *STK_WindowGetTop();
STK_WidgetListNode *STK_WindowGetWidgetList();
STK_Widget *STK_WindowGetFocusWidget();
int STK_WindowSetFocusWidget(STK_Widget *fw);
SDL_Surface *STK_WindowCreateSubSurface(SDL_Surface *sur, SDL_Rect *rect);
int STK_WindowGetMouseState( int *x, int *y );
int STK_WindowCreateWidgetSurface(STK_Widget *widget);
int STK_WindowAddWidget(STK_Widget *widget);
STK_Widget *STK_WindowRemoveWidget(STK_Widget *widget);
int STK_WindowEventRealize();
int STK_WindowEventRedraw();
int STK_WindowRealize();
int STK_WindowRedraw();
int STK_WindowEvent( SDL_Event *event );
int STK_WindowDrawLine(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int STK_WindowDrawCircle(STK_Window *win, Sint16 x, Sint16 y, Sint16 r, Uint32 color);
int STK_WindowFillRect(STK_Window *win, SDL_Rect *rect, Uint32 color);
int STK_WindowBlitTo(STK_Window *win, SDL_Rect *des_r, SDL_Surface *src, SDL_Rect *src_r);
int STK_WindowDrawBox(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int STK_WindowDrawRect(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int STK_WindowUpdateRect( Sint16 x, Sint16 y, Sint16 w, Sint16 h);


#endif /* _STK_WINDOW_H_ */
