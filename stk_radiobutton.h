#ifndef _STK_RADIOBUTTON_H_
#define _STK_RADIOBUTTON_H_

#include "stk_widget.h"
#include "stk_image.h"
#include "stk_label.h"

typedef enum STK_RadioButtonState {
	STK_RADIOBUTTON_RELEASE = 1,
	STK_RADIOBUTTON_TOGGLE
} STK_RadioButtonState;

typedef struct STK_RadioButton {
	STK_Widget widget;
	STK_Image image;
	STK_RadioButtonState state;
	STK_Label *label;

	Uint32 interval;
	Uint32 header_size;
	
	Uint32 flag;
} STK_RadioButton;

STK_RadioButton *STK_RadioButtonNew(char *caption, Uint16 x, Uint16 y, Uint16 w, Uint16 h);
void STK_RadioButtonDraw(STK_Widget *widget);
void STK_RadioButtonClose(STK_Widget *widget);
int STK_RadioButtonRegisterType();

void STK_RadioButtonFillLabel(STK_RadioButton *rb);
void STK_RadioButtonFilling(STK_RadioButton *rb, Uint32 pattern);
int STK_RadioButtonSetText(STK_RadioButton *rb, char *str);


#endif /* _STK_RADIOBUTTON_H_ */
