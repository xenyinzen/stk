#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk_mm.h"
#include "stk_base.h"
#include "stk_color.h"
#include "stk_image.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_slidebar.h"

#define STK_SLIDEBAR_BORDER_THICKNESS 2

// Getting functions
static void STK_SlidebarValueToPixel(STK_Slidebar *slider);
static void STK_SlidebarPixelToValue(STK_Slidebar *slider);

// Do step action 
static void STK_SlidebarStep(STK_Slidebar *slider, int dir, int step);

// Callback functions
static void STK_SlidebarEventMouseButtonDown(STK_Object *object, void *signaldata, void *userdata);
static void STK_SlidebarEventMouseButtonUp(STK_Object *object, void *signaldata, void *userdata);
static void STK_SlidebarEventMouseMotion(STK_Object *object, void *signaldata, void *userdata);


// New function define
STK_Slidebar *STK_SlidebarNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
	STK_Object *object;
	STK_Widget *widget;
	STK_Slidebar *slider;
	
	STK_Window *win;
	SDL_Rect rect;
	
	win = STK_WindowGetTop();
	if (!win)
		return -1;
	if (x >= win->widget.rect.w || y >= win->widget.rect.h)
		return -1;
	
	slider = (STK_Slidebar *)STK_Malloc(sizeof(STK_Slidebar));
	widget = (STK_Widget *)slider;
	object = (STK_Object *)slider;
	STK_WidgetInitInstance(widget);
	
	widget->name = "Slidebar";
	widget->type = STK_WIDGET_SLIDEBAR;
	widget->flags |= WIDGET_FOCUSABLE+WIDGET_DRAGABLE;
	widget->fixed = 1;
	widget->border = STK_SLIDEBAR_BORDER_THICKNESS;
	STK_BaseColorAssign(&widget->bgcolor, STK_COLOR_SLIDEBAR_BACKGROUND);
	STK_BaseColorAssign(&widget->fgcolor, STK_COLOR_SLIDEBAR_FOREGROUND);
	
	STK_BaseRectAssign(&rect, x, y, w, h);
	STK_WidgetSetDims(widget, &rect);
	
	// connect three mouse events
	STK_SignalConnect(widget, "mousebuttondown", STK_SlidebarEventMouseButtonDown, widget);
	STK_SignalConnect(widget, "mousebuttonup", STK_SlidebarEventMouseButtonUp, widget);
	STK_SignalConnect(widget, "mousemotion", STK_SlidebarEventMouseMotion, widget);
	
	slider->minvalue = 0;
	slider->maxvalue = 100;
	slider->curvalue = 0;
	slider->value_locked = 0;
	
	if (w > h)
		slider->orientation = STK_SLIDEBAR_HORIZONTAL;
	else
		slider->orientation = STK_SLIDEBAR_VERTICAL;
	
	// internal
	slider->minpixel = widget->border;
	slider->curpixel = slider->minpixel;
	
	// the interval between bar border and button border
	if (slider->orientation == STK_SLIDEBAR_VERTICAL) {
		slider->b_width = widget->rect.w - 2 * widget->border;
		slider->b_height = slider->b_width * 3 / 4;
		slider->maxpixel = widget->rect.h - slider->b_height - widget->border;
	}
	else {
		slider->b_height = widget->rect.h - 2 * widget->border;
		slider->b_width = slider->b_height * 3 / 4;
		slider->maxpixel = widget->rect.w - slider->b_width - widget->border;
	}
	
	slider->step = STK_SLIDEBAR_STEP;
	slider->pixelstep = (slider->maxpixel - slider->minpixel) * slider->step * 1.0 / 100;
	
	slider->r_x = -1;
	slider->r_y = -1;

	return slider;
}

int STK_SlidebarRegisterType()
{
	STK_WidgetFuncs *f;
	STK_WidgetRegisterType( "Slidebar", &f );
	f->draw = STK_SlidebarDraw;
	f->close = STK_SlidebarClose;
	
	return 0;
}

int STK_SlidebarClose(STK_Widget *widget)
{
	STK_Slidebar *slider = (STK_Slidebar *)widget;
	
	// now ready to free surface
	if (widget->surface) {
		SDL_FreeSurface(widget->surface);
	}
	// now ready to free slidebar structure
	free(slider);
	return 0;
}

