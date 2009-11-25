#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_base.h"
#include "stk_widget.h"
#include "stk_progressbar.h"

#define STK_PROGRESSBAR_BORDER_THICKNESS	2

Uint32 STK_ProgressBarCheckValue(Uint32 interval, void *param);

STK_Widget *STK_ProgressBarNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint32 *pvalue)
{
	STK_ProgressBar *pb;
	STK_Widget *widget;
	
	pb = (STK_ProgressBar *)STK_Malloc(sizeof(STK_ProgressBar));
	
	widget = (STK_Widget *)pb;
	widget->type = STK_WIDGET_PROGRESSBAR;
	widget->name = "ProgressBar";
	widget->flags = 0;
	
	widget->rect.x = x;
	widget->rect.y = y;
	widget->rect.w = w;
	widget->rect.h = h;
	
	pb->border = STK_PROGRESSBAR_BORDER_THICKNESS;
	pb->pvalue = pvalue;
	pb->value = *(pb->pvalue);
	
	
	pb->timer = SDL_AddTimer(100, STK_ProgressBarCheckValue, (void *)pb);
	
	return widget;

}

void STK_ProgressBarDraw(STK_Widget *widget)
{
	STK_ProgressBar *pb = (STK_ProgressBar *)widget;
	SDL_Rect rect;
	
	rect.x = 0;
	rect.y = 0;
	rect.w = widget->rect.w;
	rect.h = widget->rect.h;
	
	// fillup background
	SDL_FillRect(widget->surface, &rect, 0x00f0f0f0);
	
	rect.x = pb->border;
	rect.y = pb->border;
	rect.w = (widget->rect.w - 2*pb->border) * pb->value / 100;
	rect.h = widget->rect.h - 2*pb->border;
	
	// fillup progress
	SDL_FillRect(widget->surface, &rect, 0x00003030);
	
	// fillup progress text
	// TODO later	

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


