#include <stdio.h>
#include <stdlib.h>






int stk_frame_new(STk_Widget *widget)
{	
	// pass this statement, means have frame member
	if ( !(widget->flags & STK_WIDGET_FRAMEABLE))
		return 0;	
	
	// create the 8 surfaces of one frame
	widget->frame.topleft = (STK_Image *)stk_malloc(sizeof(STK_Image));
	widget->frame.top = (STK_Image *)stk_malloc(sizeof(STK_Image));
	widget->frame.topright = (STK_Image *)stk_malloc(sizeof(STK_Image));
	widget->frame.left = (STK_Image *)stk_malloc(sizeof(STK_Image));
	widget->frame.right = (STK_Image *)stk_malloc(sizeof(STK_Image));
	widget->frame.bottomleft = (STK_Image *)stk_malloc(sizeof(STK_Image));
	widget->frame.bottom = (STK_Image *)stk_malloc(sizeof(STK_Image));
	widget->frame.bottomright = (STK_Image *)stk_malloc(sizeof(STK_Image));
	
	


}