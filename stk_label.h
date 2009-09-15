#ifndef _STK_LABEL_H_
#define _STK_LABEL_H_

#include "stk_widget.h"
//#include "sdl_ttf.h"

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
//	TTF_Font *font;
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

STK_Widget* STK_LabelNew( char *str, Uint16 x, Uint16 y);
int STK_LabelClose(STK_Widget *widget);
int STK_LabelRegisterType();
void STK_LabelDraw(STK_Widget *widget);

int STK_LabelSetColor(STK_Widget *widget, int which, Uint32 color);
int STK_LabelSetAlignment(STK_Widget *widget, int alignment);


#endif /* _STK_LABEL_H_ */
