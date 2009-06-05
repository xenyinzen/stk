#ifndef _STK_WINDOW_H_
#define _STK_WINDOW_H_

#include "stk_widget.h"

typedef struct STK_Window {
	STK_Widget widget;
	
	Uint8 type;
	Uint32 bgcolor;
	int visible;

	STK_Widget *focus_widget;
	STK_WidgetListNode *widget_list;
} STK_Window;


STK_Window *stk_window_new( Sint16 x, Sint16 y, Sint16 width, Sint16 height);
int stk_window_open();
int stk_window_close();
STK_Window *stk_window_get();
STK_WidgetListNode *stk_window_getWidgetList();
STK_Widget *stk_window_getFocusWidget();
int stk_window_setFocusWidget(STK_Widget *fw);
SDL_Surface *stk_window_createSubSurface(SDL_Surface *sur, SDL_Rect *rect);
int stk_window_getMouseState( int *x, int *y );
int stk_window_createWidgetSurface(STK_Widget *widget);
int stk_window_addWidget(STK_Widget *widget);
STK_Widget *stk_window_removeWidget(STK_Widget *widget);
int stk_window_EventRealize();
int stk_window_EventRedraw();
int stk_window_realize();
int stk_window_redraw();
int stk_window_Event( SDL_Event *event );
int stk_window_drawLine(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int stk_window_drawCircle(STK_Window *win, Sint16 x, Sint16 y, Sint16 r, Uint32 color);
int stk_window_fillRect(STK_Window *win, SDL_Rect *rect, Uint32 color);
int stk_window_blitTo(STK_Window *win, SDL_Rect *des_r, SDL_Surface *src, SDL_Rect *src_r);
int stk_window_drawBox(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int stk_window_drawRect(STK_Window *win, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);



#endif /* _STK_WINDOW_H_ */