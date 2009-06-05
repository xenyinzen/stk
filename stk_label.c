#include <stdarg.h>
#include <malloc.h>
#include <string.h>

#include "stk.h"
#include "stk_widget.h"
#include "stk_label.h"

static void stk_label_calcpatt(STK_Label *label, SDL_Rect *rect);


STK_Widget *stk_label_new()
{
	STK_Widget *widget;
	STK_Label *label = (STK_Label *)stk_malloc(sizeof(STK_Label));
	
	widget = (STK_Widget *)label;
	widget->name	= "Label";
	widget->type	= STK_WIDGET_LABEL;
	widget->rect.x	= 0;
	widget->rect.y 	= 0;
	widget->rect.w	= 50;
	widget->rect.h	= 20;
	widget->flags	= 0;
	
	label->caption	= NULL;
	
	label->height	= 10;
	label->fgcolor	= 0x00000000;
	label->bgcolor	= 0x00800080;			//TRANSPARANT;
	
	label->offset	= 0;
	label->increase	= 1;
	label->alignment = TOPLEFT;
	
	label->pattern	= LABEL_NORMAL;
	
	return (STK_Widget *)label;
}

void stk_label_draw(STK_Widget *widget, SDL_Rect *area)
{
	STK_Label *label = (STK_Label *)widget;
	SDL_Rect draw_position;
	
	printf("Enter function: stk_label_draw\n");
	
	if (label->bgcolor != TRANSPARANT) {
		SDL_Rect r;
		r.x = r.y = 0;
		r.w = widget->rect.w;
		r.h = widget->rect.h;
		printf("Label->surface: 0x%x\n", widget->surface);
		SDL_FillRect(widget->surface, &r, label->bgcolor);
	}
	
//	if (label->caption) {
//		stk_label_calcpatt(label, &draw_position);
//	}

	printf("Leave function: stk_label_draw\n");
}

int stk_label_setColor(STK_Widget *widget, int which, Uint32 color)
{
	STK_Label *label = (STK_Label *)widget;
	if (label == NULL) 
		return 0;
	switch (which) {
		case FOREGROUND:
			label->fgcolor = color;
			break;
		case BACKGROUND:
			label->bgcolor = color;
			break;
		default:
			break;
	}

	return 1;
}

static void stk_label_calcpatt(STK_Label *label, SDL_Rect *rect)
{
#if 0
	STK_Widget *widget = (STK_Widget *)label;
	SDL_Rect dims;
	
	/* Calculate the total size of the string in pixels */
//	STK_font_calcDims(label->font, label->caption, &dims);
	if (dims.w > widget->rect.w) {
		switch (label->pattern) {
		case LABEL_NORMAL:
			rect->x = 0;
			rect->y = 0;
			rect->w = widget->rect.w;
			rect->h = widget->rect.h;
			break;
		case LABEL_BOUNCE:	
			rect->x = -label->offset;
			rect->y = 0;
			rect->w = widget->rect.w + label->offset;
			rect->h = widget->rect.h;
			if (label->increase == 1)
				label->offset++;
			else
				label->offset--;
	
			if (dims.w < rect->w)
				label->increase = 0;
			if (label->offset == -1)
				label->increase = 1;
			break;	
		case LABEL_SCROLL_LEFT:
			rect->x = -label->offset;
			rect->y = 0;
			rect->w = widget->rect.w + label->offset;
			rect->h = widget->rect.h;
			label->offset++;
			if (dims.w + widget->rect.w < widget->rect.w + label->offset)
				label->offset = -widget->rect.w;
			break;
		case LABEL_SCROLL_RIGHT:
			rect->x = -label->offset;
			rect->y = 0;
			rect->w = widget->rect.w + label->offset;
			rect->h = widget->rect.h;
			label->offset--;
			
			if (label->offset + widget->rect.w < 0)
				label->offset = dims.w;
			break;
		}
	}
	else {
//		int height = label->font->height;
		switch (label->alignment) {
		case TOPLEFT:
			rect->x = 0;
			rect->y = 0 - dims.y + 1;
			rect->w = dims.w;
			rect->h = dims.h;
			break;		
		case TOPCENTER:
			rect->x = 0 + (widget->rect.w - dims.w)/2 + 1;
			rect->y = 0 - dims.y + 1;
			rect->w = dims.w;
			rect->h = dims.h;
			break;
		case CENTER:
			rect->x = 0 + (widget->rect.w - dims.w)/2;
			rect->y = 0 + (widget->rect.h - height)/2 + 1;
			rect->w = widget->rect.w;
			rect->h = widget->rect.h;
			break;
		case CENTERLEFT:
			rect->x = 0;
			rect->y = 0 + (widget->rect.h - height)/2 + 1;
			rect->w = widget->rect.w;
			rect->h = widget->rect.h;
			break;			
		}
	}
#endif
}

int stk_label_setAlignment(STK_Widget *widget, int alignment)
{
	STK_Label *label = (STK_Label *)widget;
	label->alignment = alignment;
	
	return 1;
}
