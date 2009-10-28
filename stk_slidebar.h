#ifndef _STK_SLIDEBAR_H_
#define _STK_SLIDEBAR_H_

#include "stk_widget.h"

#define STK_SLIDEBAR_STEP 5

enum {
	STK_SLIDEBAR_IDLE,
	STK_SLIDEBAR_DRAG
};

enum {
	STK_SLIDEBAR_HORIZONTAL,
	STK_SLIDEBAR_VERTICAL
};

enum {
	STK_SLIDEBAR_LEFT,
	STK_SLIDEBAR_RIGHT,
	STK_SLIDEBAR_UP,
	STK_SLIDEBAR_DOWN
};

enum {
	STK_SLIDEBAR_CURVALUE,
	STK_SLIDEBAR_MAXVALUE,
	STK_SLIDEBAR_MINVALUE,
	STK_SLIDEBAR_STEPVALUE

};

typedef struct STK_Slidebar {
	STK_Widget widget;
	Uint32 orientation;	// horizontal or vertical

	Uint32 b_width;		// button width
	Uint32 b_height;	// button height
	
	int state;		// store the state of the slidebar
	int changed;		// represent whether the state of the slidebar has been changed
	int r_x;		// relative x distance between mouse and button left top corner
	int r_y;		// relative y distance between mouse and button left top corner
	
	Uint32 maxvalue;	// Unit: percentage
	Uint32 minvalue;	// Unit: percentage
	Uint32 curvalue;	// Unit: percentage
	Uint32 step;		// percentage
	
	float maxpixel;		// Unit: pixel
	float minpixel;		// Unit: pixel
	float curpixel;		// Unit: pixel
	float pixelstep;	// pixels on screen, should be accurate, otherwise will introduce in big residuals.
	
	Uint32 value_locked;
} STK_Slidebar;


STK_Widget *STK_SlidebarNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h);
int STK_SlidebarRegisterType();
int STK_SlidebarClose(STK_Widget *widget);
void STK_SlidebarDraw(STK_Widget *widget);

Uint32 STK_SlidebarGetCurrentValue(STK_Slidebar *slider);
int STK_SlidebarSetValue(STK_Slidebar *slider, int type, int value);

#endif /* _STK_SLIDEBAR_H_ */
