#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_button.h"
#include "stk_label.h"

#define STK_FRAME_THICKNESS	2
#define STK_FRAME_STYLE_CONVEX   10  //temporary

STK_Widget *STK_ButtonNew(  )
{
	STK_Button *button;
	STK_Widget *widget;
	STK_Object *object;
	
	button = (STK_Button *)STK_Malloc(sizeof(STK_Button));

	button->frame.area[i] = (STK_Image *)STK_Malloc(sizeof(STK_Image));
	button->frame.thickness = STK_FRAME_THICKNESS;

	widget = (STK_Widget *)button;
	widget->type = STK_WIDGET_BUTTON;
	widget->name = "Button";
	widget->flags = 0;
	
	object = (STK_Object *)widget;
	
	
	button->state = STK_BUTTON_UP;
	button->fillway = STK_FRAME_STYLE_CONVEX;
	button->font = NULL;
	
	return (STK_Widget *)widget;	
}

int STK_ButtonRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "Button", &f );
        f->draw = STK_ButtonDraw;
        f->close = STK_ButtonClose;

        return 1;
}

