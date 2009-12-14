
#include "stk.h"
#include "callback.h"
#include "recover.h"
#include <lua.h>
#include <lauxlib.h>


#define BORDER	5
/*
 * Global variables
 */
lua_State *L;
Recovery *grec;
// default resolution is 800x600
Screen  scr = { 800, 600 };
int progress = 0;



// ====================================================

STK_Label *draw_label_head()
{
	STK_Font *font = STK_FontGetDefaultFont(2); 	 
	STK_Label *label = (STK_Label *)STK_LabelNew(BORDER, BORDER, 400, 80, "Lemote Recovery Tool v1.0", 1);
	STK_LabelSetAlignment(label, STK_LABEL_CENTER);
	STK_LabelSetColor(label, STK_COLOR_BACKGROUND, 0x60, 0x80, 0x30);
	STK_WidgetShow((STK_Widget *)label);
	STK_LabelSetFont(label, font);	

	return label;
}

STK_Label *draw_label_status()
{
	STK_Font *font = STK_FontGetDefaultFont(2); 
	STK_Label *label = (STK_Label *)STK_LabelNew(BORDER + 5, scr.h - 50, 0, 0, "准备还原", 0);
	STK_WidgetShow((STK_Widget *)label);
	STK_LabelSetFont(label, font);
	
	return label;
}

STK_Button *draw_button_start()
{
	STK_Button *button = STK_ButtonNew(scr.w - 220, BORDER + 45, 90, 35, "开始还原");
	STK_Widget *widget = (STK_Widget *)button;
	STK_SignalConnect(widget, "mousebuttondown", cb_button_start, widget);
	
	STK_WidgetShow((STK_Widget *)button);
	return button;
}

STK_Button *draw_button_exit()
{
	STK_Button *button = STK_ButtonNew(scr.w - 120, BORDER + 45, 90, 35, "退出");
	STK_Widget *widget = (STK_Widget *)button;
	STK_WidgetSetFixed((STK_Widget *)button, 1);
	STK_SignalConnect(widget, "mousebuttondown", cb_button_exit, widget);
	
	STK_WidgetShow((STK_Widget *)button);
	return button;
}

STK_MsgBox *draw_msgbox()
{
	STK_Font *font = STK_FontGetDefaultFont(1); 
	STK_MsgBox *msgbox = STK_MsgBoxNew(BORDER, BORDER + 100, scr.w - 2*BORDER, scr.h - 2*BORDER - 210, "请确保还原文件已在U盘上，然后按‘开始还原’按钮还原系统。");
	STK_WidgetEventShow((STK_Widget *)msgbox);
	STK_MsgBoxSetFont(msgbox, font);
	
	return msgbox;
}

STK_ProgressBar *draw_progressbar(Uint32 *p)
{
	STK_ProgressBar *pb = STK_ProgressBarNew(BORDER, scr.h - 100, scr.w - 2*BORDER, 40, p);
	STK_WidgetEventShow((STK_Widget *)pb);
	
	return pb;
}

int clearLog()
{
	system("echo 0 > /root/progress.txt");

	return 0;
}


int main(int argc,char **argv)
{
	FILE *fp;
	char cmdline[256] = { 0 };
	int t = 0;
	
	// read machine type info
	if ((fp = fopen("/proc/cmdline", "r")) == NULL) {
		fprintf(stderr, "%s\n", "Can't open file /proc/cmdline! Use default resolution: 800x600.");
	}
	else if (fgets(cmdline, 256, fp) == NULL) {
		fprintf(stderr, "%s\n", "NULL string in /proc/cmdline! Use default resolution: 800x600.");
	}
	// find it: machtype=lynloong
	else if (strstr(cmdline, "machtype=lynloong") != 0) {
		scr.w = 1360;
		scr.h = 768;
	}
	else {
		fprintf(stderr, "%s\n", "There is no arg 'machtype=lynloong' in /proc/cmdline. Use default resolution: 800x600.");
	}
	
	// start main body
	L = luaL_newstate();
	luaL_openlibs(L);
	registerFuncs4Lua();	
    	
//    	STK_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER, SDL_SWSURFACE, scr.w, scr.h, 32);
    	STK_Init(SDL_INIT_VIDEO, SDL_SWSURFACE, scr.w, scr.h, 32);

    	STK_WindowNew(0, 0, scr.w, scr.h);
    	
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

	system("sleep 1 && reboot");
	
	return 0;
}
