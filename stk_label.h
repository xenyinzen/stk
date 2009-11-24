#ifndef _STK_LABEL_H_
#define _STK_LABEL_H_

#include "stk_widget.h"
#include "stk_font.h"

enum STK_LabelAlignment {
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

enum STK_LabelPattern {
    	STK_LABEL_NORMAL,
    	STK_LABEL_BOUNCE,
    	STK_LABEL_SCROLL_LEFT,
    	STK_LABEL_SCROLL_RIGHT
};

typedef struct STK_Label {
	STK_Widget widget;
	STK_Font *font;
	char *caption;
    
	Uint32  bgcolor;
	Uint32  fgcolor;

	int alignment;
	int pattern;
} STK_Label;

STK_Label* STK_LabelNew( char *str, Uint16 x, Uint16 y);
int STK_LabelClose(STK_Widget *widget);
void STK_LabelDraw(STK_Widget *widget);
int STK_LabelRegisterType();

int STK_LabelSetColor(STK_Label *label, int which, Uint8 r, Uint8 g, Uint8 b);
int STK_LabelSetAlignment(STK_Label *label, int alignment);
int STK_LabelSetText(STK_Label *label, char *str);
char *STK_LabelGetText(STK_Label *label);

#endif /* _STK_LABEL_H_ */
