#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_base.h"
#include "stk_color.h"
#include "stk_label.h"
#include "stk_radiobutton.h"
#include "stk_default.h"


STK_RadioButton *STK_RadioButtonNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, char *caption, int *pstate)
{
	STK_RadioButton *rb;
	STK_Widget *widget;
	STK_Object *object;
	STK_Window *win;
	SDL_Rect rect;
	
	win = STK_WindowGetTop();
	if (!win)
		return NULL;
	if (x >= win->widget.rect.w || y >= win->widget.rect.h)
		return NULL;
	
	rb = (STK_RadioButton *)STK_Malloc(sizeof(STK_RadioButton));
	widget = (STK_Widget *)rb;
	STK_WidgetInitInstance(widget);

	widget->name = "RadioButton";
	widget->type = STK_WIDGET_RADIOBUTTON;
	widget->flags |= WIDGET_FOCUSABLE;
	widget->border = STK_RADIOBUTTON_BORDER_THICKNESS;
	widget->fixed = 0;
	
	STK_BaseRectAssign(&rect, x, y, w, h);
	STK_WidgetSetDims(widget, &rect);
	
	rb->state = *pstate;
	rb->interval = 2*widget->border;
	rb->header_size = 20;	// hard coded	

	// create label structure
	rb->label = STK_LabelNew(	x + widget->border + rb->header_size + rb->interval,
					y + widget->border,
					0,
					0,
					caption, 0);
	if (!widget->fixed)
		STK_RadioButtonAdapterToChild(rb);

	return rb;	
}

void STK_RadioButtonDraw(STK_Widget *widget)
{
	STK_RadioButton *rb = (STK_RadioButton *)widget;
	
	if (!widget->fixed) 
		STK_RadioButtonAdapterToChild(rb);
	
	// convert state	
	if (widget->state == 1) {
		rb->state 
			= (rb->state == STK_RADIOBUTTON_RELEASE? STK_RADIOBUTTON_TOGGLE: STK_RADIOBUTTON_RELEASE);
	}
	
	// draw radio button base
	if (rb->state == STK_RADIOBUTTON_RELEASE) { 
		STK_RadioButtonFilling(widget, STK_IMAGE_BOX_HOLLOW);
	}
	else if (rb->state == STK_RADIOBUTTON_TOGGLE) {
		STK_RadioButtonFilling(widget, STK_IMAGE_BOX_SOLID);
	}

	// draw radio button's child widget: label
	STK_RadioButtonFillLabel(widget);

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


void STK_RadioButtonFilling(STK_RadioButton *rb, Uint32 pattern)
{
	STK_Widget *widget = (STK_Widget *)rb;
	SDL_Rect rect;
	Uint32 tmpcolor;
		
	// area 1: header part area, fill picture 
	STK_BaseRectAssign(&rect, widget->border, widget->border, rb->header_size, rb->header_size);
//	rb->image.fillstyle = STK_IMAGE_FILLSTYLE_MATRIX;
	STK_ImageFillRect(widget->surface, &rect, STK_IMAGE_KIND_BOX, pattern, &rb->image, 0);

	// clear this state, very important
	widget->state = 0;	
/*	// area 2: label background area, fill mono color
	STK_BaseRectAssign(
		&rect, 
		widget->border + rb->header_size + rb->interval, 
		widget->border, 
		widget->rect.w - 2 * widget->border - rb->header_size - rb->interval, 
		widget->rect.h - 2 * widget->border);
	tmpcolor = SDL_MapRGB(
		widget->surface->format,
		widget->bgcolor.r,
		widget->bgcolor.g,
		widget->bgcolor.b );
	SDL_FillRect(widget->surface, &rect, tmpcolor);
*/
	return widget;
}

void STK_RadioButtonFillLabel(STK_RadioButton *rb)
{
	STK_Widget *widget = (STK_Widget *)rb;
	STK_Widget *child = (STK_Widget *)rb->label;
	SDL_Rect rect;
	F_Widget_Draw draw;
	
	if (widget->fixed) {
		STK_BaseRectAssign(
			&rect,
			widget->border + rb->header_size + rb->interval,
			widget->border,
			widget->rect.w - 2 * widget->border - rb->header_size - rb->interval,
			widget->rect.h - 2 * widget->border );
			
		STK_BaseRectAdapter(&child->rect, &rect);
		STK_WidgetSetDims(child, &child->rect);
		child->fixed = 1;	
	}
	
	// draw label first
	draw = STK_WidgetGetDraw(child);
	if (draw)
		draw(child);
	
}

int STK_RadioButtonRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "RadioButton", &f );
        f->draw = STK_RadioButtonDraw;
        f->close = STK_RadioButtonClose;

        return 0;
}

int STK_RadioButtonSetText(STK_RadioButton *rb, char *str)
{
	
	STK_Widget *widget = (STK_Widget *)rb;
	STK_Widget *label = rb->label;
	// here to set label's new size (but will trigger a redraw event on label)
	STK_LabelSetText(label, str);
	if (!widget->fixed)
		STK_RadioButtonAdapterToChild(rb);
	
	// trigger a redraw event on radiobutton
	STK_WidgetEventRedraw(widget);
	return 0;
}

int STK_RadioButtonAdapterToChild(STK_RadioButton *rb)
{		
	STK_Widget *widget = (STK_Widget *)rb;
	STK_Widget *child_widget = (STK_Widget *)rb->label;
	SDL_Rect rect, rect_child;
	
	STK_BaseRectCopy(&rect, &widget->rect);
	STK_BaseRectCopy(&rect_child, &child_widget->rect);
	rect_child.w += 2 * widget->border + rb->interval + rb->header_size;
	rect_child.h += 2 * widget->border;
	
	STK_BaseRectAdapter(&rect, &rect_child);
	
	if (!STK_BaseRectEqual(&rect, &widget->rect))
		STK_WidgetSetDims(widget, &rect);
		
	return 0;
}
