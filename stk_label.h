#ifndef _STK_LABEL_H_
#define _STK_LABEL_H_

#include "stk_widget.h"
#include "stk_font.h"

enum STK_LabelAlignment
{
	STK_LABEL_TOPLEFT,
	STK_LABEL_TOPCENTER,
	STK_LABEL_TOPRIGHT,
	STK_LABEL_CENTERLEFT,
	STK_LABEL_CENTER,
	STK_LABEL_CENTERRIGHT,
	STK_LABEL_BOTTOMLEFT,
	STK_LABEL_BOTTOMCENTER,
	STK_LABEL_BOTTOMRIGHT
};

typedef struct STK_Label {
	STK_Widget widget;
	STK_Font *font;
	char *caption;
    
	Uint32  bgcolor;
	Uint32  fgcolor;

	int alignment;
	int pattern;

	Uint32 fixed;

} STK_Label;

enum STK_LabelPattern
{
    STK_LABEL_NORMAL,
    STK_LABEL_BOUNCE,
    STK_LABEL_SCROLL_LEFT,
    STK_LABEL_SCROLL_RIGHT
};

STK_Widget* STK_LabelNew( char *str, Uint16 x, Uint16 y);
int STK_LabelClose(STK_Widget *widget);
int STK_LabelRegisterType();
void STK_LabelDraw(STK_Widget *widget);

int STK_LabelSetColor(STK_Widget *widget, int which, Uint32 color);
int STK_LabelSetAlignment(STK_Widget *widget, int alignment);


#endif /* _STK_LABEL_H_ */
