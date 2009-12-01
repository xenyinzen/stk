#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_base.h"
#include "stk_color.h"
#include "stk_image.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_progressbar.h"
#include "stk_default.h"


Uint32 STK_ProgressBarCheckValue(Uint32 interval, void *param);

STK_ProgressBar *STK_ProgressBarNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint32 *pvalue)
{
	STK_ProgressBar *pb;
	STK_Widget *widget;
	STK_Window *win;
	int height;
	
	win = STK_WindowGetTop();
	if (!win)
		return -1;
	if (x >= win->widget.rect.w || y >= win->widget.rect.h)
		return -1;	
	
	pb = (STK_ProgressBar *)STK_Malloc(sizeof(STK_ProgressBar));
	widget = (STK_Widget *)pb;
	STK_WidgetInitInstance(widget);

	widget->name = "ProgressBar";
	widget->type = STK_WIDGET_PROGRESSBAR;
	widget->flags = 0;
	widget->border = STK_PROGRESSBAR_BORDER_THICKNESS;
	widget->fixed = 1;	// not very neccesary here
	
	height = STK_FontGetHeight(STK_FontGetDefaultFontWithSize()) + 2 * widget->border;
	if (height < h)
		height = h;
	
	STK_BaseRectAssign(&widget->rect, x, y, w, height);
	STK_WidgetSetDims(widget, &widget->rect);
	
	pb->pvalue = pvalue;
	pb->value = *(pb->pvalue);
	
	pb->timer = SDL_AddTimer(100, STK_ProgressBarCheckValue, (void *)pb);
	
	return widget;

}

void STK_ProgressBarDraw(STK_Widget *widget)
{
	STK_ProgressBar *pb = (STK_ProgressBar *)widget;
	SDL_Rect rect;
	Uint32 tmpcolor;
	char str[8];
	
	// fillup background
	tmpcolor = SDL_MapRGB(widget->surface->format, widget->bgcolor.r, widget->bgcolor.g, widget->bgcolor.b);
	SDL_FillRect(widget->surface, NULL, tmpcolor);
	STK_ImageDrawFrame(widget->surface, STK_IMAGE_FRAME_CONCAVE);
	
	// fillup progress
	STK_BaseRectAssign(
		&rect, 
		widget->border, 
		widget->border, 
		(widget->rect.w - 2*widget->border) * pb->value / 100, 
		widget->rect.h - 2*widget->border);
	
	tmpcolor = SDL_MapRGB(widget->surface->format, STK_COLOR_PROGRESSBAR_CONTENT);
	SDL_FillRect(widget->surface, &rect, tmpcolor);
	
	// fillup progress text
	sprintf(str, "%d%%", pb->value);
	STK_FontAdapter(STK_FontGetDefaultFontWithSize(), &rect, str);
	rect.x = (widget->rect.w - rect.w) / 2;
	rect.y = (widget->rect.h - rect.h) / 2;
	STK_FontDraw(	STK_FontGetDefaultFontWithSize(),
			str,
			widget,
			&rect,
			&widget->fgcolor,
			&widget->bgcolor);
	
	return;
}


void STK_ProgressBarClose(STK_Widget *widget)
{
	STK_ProgressBar *pb = (STK_ProgressBar *)widget;
	
	// free radio group's surface
	if (widget->surface) {
		SDL_FreeSurface(widget->surface);
	}
	
	// remove the timer
	SDL_RemoveTimer(pb->timer);
	// free STK_RadioGroup structure
	free(pb);
	
	return 0;
}

int STK_ProgressBarRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "ProgressBar", &f );
        f->draw = STK_ProgressBarDraw;
        f->close = STK_ProgressBarClose;

        return 0;
}

Uint32 STK_ProgressBarCheckValue(Uint32 interval, void *param)
{
	STK_ProgressBar *pb = (STK_ProgressBar *)param;
	
//	printf("I am a timer, haha...\n");

	if (pb->value != *(pb->pvalue)) {
		pb->value = *(pb->pvalue);
		STK_WidgetEventRedraw((STK_Widget *)pb);	
		
//		printf("in timer, hmm...\n");
	}
	
	// for test only
	SDL_Delay(100);
	(*pb->pvalue) += 1;
	if (*pb->pvalue >= 100)
		(*pb->pvalue) = 0; 
	
	return interval;
}


