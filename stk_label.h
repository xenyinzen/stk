#ifndef _STK_LABEL_H_
#define _STK_LABEL_H_

#include "stk_widget.h"

enum 
{
	TOPLEFT,
	TOPCENTER,
	TOPRIGHT,
	CENTERLEFT,
	CENTER,
	CENTERRIGHT,
	BOTTOMLEFT,
	BOTTOMCENTER,
	BOTTOMRIGHT
};

typedef struct STK_Label {
	STK_Widget widget;
	int height;

	SDL_TimerID redraw_timer;
    
	Uint32  bgcolor;
	Uint32  fgcolor;

	int visible;
	int offset;
	int increase;
	int alignment;

	int pattern;

	char *caption;
} STK_Label;


enum LabelPattern
{
    LABEL_NORMAL,
    LABEL_BOUNCE,
    LABEL_SCROLL_LEFT,
    LABEL_SCROLL_RIGHT
} LabelPattern;

#define TRANSPARANT	0x00000000
#define FOREGROUND	1
#define BACKGROUND	2

STK_Widget* stk_label_new( char *str, Uint16 x, Uint16 y);
int stk_label_close(STK_Widget *widget);
int stk_label_registerType();
void stk_label_draw(STK_Widget *widget, SDL_Rect *area);

int stk_label_setColor(STK_Widget *widget, int which, Uint32 color);
int stk_label_setAlignment(STK_Widget *widget, int alignment);


#endif /* _STK_LABEL_H_ */
