#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_button.h"
#include "stk_label.h"

#define STK_BUTTON_BORDER_THICK	2

STK_Widget *STK_ButtonFilling(STK_Widget *widget, Uint32 pattern);


STK_Widget *STK_ButtonNew(char *caption, Uint16 w, Uint16 h)
{
	STK_Button *button;
	STK_Widget *widget;
	STK_Object *object;
	
	button = (STK_Button *)STK_Malloc(sizeof(STK_Button));

	widget = (STK_Widget *)button;
	widget->type = STK_WIDGET_BUTTON;
	widget->name = "Button";
	widget->flags = 0;
	
	button->label = STK_LabelNew(caption, w - 2*STK_BUTTON_BORDER_THICK, h - 2*STK_BUTTON_BORDER_THICK);


//	object = (STK_Object *)widget;
	
	button->state = STK_BUTTON_UP;
	button->fillstyle = STK_IMAGESTYLE_NORMAL;
	
	return (STK_Widget *)widget;	
}

void STK_ButtonDraw(STK_Widget *widget)
{
	STK_Button *button = (STK_Button *)widget;
	
	// draw button base
	if (button->state & STK_BUTTON_UP) { 
		STK_ButtonFilling(widget, 0);
	}
	else if (button->state & STK_BUTTON_DOWN) {
		STK_ButtonFilling(widget, 1);
	}
	
	// draw button's child widget: label
	STK_ButtonFillLabel(widget);
	
}

void STK_ButtonFillLabel(STK_Widget *widget)
{
	STK_Button *button = (STK_Button *)widget;
	SDL_Rect rect;
	STK_Widget *child = (STK_Widget *)widget->label;

	rect->x = STK_BUTTON_BORDER_THICKNESS;
	rect->y = STK_BUTTON_BORDER_THICKNESS;
	rect->w = widget->rect.w - 2*STK_BUTTON_BORDER_THICKNESS;
	rect->h = widget->rect.h - 2*STK_BUTTON_BORDER_THICKNESS;

	SDL_BlitSurface(child->surface, NULL, widget->surface, &rect);

}

STK_Widget *STK_ButtonFilling(STK_Widget *widget, Uint32 pattern)
{
	STK_Button *button = (STK_Button *)widget;
	SDL_Rect rect;
	
	// area 9
	rect->x = STK_BUTTON_BORDER_THICKNESS;
	rect->y = STK_BUTTON_BORDER_THICKNESS;
	rect->w = widget->rect.w - 2*STK_BUTTON_BORDER_THICKNESS;
	rect->h = widget->rect.h - 2*STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_NORMAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 0);
	
	// area 1
	rect->x = STK_BUTTON_BORDER_THICKNESS;
	rect->y = 0;
	rect->w = widget->rect.w - 2*STK_BUTTON_BORDER_THICKNESS;
	rect->h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_HORIZONTAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 0);
	
	// area 3
	rect->x = STK_BUTTON_BORDER_THICKNESS;
	rect->y = widget->rect.h - STK_BUTTON_BORDER_THICKNESS;
	rect->w = widget->rect.w - 2*STK_BUTTON_BORDER_THICKNESS;
	rect->h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_HORIZONTAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 1);
	
	// area 2
	rect->x = widget->rect.w - STK_BUTTON_BORDER_THICKNESS;
	rect->y = STK_BUTTON_BORDER_THICKNESS;
	rect->w = STK_BUTTON_BORDER_THICKNESS;
	rect->h = widget->rect.h - 2*STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_VERTICAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 0);
	
	// area 4
	rect->x = 0;
	rect->y = STK_BUTTON_BORDER_THICKNESS;
	rect->w = STK_BUTTON_BORDER_THICKNESS;
	rect->h = widget->rect.h - 2*STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_VERTICAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 1);
	
	// area 5
	rect->x = 0;
	rect->y = 0;
	rect->w = STK_BUTTON_BORDER_THICKNESS;
	rect->h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 0);
	
	// area 6
	rect->x = widget->rect.w - STK_BUTTON_BORDER_THICKNESS;
	rect->y = 0;
	rect->w = STK_BUTTON_BORDER_THICKNESS;
	rect->h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 1);
	
	// area 7
	rect->x = widget->rect.w - STK_BUTTON_BORDER_THICKNESS;
	rect->y = widget->rect.h - STK_BUTTON_BORDER_THICKNESS;
	rect->w = STK_BUTTON_BORDER_THICKNESS;
	rect->h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 2);
	
	// area 8
	rect->x = 0;
	rect->y = widget->rect.h - STK_BUTTON_BORDER_THICKNESS;
	rect->w = STK_BUTTON_BORDER_THICKNESS;
	rect->h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 3);

}

void STK_ButtonClose(STK_Widget *widget)
{
	STK_Widget *child = (STK_Widget *)widget->label;
	// free child memory
	
	// free button's own memory
	
	
}

int STK_ButtonRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "Button", &f );
        f->draw = STK_ButtonDraw;
        f->close = STK_ButtonClose;

        return 1;
}

