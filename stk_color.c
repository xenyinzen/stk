#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_base.h"
#include "stk_color.h"

int STK_ColorSetGlobal( int which, Uint8 r, Uint8 g, Uint8 b )
{
	STK_Widget *widget;
	STK_Window *win = STK_WindowGetTop();
	if (!win)
		return -1;
	widget = (STK_Widget *)win;
	
	switch (which) {
	case STK_COLOR_BACKGROUND:
		STK_BaseColorAssign(&widget->bgcolor, r, g, b);
		break;
	case STK_COLOR_FOREGROUND:
		STK_BaseColorAssign(&widget->fgcolor, r, g, b);
		break;
	default:
		break;	
	}

	return 0;
}