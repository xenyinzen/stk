#ifndef _STK_BUTTON_H_
#define _STK_BUTTON_H_

#include "stk_widget.h"
#include "stk_image.h"
#include "stk_label.h"

typedef enum STK_ButtonState {
	STK_BUTTON_DOWN = 1,
	STK_BUTTON_UP,
	STK_BUTTON_HIGHLIGHTED,
	STK_BUTTON_FOCUSED
} STK_ButtonState;

typedef struct STK_Button {
	STK_Widget widget;
	STK_Image image;
	STK_ButtonState state;
	STK_Label *label;
	Uint32 flag;
} STK_Button;

STK_Button *STK_ButtonNew(char *caption, Uint16 x, Uint16 y, Uint16 w, Uint16 h);
void STK_ButtonDraw(STK_Widget *widget);
void STK_ButtonClose(STK_Widget *widget);
void STK_ButtonFilling(STK_Button *button, Uint32 pattern);
void STK_ButtonFillLabel(STK_Button *button);

int STK_ButtonRegisterType();
int STK_ButtonSetText(STK_Button *button, char *str);


#endif /* _STK_BUTTON_H_ */