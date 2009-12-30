
#include "unistd.h"
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
// default resolution is 800x600
Screen  scr = { 800, 600 };
Misc misc = {
	0,			// running
	0,			// progress
	0,			// recover_from
	0,			// recover_scheme
	{1, 0, 0},		// rb_states[], whole, system, user
	0,			// network
	0,
};

const char *recoway[NRECOWAY] = {
	"当前准备通过U盘还原 ",
	"当前准备通过硬盘还原",
	"当前准备通过网络还原"
};


// ====================================================

STK_Label *draw_label_head()
{
	char tmp[256];
	char ver[16];
	FILE *fp;
	
	if ((fp = fopen("./version.txt", "r")) == NULL) {
		fprintf(stderr, "Can't open version.txt file.\n");
		exit(-1);
	}
	fgets(ver, 16, fp);
	fclose(fp);
	
	sprintf(tmp, "Lemote Recovery Tool %s", ver);
	
	STK_Font *font = STK_FontGetDefaultFont(2); 	 
	STK_Label *label = (STK_Label *)STK_LabelNew(BORDER, BORDER, 400, 80, tmp, 1);
	STK_LabelSetAlignment(label, STK_LABEL_CENTER);
	STK_LabelSetColor(label, STK_COLOR_BACKGROUND, 0x60, 0x80, 0x30);
	STK_WidgetShow((STK_Widget *)label);
	STK_LabelSetFont(label, font);	

	return label;
}

STK_Label *draw_label_status()
{
	char tmp[256];
	
	sprintf(tmp, "%s", recoway[misc.recover_from]);
	STK_Font *font = STK_FontGetDefaultFont(2); 
	STK_Label *label = (STK_Label *)STK_LabelNew(BORDER + 5, scr.h - 50, 0, 0, tmp, 0);
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
	STK_WidgetSetFixed(widget, 1);
	STK_SignalConnect(widget, "mousebuttondown", cb_button_exit, widget);
	
	STK_WidgetShow(widget);
	return button;
}

STK_Button *draw_button_pro()
{
	STK_Button *button = STK_ButtonNew(scr.w - 120, scr.h - 50, 90, 35, "高级");
	STK_Widget *widget = (STK_Widget *)button;
	STK_WidgetSetFixed(widget, 1);
	STK_SignalConnect(widget, "mousebuttondown", cb_button_pro, widget);
	
	STK_WidgetShow(widget);
	return button;
}

STK_RadioGroup *draw_radiogroup(int rb_states[], int n)
{
	char *items[NSCHEME] = {
		"全盘还原",
		"系统数据区还原",
		"用户数据区还原"
	};

//	int num1 = sizeof(items) / sizeof(char);
/*	int num2 = sizeof(rb_states) / sizeof(int);
	if (num2 != n) {
		fprintf(stderr, "Error on rb_states array size.");
		exit(-1);
	}	
*/	
	STK_RadioGroup *rg = STK_RadioGroupNew(scr.w - BORDER - 160, BORDER + 125, 150, 0, items, rb_states, n);
	
	STK_WidgetShow((STK_Widget *)rg);
	return rg;
}

STK_RadioButton *draw_radiobutton()
{
	STK_RadioButton *rb = STK_RadioButtonNew(scr.w - BORDER - 160, BORDER + 250, 150, 0, "从网络还原", &misc.network);
	STK_Widget *widget = (STK_Widget *)rb;
	STK_SignalConnect(widget, "mousebuttondown", cb_rb_network, widget);
	STK_WidgetShow(widget);
	
	return rb;	
}