// 
void STK_SlidebarDraw(STK_Widget *widget)
{
	STK_Slidebar *slider = (STK_Slidebar *)widget;
	SDL_Rect r;
	SDL_Surface *s = widget->surface;
	Uint32 tmpcolor;
	int bx, by;

	if (slider->orientation == STK_SLIDEBAR_VERTICAL) {
		bx = widget->border;
		by = slider->curpixel;
	}
	else {
		bx = slider->curpixel;
		by = widget->border;		
	}

	// fill background
	tmpcolor = SDL_MapRGB(widget->surface->format, widget->bgcolor.r, widget->bgcolor.g, widget->bgcolor.b);
	SDL_FillRect(s, NULL, tmpcolor);
	
	STK_BaseRectAssign(	&r, 
				bx,
				by,
				slider->b_width,
				slider->b_height );
	tmpcolor = SDL_MapRGB(widget->surface->format, widget->fgcolor.r, widget->fgcolor.g, widget->fgcolor.b);
	SDL_FillRect(s, &r, tmpcolor);
	// draw outer border
	STK_ImageDrawFrame(s, STK_IMAGE_FRAME_CONCAVE);
	// draw inner border
	STK_ImageDrawFrameWithRect(s, &r, STK_IMAGE_FRAME_CONVEX);
	
	return;
}

static void STK_SlidebarEventMouseButtonDown(STK_Object *object, void *signaldata, void *userdata)
{
	STK_Slidebar *slider = (STK_Slidebar *)object;
	STK_Widget *widget = (STK_Widget *)object;
	SDL_Event *event = (SDL_Event *)signaldata;
	
	printf("Enter slidebar mouse button down event process.\n");
	// if mouse point is in slidebar
	if (STK_WidgetIsInside(widget, event->motion.x, event->motion.y)) {
		int motion;
		if (slider->orientation == STK_SLIDEBAR_HORIZONTAL) {
			motion = event->motion.x;
			// if pressed is left mouse button
			if (event->button.button == SDL_BUTTON_LEFT) {
				// if mouse point is in part 1, move left, big step
				if (motion < (widget->rect.x + slider->curpixel)) {
					STK_SlidebarStep(slider, STK_SLIDEBAR_LEFT, STK_SLIDEBAR_STEP);
				}
				// if mouse point is in part 3, move right, big step
				else if (motion > (widget->rect.x + slider->curpixel + slider->b_width)) {
					STK_SlidebarStep(slider, STK_SLIDEBAR_RIGHT, STK_SLIDEBAR_STEP);
				}
				// if mouse point is in part 2, don't move, but change the button(slidebar)'s state, for later motion
				else {
					slider->state = STK_SLIDEBAR_DRAG;
					slider->r_x = motion - widget->rect.x - slider->curpixel;
					// maybe need to change the appearance of the button when was pressed
					// STK_WidgetEventRedraw(widget);
					return ;
				}
				slider->changed = 1;
				
			}
			// down button on touchpad
			if (event->button.button == 4)
				STK_SlidebarStep(slider, STK_SLIDEBAR_RIGHT, STK_SLIDEBAR_STEP);
			// up button on touchpad
			if (event->button.button == 5)
				STK_SlidebarStep(slider, STK_SLIDEBAR_LEFT, STK_SLIDEBAR_STEP);
		
		}
		// vertical
		else {
			// take cake of y coordination
			motion = event->motion.y;
			// only left mouse button can affect
			if (event->button.button == SDL_BUTTON_LEFT) {
				// if mouse point is in part 3, move downwards
				if (motion > (widget->rect.y + slider->curpixel + slider->b_height)) {
					STK_SlidebarStep(slider, STK_SLIDEBAR_DOWN, STK_SLIDEBAR_STEP);
				}
				// if mouse point is in part 2, change the slidebar's state
				else if (motion > (widget->rect.y + slider->curpixel)) {
					slider->state = STK_SLIDEBAR_DRAG;
					slider->r_y = motion - widget->rect.y - slider->curpixel;
					// STK_WidgetEventRedraw(widget);
					return ;
				}
				// if mouse point is in part 1, move upwards
				else {
					STK_SlidebarStep(slider, STK_SLIDEBAR_UP, STK_SLIDEBAR_STEP);
				}
				slider->changed = 1;
			}
			// down button on touchpad
			if (event->button.button == 4)
                                STK_SlidebarStep(slider, STK_SLIDEBAR_UP, STK_SLIDEBAR_STEP);
                        // up button on touchpad
                        if (event->button.button == 5)
                                STK_SlidebarStep(slider, STK_SLIDEBAR_DOWN, STK_SLIDEBAR_STEP);
		}
	}
	
	// if have some changes
	if (slider->changed) {
		STK_SignalEmit(widget, "value-changed", event);
		// restore state
		slider->changed = 0;
		// redraw on screen
		// STK_WidgetEventRedraw(widget);
	}
	printf("exit slidebar mouse button down event process.\n");

}

// button up only change the state of the bar
static void STK_SlidebarEventMouseButtonUp(STK_Object *object, void *signaldata, void *userdata)
{
	STK_Slidebar *slider = (STK_Slidebar *)object;
	STK_Widget *widget = (STK_Widget *)object;
	
	printf("Enter slidebar mouse button up event process.\n");
	if (slider->state == STK_SLIDEBAR_DRAG) {
		slider->state = STK_SLIDEBAR_IDLE;
		// force to do a redrawing
		slider->changed = 1;
	}
	
	if (slider->changed) {
		STK_SignalEmit(widget, "value-changed", NULL);
		slider->changed = 0;
		// STK_WidgetEventRedraw(widget);
	}
	printf("exit slidebar mouse button up event process.\n");
}

