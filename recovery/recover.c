
#include "stk.h"
#include "callback.h"
#include "recover.h"
#include <lua.h>
#include <lauxlib.h>


/*
 * Global variables
 */
lua_State *L;
Recovery *grec;
int progress = 0;

// ====================================================

STK_Label *draw_label_head()
{
	
	STK_Label *label = (STK_Label *)STK_LabelNew(5, 5, 250, 80, "Lemote Recovery Tool v1.0.0", 1);
	STK_LabelSetAlignment(label, STK_LABEL_CENTER);
	STK_LabelSetColor(label, STK_COLOR_BACKGROUND, 0x60, 0x80, 0x30);
	STK_WidgetShow((STK_Widget *)label);
	
	return label;
}

STK_Label *draw_label_status()
{
	
	STK_Label *label = (STK_Label *)STK_LabelNew(10, 440, 0, 0, "准备还原", 0);
	STK_WidgetShow((STK_Widget *)label);
	
	return label;
}

STK_Button *draw_button_start()
{
	STK_Button *button = STK_ButtonNew(420, 50, 90, 35, "开始还原");
	STK_Widget *widget = (STK_Widget *)button;
	STK_SignalConnect(widget, "mousebuttondown", cb_button_start, widget);
	
	STK_WidgetShow((STK_Widget *)button);
	return button;
}

STK_Button *draw_button_exit()
{
	STK_Button *button = STK_ButtonNew(520, 50, 90, 35, "退出");
	STK_Widget *widget = (STK_Widget *)button;
	STK_WidgetSetFixed((STK_Widget *)button, 1);
	STK_SignalConnect(widget, "mousebuttondown", cb_button_exit, widget);
	
	STK_WidgetShow((STK_Widget *)button);
	return button;
}

STK_MsgBox *draw_msgbox()
{
	STK_MsgBox *msgbox = STK_MsgBoxNew(5, 100, 630, 260, "请按‘开始还原’按钮还原系统。");
	STK_WidgetEventShow((STK_Widget *)msgbox);
	
	return msgbox;
}

STK_ProgressBar *draw_progressbar(Uint32 *p)
{
	STK_ProgressBar *pb = STK_ProgressBarNew(5, 380, 630, 40, p);
	STK_WidgetEventShow((STK_Widget *)pb);
	
	return pb;
}

int clearLog()
{
//	system("rm log.txt");
	system("rm progress.txt");

	return 0;
}


int main(int argc,char **argv)
{
	L = luaL_newstate();
	luaL_openlibs(L);
	
    	
    	STK_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER, SDL_SWSURFACE, 640, 480, 32);

    	STK_WindowNew(0, 0, 640, 480);
    	
	clearLog();
    	grec = (Recovery *)STK_Malloc(sizeof(Recovery));
    	grec->label_head = draw_label_head();
	grec->button_start = draw_button_start();
	grec->button_exit = draw_button_exit();
	grec->msgbox = draw_msgbox();
	grec->pb = draw_progressbar(&progress);
	grec->label_status = draw_label_status();
	
	STK_WindowOpen();
	
    	STK_Main();
}