STK_MsgBox *draw_msgbox()
{
	STK_Font *font = STK_FontGetDefaultFont(1); 
	STK_MsgBox *msgbox = STK_MsgBoxNew(BORDER, BORDER + 100, scr.w - 2*BORDER, scr.h - 2*BORDER - 210, "请按‘开始还原’按钮还原系统。");
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

int chooseScreenResolution()
{
	FILE *fp;
	char cmdline[256] = {0};

	// read machine type info
	if ((fp = fopen("/proc/cmdline", "r")) == NULL) {
		fprintf(stderr, "%s\n", "Can't open file /proc/cmdline! Use default resolution: 800x600.");
	}
	else if (fgets(cmdline, 256, fp) == NULL) {
		fprintf(stderr, "%s\n", "NULL string in /proc/cmdline! Use default resolution: 800x600.");
	}
	// find it, lynloong
	else if (strstr(cmdline, "machtype=lynloong") != 0) {
		scr.w = 1360;
		scr.h = 768;
	}
	// find it, yeeloong
	else if (strstr(cmdline, "machtype=yeeloong") != 0) {
		scr.w = 1024;
		scr.h = 600;
	}
	// find it, fuloong
	else if (strstr(cmdline, "machtype=fuloong") != 0) {
		scr.w = 1024;
		scr.h = 768;
	}	
	else {
		fprintf(stderr, "%s\n", "There is no arg 'machtype=lynloong' in /proc/cmdline. Use default resolution: 800x600.");
	}
}

int findMountDir(char config_file[])
{
	FILE *fp;
	char strtmp[256] = {0};
	fp = popen("mount", "r");
	while (fgets(strtmp, 256, fp)) {
		// if /root/udisk mounted
		if (strstr(strtmp, "/root/udisk")) {
			sprintf(config_file, "/root/udisk/config.txt");
			misc.recover_from = RECO_FROM_UDISK;
			break;
		}
		// if /root/ldisk mounted
		if (strstr(strtmp, "/root/ldisk")) {
			sprintf(config_file, "/root/ldisk/config.txt");
			misc.recover_from = RECO_FROM_LDISK;
			break;
		}
	}
	
	pclose(fp);
	return 0;
}

int readAutoStartFile(FILE *fp)
{
	unsigned int tmp1, tmp2;
	
	fscanf(fp, "%d", &tmp1);
	fscanf(fp, "%d", &tmp2);
	
	if (tmp2 > 2) {
		fprintf(stderr, "Configuration error in autostart.txt\n");
	}
	
	misc.autostart = (tmp1 == 0? 0: 1);
	misc.recover_scheme = tmp2;
	
	if (misc.autostart) {
		switch (misc.recover_scheme) {
		case 0:
			misc.rb_states[0] = 1;
			misc.rb_states[1] = 0;
			misc.rb_states[2] = 0;
			break;

		case 1:
			misc.rb_states[0] = 0;
			misc.rb_states[1] = 1;
			misc.rb_states[2] = 0;
			break;

		case 2:
			misc.rb_states[0] = 0;
			misc.rb_states[1] = 0;
			misc.rb_states[2] = 1;
			break;
		} 
	}

	return 0;
}

int clearLog()
{
	system("echo 0 > /root/progress.txt");

	return 0;
}


int main(int argc, char **argv)
{
	FILE *fp;
	SDL_Event event;
	char config_file[256] = {0};
	int ret = 0;
	int t = 0;
	
	// set different screen resolution for different machines
	chooseScreenResolution();
	
	findMountDir(config_file);
/*	// download config.txt from network
	else {
		chdir("/root/ndisk/");
		// set local IP
		// system("ifconfig eth0 up");
		// system("ifconfig eth0 192.168.1.100");
		ret = system("wget ftp://192.168.1.1/config.txt");
		chdir("-");
		if (ret != 0) {
			fprintf(stderr, "Can not download the config.txt file from server.\n");
			return -1;
		}
		sprintf(mountdir, "/root/ndisk/config.txt");	
		misc.recover_from = RECO_FROM_NETWORK;
		misc.network = 1;
	}
*/
//	misc.recover_from = 0;
	// start main body
	L = luaL_newstate();
	luaL_openlibs(L);
	registerFuncs4Lua();	

	// load local config file
	if (loadConfig(L, "./localcfg.lua") != 0) {		
		fprintf(stderr, "Can not find local config file\n");
		lua_close(L);
		return -1;
	}
	// load external config.txt file
	if (loadConfig(L, config_file) != 0) {		
		fprintf(stderr, "Can not find config.txt file in %s\n", config_file);
		lua_close(L);
		return -1;
	}
	
	if ( (fp = fopen("./autostart.txt", "r")) == NULL) {
		fprintf(stderr, "Warning! Can't open file autostart.txt.\n");
	}
	else {
		readAutoStartFile(fp);
 		fclose(fp);
	}

//    	STK_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER, SDL_SWSURFACE, scr.w, scr.h, 32);
    	STK_Init(SDL_INIT_VIDEO, SDL_SWSURFACE, scr.w, scr.h, 32);

    	STK_WindowNew(0, 0, scr.w, scr.h);
    	
	clearLog();
    	grec = (Recovery *)STK_Malloc(sizeof(Recovery));
    	grec->label_head = draw_label_head();
	grec->button_start = draw_button_start();
	grec->button_exit = draw_button_exit();
	grec->button_pro = draw_button_pro();
	grec->msgbox = draw_msgbox();
	grec->pb = draw_progressbar(&misc.progress);
	grec->label_status = draw_label_status();
	
	STK_WindowOpen();
	
//    	STK_Main();

	// clear the event's initial value to zero
	memset(&event, 0, sizeof(event));
	while (1) {
		while (SDL_WaitEvent(&event)) {
			if (STK_DispatchEvent(&event) != 0) {
				STK_Quit();
				free(grec);
				system("sleep 1 && reboot");
				return 0;
			}

			if (misc.autostart == 1) {
				SDL_Event e;
				e.button.type = SDL_MOUSEBUTTONDOWN;
				e.button.button = SDL_BUTTON_LEFT;
				e.button.state = SDL_PRESSED;
				// we mutate a click on button_start
				e.button.x = scr.w - 220 + 5;
				e.button.y = BORDER + 45 + 5;
				SDL_PushEvent(&e);
				
				misc.autostart = 2;
			}
		}
		
		// must here
		SDL_Delay(10);
	}

	free(grec);
	system("sleep 1 && reboot");
	
	return 0;
}
