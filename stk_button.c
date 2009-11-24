#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_label.h"
#include "stk_button.h"

#define STK_BUTTON_BORDER_THICKNESS	2

STK_Button *STK_ButtonNew(char *caption, Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
	STK_Button *button;
	STK_Widget *widget;
	STK_Object *object;
	STK_Window *win;
	SDL_Rect rect;
	
	win = STK_WindowGetTop();
	if (!win)
		return -1;
	if (x >= win->widget.rect.w || y >= win->widget.rect.h)
		return -1;
	
	button = (STK_Button *)STK_Malloc(sizeof(STK_Button));
	widget = (STK_Widget *)button;
	STK_WidgetInitInstance(widget);
	
	widget->name = "Button";
	widget->type = STK_WIDGET_BUTTON;
	widget->flags |= WIDGET_FOCUSABLE;
	widget->border = STK_BUTTON_BORDER_THICKNESS;
	widget->fixed = 1;
	
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	STK_WidgetSetDims(widget, &rect);

	// create label structure
	button->label = STK_LabelNew(caption, x + STK_BUTTON_BORDER_THICKNESS, y + STK_BUTTON_BORDER_THICKNESS);

	button->state = STK_BUTTON_UP;

	return button;	
}

void STK_ButtonDraw(STK_Widget *widget)
{
	STK_Button *button = (STK_Button *)widget;
	STK_Widget *child_widget = (STK_Widget *)button->label;
	SDL_Rect rect, rect_child;
	
	// if button is able to extend
	if (!widget->fixed) {	
        	STK_BaseRectCopy(&rect, &widget->rect);
		
		STK_BaseRectCopy(&rect_child, &child_widget->rect);
		rect_child.w += 2 * widget->border;
		rect_child.h += 2 * widget->border;
        	
        	if (child_widget)
			STK_BaseRectAdapter(STK_FontGetDefaultFontWithSize(), &rect, &rect_child);
	
        	if (!STK_BaseRectEqual(&rect, &widget->rect))
	                STK_WidgetSetDims(widget, &rect);
	}

	// check status, these numbers are hard coded
	// button down
	if (widget->state == 1)
		button->state = STK_BUTTON_DOWN;
	// button up
	else if (widget->state == 2)
		button->state = STK_BUTTON_UP;
	
	// draw button base
	if (button->state == STK_BUTTON_UP) { 
		STK_ButtonFilling(widget, STK_IMAGE_FRAME_CONVEX);
	}
	else if (button->state == STK_BUTTON_DOWN) {
		STK_ButtonFilling(widget, STK_IMAGE_FRAME_CONCAVE);
	}
	
	// draw button's child widget: label
	STK_ButtonFillLabel(widget);
	
	return;
}

void STK_ButtonClose(STK_Widget *widget)
{
	STK_Button *button = (STK_Button *)widget;
	STK_Widget *child = (STK_Widget *)button->label;
	F_Widget_Close close;
	
	// free child memory: label
	close = STK_WidgetGetClose(child);
	if (close)
		close(child);
	button->label = NULL;

	// free button's own memory
        // now ready to free surface
        if (widget->surface) {
                SDL_FreeSurface(widget->surface);
        }
        // now ready to free button structure
        free(button);

	return 0;
}


void STK_ButtonFilling(STK_Button *button, Uint32 pattern)
{
	STK_Widget *widget = (STK_Widget *)button;
	SDL_Rect rect;
	Uint32 tmpcolor;

	// draw frame, in an other word: area 1~8
	STK_ImageDrawFrame(widget->surface, pattern);
		
	// area 9
	STK_BaseRectAssign(&rect, 2, 2, widget->rect.w - 4, widget->rect.h - 4);
	tmpcolor = SDL_MapRGB(widget->surface->format, widget->bgcolor.r, widget->bgcolor.g, widget->bgcolor.b);
	SDL_FillRect(widget->surface, &rect, tmpcolor);

	return;
}

void STK_ButtonFillLabel(STK_Button *button)
{
	STK_Widget *widget = (STK_Widget *)button;
	STK_Widget *child = (STK_Widget *)button->label;
	SDL_Rect rect;
	F_Widget_Draw draw;
	
	if (widget->fixed) {
		STK_BaseRectAssign(&rect, 2, 2, widget->rect.w - 4, widget->rect.h - 4);
		STK_BaseRectAdapter(&child->rect, &rect);
		STK_WidgetSetDims(child, &child->rect);
		STK_LabelSetAlignment((STK_Label *)child, STK_LABEL_CENTER);
		child->fixed = 1;
	}
	// draw label
	draw = STK_WidgetGetDraw(child);
	if (draw)
		draw(child);

}


int STK_ButtonRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "Button", &f );
        f->draw = STK_ButtonDraw;
        f->close = STK_ButtonClose;

        return 0;
}

int STK_ButtonSetText(STK_Button *button, char *str)
{
	STK_Label *label = button->label;
	// here to set label's new size (but will trigger a redraw event on label)
	STK_LabelSetText(label, str);
	
	// trigger a redraw event on button
	STK_WidgetEventRedraw((STK_Widget *)button);
	
	return 0;
}

