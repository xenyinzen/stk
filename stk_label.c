#include <stdarg.h>
#include <malloc.h>
#include <string.h>

#include "stk.h"
#include "stk_widget.h"
#include "stk_label.h"

static void stk_label_calculatePattern(STK_Label *label, SDL_Rect *rect);


STK_Widget *stk_label_new( char *str, Uint16 x, Uint16 y )
{
	STK_Widget *widget;
	STK_Label *label = (STK_Label *)stk_malloc(sizeof(STK_Label));
	
	// here, need to check x and y's reasonablition
	
	widget = (STK_Widget *)label;
	widget->name	= "Label";
	widget->type	= STK_WIDGET_LABEL;
	widget->rect.x	= x;
	widget->rect.y 	= y;
	widget->rect.w	= 60;
	widget->rect.h	= 30;
	widget->flags	= 0;
	
	if (str) {
		label->caption = (char *)stk_malloc(strlen(str) + 1);
		// after this copy, the last char left in caption is 0
		strcpy(label->caption, str);
	}
	else {
		label->caption = NULL;
	}
	
	label->height	= 20;
	label->fgcolor	= 0x00000000;
	label->bgcolor	= 0x00d4d4d4;			//TRANSPARANT;
	
	label->offset	= 0;
	label->increase	= 1;
	label->alignment = TOPLEFT;
	
	label->pattern	= LABEL_NORMAL;
	
	stk_widget_EventShow((STK_Widget *)label);
	
	return (STK_Widget *)label;
}

int stk_label_registerType()
{
	STK_WidgetFuncs *f;
	stk_widget_registerType( "Label", &f );
	f->draw = stk_label_draw;
	f->close = stk_label_close;
	
	return 1;
}


void stk_label_draw(STK_Widget *widget, SDL_Rect *area)
{
	SDL_Rect rect = {0};
	STK_Label *label = (STK_Label *)widget;
	//SDL_Rect draw_position;
	SDL_Color fg = {0};
	SDL_Color bg = {0};
	
	
	stk_font_adapter(&widget->rect, label->caption);
	
	if (label->bgcolor != TRANSPARANT) {
		SDL_Rect r;
		r.x = r.y = 0;
		r.w = widget->rect.w;
		r.h = widget->rect.h;
		SDL_FillRect(widget->surface, &r, label->bgcolor);
	}
	
	if (label->caption) {
		stk_label_calculatePattern(label, &rect);
		rect.x = rect.x + widget->rect.x;
		rect.y = rect.y + widget->rect.y;
		
		fg.r = (Uint8)((label->fgcolor >> 16) & 0xff);
		fg.g = (Uint8)((label->fgcolor >> 8) & 0xff);
		fg.b = (Uint8)((label->fgcolor >> 0) & 0xff);
		
		bg.r = (Uint8)((label->bgcolor >> 16) & 0xff);
		bg.g = (Uint8)((label->bgcolor >> 8) & 0xff);
		bg.b = (Uint8)((label->bgcolor >> 0) & 0xff);

		stk_font_draw( &rect, label->caption, &fg, &bg );
	}

}

int stk_label_close(STK_Widget *widget)
{
	STK_Label *label = (STK_Label *)widget;
	
	// now ready to free surface
	if (widget->surface) {
		SDL_FreeSurface(widget->surface);
	}
	// now ready to free string
	if (label->caption) {
		free(label->caption);
		label->caption = NULL;
	}
	// now ready to free label node
	free(label);
	return 1;
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

static void stk_label_calculatePattern(STK_Label *label, SDL_Rect *rect)
{
	STK_Widget *widget = (STK_Widget *)label;
	SDL_Rect dims = {0};
	
	/* Calculate the total size of the string in pixels */
	stk_font_adapter(&dims, label->caption);
	switch (label->alignment) {
	case TOPLEFT:
		rect->x = 0;
		rect->y = 0;
		rect->w = dims.w;
		rect->h = dims.h;
		break;		
	case TOPCENTER:
		rect->x = 0 + (widget->rect.w - dims.w)/2 + 1;
		rect->y = 0;
		rect->w = dims.w;
		rect->h = dims.h;
		break;
	case CENTER:
		rect->x = 0 + (widget->rect.w - dims.w)/2;
		rect->y = 0 + (widget->rect.h - dims.h)/2 + 1;
		rect->w = dims.w;
		rect->h = dims.h;
		break;
	case CENTERLEFT:
		rect->x = 0;
		rect->y = 0 + (widget->rect.h - dims.h)/2 + 1;
		rect->w = dims.w;
		rect->h = dims.h;
		break;			
	}
}

int stk_label_setAlignment(STK_Widget *widget, int alignment)
{
	STK_Label *label = (STK_Label *)widget;
	label->alignment = alignment;
	
	return 1;
}

int stk_label_setText(STK_Widget *widget, char * text)
{
	STK_Label *label = (STK_Label *)widget;
	
	if (text == NULL)
		return 0;
	
	if (label->caption) {
		free(label->caption);
		label->caption = NULL;
	}
	// because the c string have a zero char in the end, malloc text size + 1 
	label->caption = (char *)stk_malloc(strlen(text) + 1);
	// after this copy, the last char left in caption is 0
	strcpy(label->caption, text);
	
	stk_widget_EventRedraw(widget);
			
	return 1;
}

char *stk_label_getText(STK_Widget *widget)
{
	STK_Label *label = (STK_Label *)widget;
		
	return label->caption;
}

