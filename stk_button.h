#ifndef _STK_BUTTON_H_
#define _STK_BUTTON_H_


typedef enum STK_ButtonState {
	STK_BUTTON_DOWN,
	STK_BUTTON_UP,
	STK_BUTTON_HIGHLIGHTED,
	STK_BUTTON_FOCUSED
} STK_ButtonState;

typedef struct STK_Button {
	STK_Widget widget;
	STK_Frame frame;
	STK_ButtonState state;
	STK_Widget *label;
	STK_Font *font;
} STK_Button;






#endif /* _STK_BUTTON_H_ */