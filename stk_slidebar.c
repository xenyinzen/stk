#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk_mm.h"
#include "stk_slidebar.h"

#define STK_SLIDEBAR_BORDER_THICKNESS 3

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
STK_Widget *STK_SlidebarNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
	STK_Object *object;
	STK_Widget *widget;
	STK_Slidebar *slider;
	int orientation;
	
	slider = (STK_Slidebar *)STK_Malloc(sizeof(STK_Slidebar));
	widget = (STK_Widget *)slider;
	object = (STK_Object *)slider;
	
	widget->rect.x = x;
	widget->rect.y = y;
	widget->rect.w = w;
	widget->rect.h = h;
	
	if (w > h)
		orientation = STK_SLIDEBAR_HORIZONTAL;
	else
		orientation = STK_SLIDEBAR_VERTICAL;
	
	widget->flags = 0;
	widget->type = "STK_Slidebar";
	
	// connect three mouse events
	STK_SignalConnect(widget, "mousebuttondown", STK_SlidebarEventMouseButtonDown, widget);
	STK_SignalConnect(widget, "mousebuttonup", STK_SlidebarEventMouseButtonUp, widget);
	STK_SignalConnect(widget, "mousemotion", STK_SlidebarEventMouseMotion, widget);
	
	slider->minvalue = 0;
	slider->maxvalue = 100;
	slider->curvalue = 0;
	slider->value_locked = 0;
	
	slider->orientation = orientation;
	// interval
	slider->minpixel = STK_SLIDEBAR_BORDER_THICKNESS;
	slider->curpixel = slider->minpixel;
	// 6 is 2*3, the interval between bar border and button border
	if (slider->orientation == STK_SLIDEBAR_VERTICAL) {
		slider->b_width = w - 2*STK_SLIDEBAR_BORDER_THICKNESS;
		slider->b_height = slider->b_width * 3 / 4;
		slider->maxpixel = h - slider->b_height - STK_SLIDEBAR_BORDER_THICKNESS;
	}
	else {
		slider->b_height = h - 2*STK_SLIDEBAR_BORDER_THICKNESS;
		slider->b_width = slider->b_height * 3 / 4;
		slider->maxpixel = w - slider->b_width - STK_SLIDEBAR_BORDER_THICKNESS;
	}
	
	slider->step = STK_SLIDEBAR_STEP;
	slider->pixelstep = (slider->maxpixel - slider->minpixel) * slider->step * 1.0 / 100;
	
	slider->r_x = -1;
	slider->r_y = -1;

	return widget;
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
	STK_Slidebar *slider = (STK_Slidebar *)widget
	
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
	STK_Slidebar * slider = (STK_Slidebar *)widget;
	
	// vertical
	if (slider->orientation == STK_SLIDEBAR_VERTICAL) {
		SDL_Rect r;
		SDL_Surface *s = widget->surface;
		
		// the background color of slidebar, temporarily
		r.x = 0;
		r.y = 0;
		r.w = widget->rect.w;
		r.h = widget->rect.h;
		SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0xf4, 0xf0, 0xe8));
		
		// fill the button background color
		r.x = STK_SLIDEBAR_BORDER_THICKNESS;
		r.y = slider->curpixel + STK_SLIDEBAR_BORDER_THICKNESS;
		r.w = widget->rect.w - 2 * STK_SLIDEBAR_BORDER_THICKNESS;
		// we define the button's w:h is 4:3
		r.h = r.w * 3 / 4;
		SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0xd4, 0xd0, 0xc8));
		
		if (r.h < 10)
			r.h = 10;
		
		// fill the border of the button
		STK_DrawLine(s, r.x, r.y+r.h-1, r.x+r.w-1, r.y+r.h-1, 0x404040ff);	// bottom line 1
		STK_DrawLine(s, r.x+r.w-1, r.y, r.x+r.w-1, r.y+r.h-1, 0x404040ff);	// right line 1
		STK_DrawLine(s, r.x+1, r.y+r.h-2, r.x+r.w-2, r.y+r.h-2, 0x808080ff);	// bottom line 2
		STK_DrawLine(s, r.x+r.w-2, r.y+1, r.x+r.w-2, r.y+r.h-2, 0x808080ff);	// right line 2
		
		STK_DrawLine(s, r.x+1, r.y+1, r.x+r.w-2, r.y+1, 0xffffffff);		// top line 2
		STK_DrawLine(s, r.x+1, r.y+1, r.x+1, r.y+r.h-2, 0xffffffff);		// left line 2
		STK_DrawLine(s, r.x, r.y, r.x+r.w-1, r.y, 0xd4d0c8ff);			// top line 1
		STK_DrawLine(s, r.x, r.y, r.x, r.y+r.h-1, 0xd4d0c8ff);			// left line 1
	} 
	// horizontal
	else {
		SDL_Rect r;
		SDL_Surface *s = widget->surface;
		
		r.x = 0;
		r.y = 0;
		r.w = widget->rect.w;
		r.h = widget->rect.h;
		SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0xf4, 0xf0, 0xe8));
		
		r.x = slider->curpixel + STK_SLIDEBAR_BORDER_THICKNESS;
		r.y = STK_SLIDEBAR_BORDER_THICKNESS;
		r.h = widget->rect.h - 2 * STK_SLIDEBAR_BORDER_THICKNESS;
		r.w = r.h * 3 / 4;
		SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0xd4, 0xd0, 0xc8));
		
		if (r.w < 10)
			r.w = 10;
		
		// fill the border of the inner button
		STK_DrawLine(s, r.x, r.y+r.h-1, r.x+r.w-1, r.y+r.h-1, 0x404040ff);
		STK_DrawLine(s, r.x+r.w-1, r.y, r.x+r.w-1, r.y+r.h-1, 0x404040ff);
		STK_DrawLine(s, r.x+1, r.y+r.h-2, r.x+r.w-2, r.y+r.h-2, 0x808080ff);
		STK_DrawLine(s, r.x+r.w-2, r.y+1, r.x+r.w-2, r.y+r.h-2, 0x808080ff);
		
		STK_DrawLine(s, r.x+1, r.y+1, r.x+r.w-2, r.y+1, 0xffffffff);
		STK_DrawLine(s, r.x+1, r.y+1, r.x+1, r.y+r.h-2, 0xffffffff);
		STK_DrawLine(s, r.x, r.y, r.x+r.w-1, r.y, 0xd4d0c8ff);
		STK_DrawLine(s, r.x, r.y, r.x, r.y+r.h-1, 0xd4d0c8ff);		
	
	}
}

