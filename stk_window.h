#ifndef _STK_WINDOW_H_
#define _STK_WINDOW_H_





typedef struct STK_Window {
	STK_Widget widget;
	
	Uint8 type;
	Uint32 bgcolor;
	int visible;

	STK_Widget *focus_widget;
	STK_WidgetListNode *widget_list;
} STK_Window;









#endif /* _STK_WINDOW_H_ */