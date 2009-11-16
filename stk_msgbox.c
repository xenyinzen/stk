#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_base.h"
#include "stk_widget.h"
#include "stk_text.h"
#include "stk_msgbox.h"

#define STK_MSGBOX_BORDER_THICKNESS	2



int STK_MsgBoxCalcDisplayLineWindow(STK_MsgBox *msgbox, int font_height);
int STK_MsgBoxCalcTextArea(STK_MsgBox *msgbox);


STK_Widget *STK_MsgBoxNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, char *str)
{
	STK_MsgBox *msgbox;
	STK_Widget *widget;
	int i = 0;
	
	msgbox = (STK_MsgBox *)STK_Malloc(sizeof(STK_MsgBox));
	widget = (STK_Widget *)msgbox;
	
	widget->type = STK_WIDGET_MSGBOX;
	widget->name = "MsgBox";
	widget->flags = 0;
	
	widget->rect.x = x;
	widget->rect.y = y;
	widget->rect.w = w;
	widget->rect.h = h;
	
	msgbox->start_line = 0;
	msgbox->end_line = 0;
	msgbox->border = STK_MSGBOX_BORDER_THICKNESS;
	msgbox->interval = 2*STK_MSGBOX_BORDER_THICKNESS;
	
	STK_MsgBoxCalcTextArea(msgbox);
	
	msgbox->cur_x = 0;
	msgbox->cur_y = 0;
	
	if (str) {
		msgbox->linebuf[0] = STK_TextNew(str);
		msgbox->end_line++;
	}	

	return widget;
}

void STK_MsgBoxDraw(STK_Widget *widget)
{
	STK_MsgBox *msgbox = (STK_MsgBox *)widget;
	SDL_Rect rect;
	int font_height;
	SDL_Color fg, bg;
	int i;
		
	// fillup background
	SDL_FillRect(widget->surface, NULL, 0x00f0f0f0);
	
	font_height = STK_FontGetHeight();
	STK_MsgBoxCalcDisplayLineWindow(msgbox, font_height);
	
	// fill text using font
	fg.r = (Uint8)((0x00101010 >> 16) & 0xff);
	fg.g = (Uint8)((0x00101010 >> 8) & 0xff);
	fg.b = (Uint8)((0x00101010 >> 0) & 0xff);
	
	bg.r = (Uint8)((0x00f0f0f0 >> 16) & 0xff);
	bg.g = (Uint8)((0x00f0f0f0 >> 8) & 0xff);
	bg.b = (Uint8)((0x00f0f0f0 >> 0) & 0xff);
	
	printf("In MsgBoxDraw\n");
	printf("msgbox->start_line is: %d.\n", msgbox->start_line);
	printf("msgbox->end_line is: %d.\n", msgbox->end_line);
	if (msgbox->start_line < msgbox->end_line) {
		for (i = 0; i<msgbox->end_line - msgbox->start_line; i++) {

			rect.x = msgbox->textarea.x;
			rect.y = msgbox->textarea.y + i*font_height + (i>0? (i-1)*msgbox->interval: 0);
			// here, we must ensure that linebuf[i]->data is valid
			STK_FontDraw(widget, msgbox->linebuf[i+msgbox->start_line]->data, &rect, &fg, &bg );
		
		}	
	}
}

void STK_MsgBoxClose(STK_Widget *widget)
{
	STK_MsgBox *msgbox = (STK_MsgBox *)widget;
	int i = 0;
/*	
	// release the text line buffer
	for (i = 0; i< STK_MSGBOX_LINEBUF_NUM; i++) {
		if (msgbox->linebuf[i]) {
			STK_TextFree(msgbox->linebuf[i]);
		}
	
	}
*/
	// release surface
	if (widget->surface)
		SDL_FreeSurface(widget->surface);
			
	// release STK_MsgBox
	free(msgbox);	
}

int STK_MsgBoxRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "MsgBox", &f );
        f->draw = STK_MsgBoxDraw;
        f->close = STK_MsgBoxClose;

        return 0;
}



int STK_MsgBoxCalcTextArea(STK_MsgBox *msgbox)
{
	STK_Widget *widget = (STK_Widget *)msgbox;
	
	msgbox->textarea.x = msgbox->border;
	msgbox->textarea.y = msgbox->border;
	msgbox->textarea.w = widget->rect.w - 2*msgbox->border;
	msgbox->textarea.h = widget->rect.h - 2*msgbox->border;
}


// now, we only use linear array to store output message, not circle array
int STK_MsgBoxCalcDisplayLineWindow(STK_MsgBox *msgbox, int font_height)
{
	int sum;
	int d = msgbox->end_line - msgbox->start_line;
	
	if (d <= 0)
		return -1;
	
	sum = font_height*d + msgbox->interval*(d - 1);
	
	printf("sum = %d\n", sum);
	printf("In msgbox: msgbox->textarea.h = %d\n", msgbox->textarea.h );
	while (sum >= msgbox->textarea.h) {
		sum -= (font_height + msgbox->interval);
		// move the top line up
		msgbox->start_line++;
		printf("sum = %d\n", sum);
		printf("msgbox->start_line = %d\n", msgbox->start_line);
	}

	return 0;
}

int STK_MsgBoxAddMsg(STK_MsgBox *msgbox, char *str)
{
	if (msgbox->end_line >= STK_MSGBOX_LINEBUF_NUM || str == NULL)
		return -1;
	
	if (str) {
		msgbox->linebuf[msgbox->end_line] = STK_TextNew(str);
		msgbox->end_line++;
	}	

	STK_WidgetEventRedraw((STK_Widget *)msgbox);

	return 0;
}