// drag to move 
static void STK_SlidebarEventMouseMotion(STK_Object *object, void *signaldata, void *userdata)
{
	STK_Slidebar *slider = (STK_Slidebar *)object;
	STK_Widget *widget = (STK_Widget *)object;
	SDL_Event *event = (SDL_Event *)signaldata;
	int t;
	
	printf("Enter slidebar mouse motion event process.\n");
	if (slider->state == STK_SLIDEBAR_DRAG) {
		// horizontal
		if (slider->orientation == STK_SLIDEBAR_HORIZONTAL)
			t = event->motion.x - widget->rect.x - slider->r_x;
		// vertical
		else
			t = event->motion.y - widget->rect.y - slider->r_y;

		if (t > slider->minpixel && t < slider->maxpixel) 
			slider->curpixel = t;
		else if (t >= slider->maxpixel)
			slider->curpixel = slider->maxpixel;
		else if (t <= slider->minpixel)
			slider->curpixel = slider->minpixel;
		
		// calculate the current value presented by the slidebar
		STK_SlidebarPixelToValue(slider);
		slider->changed = 1;
	}

	if (slider->changed) {
		STK_SignalEmit(widget, "value-changed", NULL);
		slider->changed = 0;
		STK_WidgetEventRedraw(widget);
	}
	printf("exit slidebar mouse motion event process.\n");
}

static void STK_SlidebarValueToPixel(STK_Slidebar *slider)
{
	slider->curpixel = slider->minpixel + (slider->maxpixel - slider->minpixel)
				* (slider->curvalue - slider->minvalue) / (slider->maxvalue - slider->minvalue);
	
}

static void STK_SlidebarPixelToValue(STK_Slidebar *slider)
{
	slider->curvalue = slider->minvalue + (slider->maxvalue - slider->minvalue) 
				* (slider->curpixel - slider->minpixel) / (slider->maxpixel - slider->minpixel);
}

static void STK_SlidebarStep(STK_Slidebar *slider, int dir, int step)
{
	switch (dir) {
	case STK_SLIDEBAR_UP:
	case STK_SLIDEBAR_LEFT:
		if (slider->curpixel > slider->minpixel) {
			if (slider->curvalue > slider->step) {
				slider->curvalue -= slider->step;
				slider->curpixel -= slider->pixelstep;
			}
			else {
				slider->curvalue = slider->minvalue;
				slider->curpixel = slider->minpixel;
			}
			slider->changed = 1;
		}
		break;
	case STK_SLIDEBAR_DOWN:
	case STK_SLIDEBAR_RIGHT:
		if (slider->curpixel < slider->maxpixel) {
			if (slider->curvalue < slider->maxvalue - slider->step) {
				slider->curvalue += slider->step;
				slider->curpixel += slider->pixelstep;
			}
			else {
				slider->curvalue = slider->maxvalue;
				slider->curpixel = slider->maxpixel;
			}
			slider->changed = 1;
		}
		break;
	}	

}

Uint32 STK_SlidebarGetCurrentValue(STK_Slidebar *slider)
{
	return slider->curvalue;
}

int STK_SlidebarSetValue(STK_Slidebar *slider, int type, int value)
{
	STK_Widget *widget = (STK_Widget *)slider;
	
	// 
	if (slider == NULL)
		return 1;
	switch (type) {
	case STK_SLIDEBAR_CURVALUE:
		if (value != slider->curvalue) {
			if (value >= slider->maxvalue)
				slider->curvalue = slider->maxvalue;
			else
				slider->curvalue = value;
			STK_SlidebarValueToPixel(slider);
			STK_SignalEmit(widget, "value-changed", NULL);
			STK_WidgetEventRedraw(widget);			
		}	
		break;
	case STK_SLIDEBAR_MINVALUE:
		slider->minvalue = value;
		if (slider->curvalue < slider->minvalue) {
			slider->curvalue = slider->minvalue;
			STK_SignalEmit(widget, "value-changed", NULL);
			STK_WidgetEventRedraw(widget);
		}
		break;
	case STK_SLIDEBAR_MAXVALUE:
		slider->maxvalue = value;
		if (slider->curvalue > slider->maxvalue) {
			slider->curvalue = slider->maxvalue;
			STK_SignalEmit(widget, "value-changed", NULL);
			STK_WidgetEventRedraw(widget);
		}
		break;
	case STK_SLIDEBAR_STEPVALUE:
		slider->step = value;
		break; 
	}
	
	return 0;
}
