#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_text.h"
#include "stk_base.h"
#include "stk_image.h"
#include "stk_color.h"
#include "stk_font.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_msgbox.h"
#include "stk_default.h"

int STK_MsgBoxCalcDisplayLineWindow(STK_MsgBox *msgbox, int font_height);
int STK_MsgBoxCalcTextArea(STK_MsgBox *msgbox);


STK_MsgBox *STK_MsgBoxNew(char *str, Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
	STK_MsgBox *msgbox;
	STK_Widget *widget;
	STK_Window *win;
	SDL_Rect rect;	
	int i = 0;
	int width, height;
	
	win = STK_WindowGetTop();
	if (!win)
		return -1;
	if (x >= win->widget.rect.w || y >= win->widget.rect.h)
		return -1;
	
	msgbox = (STK_MsgBox *)STK_Malloc(sizeof(STK_MsgBox));
	widget = (STK_Widget *)msgbox;
	STK_WidgetInitInstance(widget);
	
	widget->name = "MsgBox";
	widget->type = STK_WIDGET_MSGBOX;
	widget->flags = 0;
	widget->border = STK_MSGBOX_BORDER_THICKNESS;
	STK_BaseColorAssign(&widget->bgcolor, STK_COLOR_MSGBOX_BACKGROUND);
	STK_BaseColorAssign(&widget->fgcolor, STK_COLOR_MSGBOX_FOREGROUND);
	
	width = 60;
	height = STK_FontGetHeight(STK_FontGetDefaultFontWithSize()) + 2 * widget->border;
	if (width < w)
		width = w;
	if (height < h)
		height = h;
	STK_BaseRectAssign(&rect, x, y, width, height);	
	STK_WidgetSetDims(widget, &rect);

	msgbox->start_line = 0;
	msgbox->end_line = 0;
	msgbox->interval = 4;
	msgbox->log = 1;
	STK_BaseRectAssign(	&msgbox->textarea, 
				widget->border, 
				widget->border,
				widget->rect.w - 2 * widget->border,
				widget->rect.h - 2 * widget->border );
	msgbox->cur_x = 0;
	msgbox->cur_y = 0;
	
	if (str) {
		STK_MsgBoxAddMsg(msgbox, str);
	}	

	return msgbox;
}

void STK_MsgBoxDraw(STK_Widget *widget)
{
	STK_MsgBox *msgbox = (STK_MsgBox *)widget;
	SDL_Rect rect;
	STK_Font *font;
	Uint32 tmpcolor;
	Uint32 font_height;
	int i;
		
	// fillup background
	tmpcolor = SDL_MapRGB(widget->surface->format, widget->bgcolor.r, widget->bgcolor.g, widget->bgcolor.b);
	SDL_FillRect(widget->surface, NULL, tmpcolor);
	STK_ImageDrawFrame(widget->surface, STK_IMAGE_FRAME_SINGLELINE);

	font = STK_FontGetDefaultFont(0);
	font_height = STK_FontGetHeight(font);
	STK_MsgBoxCalcDisplayLineWindow(msgbox, font_height);
	
	i = msgbox->start_line;
	while (i != msgbox->end_line) {
		rect.x = msgbox->textarea.x;
		rect.y = msgbox->textarea.y \
				+ ((i + STK_MSGBOX_LINEBUF_NUM - msgbox->start_line) \
				% STK_MSGBOX_LINEBUF_NUM) \
				* (font_height + msgbox->interval);
		rect.w = msgbox->textarea.w + widget->border - rect.x;
		rect.h = msgbox->textarea.h + widget->border - rect.y;
		// here, we must ensure that linebuf[i]->data is valid
		STK_FontDraw(	font,
				//STK_FontGetDefaultFontWithSize(),
				msgbox->linebuf[i]->data, 
				widget, 
				&rect, 
				&widget->fgcolor, 
				&widget->bgcolor );
		i++;
		i %= STK_MSGBOX_LINEBUF_NUM;
	}	
}

void STK_MsgBoxClose(STK_Widget *widget)
{
	STK_MsgBox *msgbox = (STK_MsgBox *)widget;
	int i = 0;
	
	// release the text line buffer
	for (i = 0; i< STK_MSGBOX_LINEBUF_NUM; i++) {
		if (msgbox->linebuf[i]) {
			STK_TextFree(msgbox->linebuf[i]);
		}
	
	}

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




// now, we only use linear array to store output message, not circle array
int STK_MsgBoxCalcDisplayLineWindow(STK_MsgBox *msgbox, int font_height)
{
	int sum;
	int d = msgbox->end_line - msgbox->start_line;
	
	if (d <= 0)
		return -1;
	
	sum = font_height*d + msgbox->interval*(d - 1);
	
	while (sum >= msgbox->textarea.h) {
		sum -= (font_height + msgbox->interval);
		// move the top line up
		msgbox->start_line++;
		msgbox->start_line %= STK_MSGBOX_LINEBUF_NUM; 
	}

	return 0;
}

int STK_MsgBoxAddMsg(STK_MsgBox *msgbox, char *str)
{
	int i;
	
	if (msgbox->end_line >= STK_MSGBOX_LINEBUF_NUM || str == NULL)
		return -1;
	
	if (str) {
		char *psrc;
		char *p, *saveptr;
		char **p1, **p0;
		int i, n;
		
		n = 0;
		psrc = (char *)STK_Malloc(strlen(str) + 1);
		strcpy(psrc, str);
		for (i=0; i<strlen(psrc); i++) {
			if (psrc[i] == '\n')
				n++; 
		}
		
		if (n == 0) {
			msgbox->linebuf[msgbox->end_line] = STK_TextNew(str);
			STK_MsgBoxLog(msgbox);
			msgbox->end_line++;
			if (msgbox->end_line >= STK_MSGBOX_LINEBUF_NUM)
				msgbox->end_line %= STK_MSGBOX_LINEBUF_NUM;
			
		}
		// multiple line
		else if (n > 0) {
			int t = 0;
			p0 = (char **)STK_Malloc(sizeof(char *) * (n+1));

			p1 = p0;
			p = strtok_r(psrc, "\n", &saveptr);
			while (p) {
				*p1++ = p;
				t++;
				p = strtok_r(NULL, "\n", &saveptr);
			}
			
			p1 = p0;
			for (i=0; i<t; i++) {
				msgbox->linebuf[msgbox->end_line] = STK_TextNew(p1[i]);
				STK_MsgBoxLog(msgbox);
				msgbox->end_line++;
				if (msgbox->end_line >= STK_MSGBOX_LINEBUF_NUM)
					msgbox->end_line %= STK_MSGBOX_LINEBUF_NUM;
			}			
		}
		
		
	}	

	STK_WidgetEventRedraw((STK_Widget *)msgbox);

	return 0;
}

int STK_MsgBoxLog(STK_MsgBox *msgbox)
{
	FILE *fp;
	
	if (msgbox->log) {
		// if log flag swith on, we need to record the buffer content to log file
		if ((fp = fopen("log.txt", "a")) == NULL) {
			printf("Can't open log file: log.txt");
			exit(-1);
		}
		
		fprintf(fp, "%s\n", msgbox->linebuf[msgbox->end_line]->data);
		
		fclose(fp);
		SDL_Delay(10);
	}

	return 0;
}

