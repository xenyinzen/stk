#ifndef _STK_MSGBOX_H_
#define _STK_MSGBOX_H_

#include "SDL.h"
#include "stk_text.h"
#include "stk_font.h"

#define STK_MSGBOX_LINEBUF_NUM	50

typedef struct STK_MsgBox {
	STK_Widget widget;
	STK_Font *font;			// font type and size pointer

	STK_Text *linebuf[STK_MSGBOX_LINEBUF_NUM];	// point to line bufs
	SDL_Rect textarea;		// area to fill up text
	
	Uint16 start_line;		// start line to draw
	Uint16 end_line;		// end line to draw
	
	Uint16 cur_x;			// current x coordinate
	Uint16 cur_y;			// current y coordinate
	
	Uint32 interval;		// the interval between text
	Uint32 log;			// log or not?

} STK_MsgBox;

STK_MsgBox *STK_MsgBoxNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, char *str);
void STK_MsgBoxDraw(STK_Widget *widget);
void STK_MsgBoxClose(STK_Widget *widget);
int STK_MsgBoxRegisterType();

int STK_MsgBoxAddMsg(STK_MsgBox *msgbox, char *str);
int STK_MsgBoxSetFont(STK_MsgBox *msgbox, STK_Font *font);

#endif /* _STK_MSGBOX_H_ */
