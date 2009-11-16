#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_label.h"
#include "stk_radiobutton.h"

#define STK_RADIOBUTTON_BORDER_THICKNESS	2

STK_Widget *STK_RadioButtonNew(char *caption, Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
	STK_RadioButton *radiobutton;
	STK_Widget *widget;
	STK_Object *object;
	
	radiobutton = (STK_RadioButton *)STK_Malloc(sizeof(STK_RadioButton));

	widget = (STK_Widget *)radiobutton;
	widget->type = STK_WIDGET_RADIOBUTTON;
	widget->name = "RadioButton";
	widget->flags |= WIDGET_FOCUSABLE;
	
	widget->rect.x = x;
	widget->rect.y = y;
	widget->rect.w = w;
	widget->rect.h = h;

	radiobutton->state = STK_RADIOBUTTON_RELEASE;
	radiobutton->image.fillstyle = STK_IMAGESTYLE_NORMAL;
	radiobutton->border = STK_RADIOBUTTON_BORDER_THICKNESS;
	radiobutton->interval = 2*STK_RADIOBUTTON_BORDER_THICKNESS;
	radiobutton->header_height = 20;	// temporarily	

	printf("Enter STK_RadioButtonNew\n");
	// create label structure
	radiobutton->label = STK_LabelNew(
				caption, 
				x + radiobutton->border + radiobutton->header_height + radiobutton->interval,
				y + radiobutton->border);

	printf("Exit STK_RadioButtonNew\n");
	return widget;	
}

void STK_RadioButtonDraw(STK_Widget *widget)
{
	STK_RadioButton *radiobutton = (STK_RadioButton *)widget;
	STK_Widget *child_widget = (STK_Widget *)radiobutton->label;
	SDL_Rect rect;
	
	printf("Enter STK_ButtonDraw\n");	
	
        rect.x = widget->rect.x;
        rect.y = widget->rect.y;
        rect.w = widget->rect.w;
        rect.h = widget->rect.h;
	
	// before drawing, we need to do some computation.
        if (child_widget) {
		STK_ChildAdapter(&widget->rect, &child_widget->rect);
	}
	
        // if area has changed, need to reset dimensions of this label: free previous surface and alloc a new one
        if (rect.x != widget->rect.x
                || rect.y != widget->rect.y
                || rect.w != widget->rect.w
                || rect.h != widget->rect.h)
                STK_WidgetSetDims(widget, widget->rect.x, widget->rect.y, widget->rect.w, widget->rect.h);
	
	
	if (widget->state == 1) {
		radiobutton->state 
			= (radiobutton->state == STK_RADIOBUTTON_RELEASE? STK_RADIOBUTTON_TOGGLE: STK_RADIOBUTTON_RELEASE);
	}
	
	// draw radio button base
	if (radiobutton->state == STK_RADIOBUTTON_RELEASE) { 
		STK_RadioButtonFilling(widget, 0);
	}
	else if (radiobutton->state == STK_RADIOBUTTON_TOGGLE) {
		STK_RadioButtonFilling(widget, 1);
	}

	// draw radio button's child widget: label
	STK_RadioButtonFillLabel(widget);

	printf("Exit STK_RadioButtonDraw\n");	
	
}

void STK_RadioButtonFillLabel(STK_Widget *widget)
{
	STK_RadioButton *radiobutton = (STK_RadioButton *)widget;
	STK_Widget *child = (STK_Widget *)radiobutton->label;
	SDL_Rect rect;
	F_Widget_Draw draw;
	
	// draw label first
	draw = STK_WidgetGetDraw(child);
	if (draw)
		draw(child);
	
	rect.x = radiobutton->border + radiobutton->header_height + radiobutton->interval;
	rect.y = radiobutton->border;
	rect.w = widget->rect.w - rect.x - radiobutton->border;
	rect.h = widget->rect.h - 2 * radiobutton->border;

	// blit label's surface to radio button's surface
	SDL_BlitSurface(child->surface, NULL, widget->surface, &rect);

}

void STK_RadioButtonClose(STK_Widget *widget)
{
	STK_RadioButton *radiobutton = (STK_RadioButton *)widget;
	STK_Widget *child = (STK_Widget *)radiobutton->label;
	F_Widget_Close close;
	
	// free child memory: label
	close = STK_WidgetGetClose(child);
	if (close)
		close(child);
	radiobutton->label = NULL;

	// free radio button's own memory
        // now ready to free surface
        if (widget->surface) {
                SDL_FreeSurface(widget->surface);
        }
        // now ready to free radio button structure
        free(radiobutton);

	return 0;
}


STK_Widget *STK_RadioButtonFilling(STK_Widget *widget, Uint32 pattern)
{
	STK_RadioButton *radiobutton = (STK_RadioButton *)widget;
	SDL_Rect rect;
	printf("Enter STK_RadioButtonFilling");
		
	// area 1: header part area, fill picture 
	rect.x = radiobutton->border;
	rect.y = radiobutton->border;
	rect.w = radiobutton->header_height;
	rect.h = radiobutton->header_height;
	radiobutton->image.fillstyle = STK_IMAGESTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &radiobutton->image, &rect, pattern, 10);
	
	// area 2: label background area, fill mono color
	rect.x = radiobutton->border + radiobutton->header_height + radiobutton->interval;
	rect.y = radiobutton->border;
	rect.w = widget->rect.w - rect.x - radiobutton->border;
	rect.h = widget->rect.h - 2 * radiobutton->border;
	radiobutton->image.fillstyle = STK_IMAGESTYLE_NORMAL;
	STK_ImageFillRect(widget->surface, &radiobutton->image, &rect, pattern, 10);
	
	printf("Exit STK_RadioButtonFilling");
	return widget;
}

int STK_RadioButtonRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "RadioButton", &f );
        f->draw = STK_RadioButtonDraw;
        f->close = STK_RadioButtonClose;

        return 0;
}

int STK_RadioButtonSetText(STK_Widget *widget, char *str)
{
	STK_RadioButton *radiobutton = (STK_RadioButton *)widget;
	STK_Widget *label = radiobutton->label;
	// here to set label's new size (but will trigger a redraw event on label)
	STK_LabelSetText(label, str);
	
	// trigger a redraw event on radiobutton
	STK_WidgetEventRedraw(widget);
	return 0;
}

