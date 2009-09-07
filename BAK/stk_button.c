#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_button.h"
#include "stk_label.h"


STK_Widget *stk_button_new(  )
{
	STK_Button *button;
	STK_Widget *widget;
	STK_Object *object;
	
	button = (STK_Button *)stk_malloc(sizeof(STK_Button));
	widget = (STK_Widget *)button;
	
	widget->type = STK_WIDGET_TYPE;
	widget->flags = 0;
	
	object = (STK_Object *)widget;
	
	
	button->state = STK_BUTTON_UP;
	button->font = NULL;
	
	return (STK_Widget *)widget;	
}