static void STK_SlidebarEventMouseButtonDown(STK_Object *object, void *signaldata, void *userdata)
{
	STK_Slidebar *slider = (STK_Slidebar *)object;
	STK_Widget *widget = (STK_Widget *)object;
	STK_Event *event = (STK_Event *)signaldata;
	
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
					STK_WidgetRedrawEvent(widget);
					return ;
				}
				
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
				if (motion > (widget->rect.y + slider->curpixel + slider->b_height) {
					STK_SlidebarStep(slider, STK_SLIDEBAR_DOWN, STK_SLIDEBAR_STEP);
				}
				// if mouse point is in part 2, change the slidebar's state
				else if (motion > (widget->rect.y + slider->curpixel)) {
					slider->state = STK_SLIDEBAR_DRAG;
					slider->r_y = motion - widget->rect.y - slider->curpixel;
					STK_WidgetRedrawEvent(widget);
					return ;
				}
				// if mouse point is in part 1, move upwards
				else {
					STK_SlidebarStep(slider, STK_SLIDEBAR_UP, STK_SLIDEBAR_STEP);
				}
			}
			// down button on touchpad
			if (event->button.button == 4)
                                STK_SlidebarStep(slider, STK_SLIDEBAR_UP, STK_SLIDEBAR_STEP);
                        // up button on touchpad
                        if (event->button.button == 5)
                                STK_SlidebarStep(slider, STK_SLIDEBAR_BUTTON, STK_SLIDEBAR_STEP);
		}
	}
	
	// if have some changes
	if (slider->changed) {
		STK_SignalEmit(widget, "value-changed", event);
		// restore state
		slider->changed = 0;
		// redraw on screen
		STK_WidgetEventRedraw(widget);
	}

}

// button up only change the state of the bar
static void STK_SlidebarEventMouseButtonUp(STK_Object *object, void *signaldata, void *userdata)
{
	STK_Slidebar *slider = (STK_Slidebar *)object;
	STK_Widget *widget = (STK_Widget *)object;
	
	if (slider->state == STK_SLIDEBAR_DRAG) {
		slider->state = STK_SLIDEBAR_IDLE;
		// force to do a redrawing
		slider->changed = 1;
	}
	
	if (slider->changed) {
		STK_SignalEmit(widget, "value-changed", NULL);
		slider->changed = 0;
		STK_WidgetEventRedraw(widget);
	}
}

// drag to move 
static void STK_SlidebarEventMouseMotion(STK_Object *object, void *signaldata, void *userdata)
{
	STK_Slidebar *slider = (STK_Slidebar *)object;
	STK_Widget *widget = (STK_Widget *)object;
	SDL_Event *event = (SDL_Event *)signaldata;
	
	if (slider->state == STK_SLIDEBAR_DRAG) {
		// horizontal
		if (slider->orientation == STK_SLIDEBAR_HORIZONTAL) {
			slider->curpixel = event->motion.x - widget->rect.x - slider->r_x;
		}
		// vertical
		else {
			slider->curpixel = event->motion.y - widget->rect.y - slider->r_y;
		}
		// calculate the current value presented by the slidebar
		STK_SlidebarPixelToValue(slider);
		slider->changed = 1;
	}

	if (slider->changed) {
		STK_SignalEmit(widget, "value-changed", NULL);
		slider->changed = 0;
		STK_WidgetEventRedraw(widget);
	}
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
	case STK_SLIDEBAR_CURRENT_VALUE:
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
	case STK_SLIDEBAR_MIN_VALUE:
		slider->minvalue = value;
		if (slider->curvalue < slider->minvalue) {
			slider->curvalue = slider->minvalue;
			STK_SignalEmit(widget, "value-changed", NULL);
			STK_WidgetEventRedraw(widget);
		}
		break;
	case STK_SLIDEBAR_MAX_VALUE:
		slider->maxvalue = value;
		if (slider->curvalue > slider->maxvalue) {
			slider->curvalue = slider->maxvalue;
			STK_SignalEmit(widget, "value-changed", NULL);
			STK_WidgetEventRedraw(widget);
		}
		break;
	case STK_SLIDEBAR_STEP:
		slider->step = value;
		break; 
	}
	
	return 0;
}
