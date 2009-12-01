
#include "stk.h"

STK_Label *draw_label()
{
	
	STK_Label *label = (STK_Label *)STK_LabelNew(5, 5, 250, 80, "Lemote Recovery Tool v1.0.0", 1);
	STK_LabelSetAlignment(label, STK_LABEL_CENTER);
	STK_LabelSetColor(label, STK_COLOR_BACKGROUND, 0x60, 0x80, 0x30);
	STK_WidgetShow((STK_Widget *)label);
	
	return label;
}

STK_Button *draw_button_start()
{
	STK_Button *button = STK_ButtonNew(420, 50, 90, 35, "开始还原");
	STK_WidgetSetFixed((STK_Widget *)button, 1);
	
	STK_WidgetShow((STK_Widget *)button);
	return button;
}

STK_Button *draw_button_exit()
{
	STK_Button *button = STK_ButtonNew(520, 50, 90, 35, "退出");
	STK_WidgetSetFixed((STK_Widget *)button, 1);
	
	STK_WidgetShow((STK_Widget *)button);
	return button;
}

STK_MsgBox *draw_msgbox()
{
	STK_MsgBox *msgbox = STK_MsgBoxNew(5, 100, 630, 260, "first line");
	STK_WidgetEventShow((STK_Widget *)msgbox);
	
	return msgbox;
}

STK_ProgressBar *draw_progressbar(Uint32 *p)
{
	STK_ProgressBar *pb = STK_ProgressBarNew(5, 380, 630, 40, p);
	STK_WidgetEventShow((STK_Widget *)pb);
	
	return pb;
}


int main(int argc,char **argv)
{
    	int p;
    	
    	STK_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER, SDL_SWSURFACE, 640, 480, 32);

    	STK_WindowNew(0, 0, 640, 480);
    	
    	draw_label();
	draw_button_start();
	draw_button_exit();
	draw_msgbox();
	draw_progressbar(&p);
	
	STK_WindowOpen();
	
    	STK_Main();
}
