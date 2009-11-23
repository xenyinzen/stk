#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_label.h"
#include "stk_button.h"

#define STK_BUTTON_BORDER_THICKNESS	2

STK_Widget *STK_ButtonNew(char *caption, Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
	STK_Button *button;
	STK_Widget *widget;
	STK_Object *object;
	
	button = (STK_Button *)STK_Malloc(sizeof(STK_Button));

	widget = (STK_Widget *)button;
	widget->type = STK_WIDGET_BUTTON;
	widget->name = "Button";
	widget->flags |= WIDGET_FOCUSABLE;
	
	widget->rect.x = x;
	widget->rect.y = y;
	widget->rect.w = w;
	widget->rect.h = h;

	printf("Enter STK_ButtonNew\n");
	// create label structure
	button->label = STK_LabelNew(caption, x + STK_BUTTON_BORDER_THICKNESS, y + STK_BUTTON_BORDER_THICKNESS);

	button->state = STK_BUTTON_UP;
	button->image.fillstyle = STK_IMAGESTYLE_NORMAL;

//	STK_WidgetEventShow(widget);	
	//STK_WidgetEventRedraw(widget);

	printf("Exit STK_ButtonNew\n");
	return widget;	
}

void STK_ButtonDraw(STK_Widget *widget)
{
	STK_Button *button = (STK_Button *)widget;
	STK_Widget *child_widget = (STK_Widget *)button->label;
	SDL_Rect rect;
	
	printf("Enter STK_ButtonDraw\n");	
	
        rect.x = widget->rect.x;
        rect.y = widget->rect.y;
        rect.w = widget->rect.w;
        rect.h = widget->rect.h;

        if (child_widget) {
		STK_ChildAdapter(&widget->rect, &child_widget->rect);
	}
	
        // if area has changed, need to reset dimensions of this label: free previous surface and alloc a new one
        if (rect.x != widget->rect.x
                || rect.y != widget->rect.y
                || rect.w != widget->rect.w
                || rect.h != widget->rect.h)
                STK_WidgetSetDims(widget, &widget->rect);
	
	if (widget->state == 1)
		button->state = STK_BUTTON_DOWN;
	else if (widget->state == 2)
		button->state = STK_BUTTON_UP;
	
	// draw button base
	if (button->state == STK_BUTTON_UP) { 
		STK_ButtonFilling(widget, 0);
	}
	else if (button->state == STK_BUTTON_DOWN) {
		STK_ButtonFilling(widget, 1);
	}
	
	printf("Exit STK_ButtonDraw\n");	

	// draw button's child widget: label
	STK_ButtonFillLabel(widget);
	
}

void STK_ButtonFillLabel(STK_Widget *widget)
{
	STK_Button *button = (STK_Button *)widget;
	STK_Widget *child = (STK_Widget *)button->label;
	SDL_Rect rect;
	F_Widget_Draw draw;
	
	// draw label first
	draw = STK_WidgetGetDraw(child);
	if (draw)
		draw(child);
	
	rect.x = STK_BUTTON_BORDER_THICKNESS;
	rect.y = STK_BUTTON_BORDER_THICKNESS;
	rect.w = widget->rect.w - 2*STK_BUTTON_BORDER_THICKNESS;
	rect.h = widget->rect.h - 2*STK_BUTTON_BORDER_THICKNESS;

	// blit label's surface to button's surface
	SDL_BlitSurface(child->surface, NULL, widget->surface, &rect);

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


STK_Widget *STK_ButtonFilling(STK_Widget *widget, Uint32 pattern)
{
	STK_Button *button = (STK_Button *)widget;
	SDL_Rect rect;
	printf("Enter STK_ButtonFilling");
		
	// area 9
	rect.x = STK_BUTTON_BORDER_THICKNESS;
	rect.y = STK_BUTTON_BORDER_THICKNESS;
	rect.w = widget->rect.w - 2*STK_BUTTON_BORDER_THICKNESS;
	rect.h = widget->rect.h - 2*STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_NORMAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 0);
	
	// area 1
	rect.x = STK_BUTTON_BORDER_THICKNESS;
	rect.y = 0;
	rect.w = widget->rect.w - 2*STK_BUTTON_BORDER_THICKNESS;
	rect.h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_HORIZONTAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 0);
	
	// area 3
	rect.x = STK_BUTTON_BORDER_THICKNESS;
	rect.y = widget->rect.h - STK_BUTTON_BORDER_THICKNESS;
	rect.w = widget->rect.w - 2*STK_BUTTON_BORDER_THICKNESS;
	rect.h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_HORIZONTAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 1);
	
	// area 2
	rect.x = widget->rect.w - STK_BUTTON_BORDER_THICKNESS;
	rect.y = STK_BUTTON_BORDER_THICKNESS;
	rect.w = STK_BUTTON_BORDER_THICKNESS;
	rect.h = widget->rect.h - 2*STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_VERTICAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 0);
	
	// area 4
	rect.x = 0;
	rect.y = STK_BUTTON_BORDER_THICKNESS;
	rect.w = STK_BUTTON_BORDER_THICKNESS;
	rect.h = widget->rect.h - 2*STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_VERTICAL;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 1);
	
	// area 5
	rect.x = 0;
	rect.y = 0;
	rect.w = STK_BUTTON_BORDER_THICKNESS;
	rect.h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 0);
	
	// area 6
	rect.x = widget->rect.w - STK_BUTTON_BORDER_THICKNESS;
	rect.y = 0;
	rect.w = STK_BUTTON_BORDER_THICKNESS;
	rect.h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 1);
	
	// area 7
	rect.x = widget->rect.w - STK_BUTTON_BORDER_THICKNESS;
	rect.y = widget->rect.h - STK_BUTTON_BORDER_THICKNESS;
	rect.w = STK_BUTTON_BORDER_THICKNESS;
	rect.h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 2);
	
	// area 8
	rect.x = 0;
	rect.y = widget->rect.h - STK_BUTTON_BORDER_THICKNESS;
	rect.w = STK_BUTTON_BORDER_THICKNESS;
	rect.h = STK_BUTTON_BORDER_THICKNESS;
	button->image.fillstyle = STK_IMAGESTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &button->image, &rect, pattern, 3);

	printf("Exit STK_ButtonFilling");

}

int STK_ButtonRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "Button", &f );
        f->draw = STK_ButtonDraw;
        f->close = STK_ButtonClose;

        return 0;
}

int STK_ChildAdapter(SDL_Rect *r0, SDL_Rect *r1)
{
	int width = r1->w + 2*STK_BUTTON_BORDER_THICKNESS;
	int height = r1->h + 2*STK_BUTTON_BORDER_THICKNESS;
	printf("Enter STK_ChildAdapter\n");
	if (r0->w != width || r0->h != height) {
		r0->w = width;
		r0->h = height;
	}

	printf("Exit STK_ChildAdapter\n");
	return 0;
}

int STK_ButtonSetText(STK_Widget *widget, char *str)
{
	STK_Button *button = (STK_Button *)widget;
	STK_Widget *label = button->label;
	// here to set label's new size (but will trigger a redraw event on label)
	STK_LabelSetText(label, str);
	
	// trigger a redraw event on button
	STK_WidgetEventRedraw(widget);
	return 0;
}

// not very needed
int STK_ButtonSetSize(STK_Widget *widget, Uint16 width, Uint16 height)
{
	STK_Button *button = (STK_Button *)widget;
	STK_Widget *label = button->label;
	
	

	

}





  