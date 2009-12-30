#include "stk.h"
#include "lua.h"
#include "lauxlib.h"

#include "recover.h"

extern Recovery *grec;
extern Misc misc;
extern Screen scr;
extern const char *recoway[];
extern lua_State *L;

static int pt_flag = 1;

void cb_button_exit()
{
	SDL_Event event;
	
	lua_close(L);

	event.type = SDL_QUIT;
	event.user.code = 0;
	event.user.data1 = NULL;
	event.user.data2 = 0;
		
	SDL_PushEvent(&event);

}

int thread_bar(void *data)
{
	int ret;

	misc.running = 1;
	if (grec->button_start) {
		STK_WidgetClose((STK_Widget *)grec->button_start);
		grec->button_start = NULL;	
	}

	if (grec->button_exit) {
		STK_WidgetClose((STK_Widget *)grec->button_exit);
		grec->button_exit = NULL;	
	}

	if (grec->button_pro) {
		STK_WidgetClose((STK_Widget *)grec->button_pro);
		grec->button_pro = NULL;	
	}

//	ret = system("./bar /root/tmp/home.tar.gz | tar xzf - -C ttt");
	
	ret = loadFile(L, "recover.lua");
	printf("ret = %d \n", ret);

	// pass
	if (ret == 0) {
		printf("%s\n", "Revovery finish.");
		STK_MsgBoxAddMsg(grec->msgbox, "还原结束。");
		STK_LabelSetColor(grec->label_status, STK_COLOR_FOREGROUND, 0x40, 0x80, 0x20);		
		STK_LabelSetText(grec->label_status, "系统还原结束。请按‘退出’键重启");
	
	}
	// failed
	else {
		STK_MsgBoxAddMsg(grec->msgbox, "还原出错！");
		STK_LabelSetColor(grec->label_status, STK_COLOR_FOREGROUND, 0xff, 0x00, 0x00);
		STK_LabelSetText(grec->label_status, "  还原出错！                   ");
	}
	
	if (!grec->button_exit) {
		grec->button_exit = (STK_Button *)draw_button_exit();
	}
	
	pt_flag = 0;
	SDL_Delay(3000);
	if (misc.autostart == 2) {
		cb_button_exit();
	}
		
	return 0;
}

int thread_update(void *data)
{
	FILE *fprogress;
	char str[16];
	int ret;
	
	while (pt_flag || misc.progress <= 99) {
		
		if ((fprogress = fopen("/root/progress.txt", "r")) == NULL) {
			//printf("progress.txt doesn't exist.\n");
			//progress = 0;
		}
		else {	
			fscanf(fprogress, "%d", &misc.progress);
			fclose(fprogress);

			STK_ProgressBarSetValue(grec->pb, misc.progress);
			
			//sprintf(str, "%d%%", progress);
			//STK_MsgBoxAddMsg(grec->msgbox, str);
		}
		SDL_Delay(1000);
	}
	
	return 0;
}


int config_preprocess()
{
	int scheme[NSCHEME] = {0};
	char tmp[256];
	
	// make sure recovery way by latency
	if (grec->rg) {
		memcpy(scheme, grec->rg->rb_states, sizeof(int)*NSCHEME);
	}
	else {
		memcpy(scheme, misc.rb_states, sizeof(int)*NSCHEME);
	}
	
	if (misc.network) {
		misc.recover_from = RECO_FROM_NETWORK;
	}
	
	// ------------------------------------------------------
	// here, set lua's global configuration
	// ------------------------------------------------------
	// if whole recovery
	if (scheme[0]) {
		setTableBooleanElement(L, "Cfg", "whole_recover", 1);	
		setTableBooleanElement(L, "Cfg", "system_recover", 0);	
		setTableBooleanElement(L, "Cfg", "user_recover", 0);		
	}
	// if system recovery
	else if (scheme[1]) {
		setTableBooleanElement(L, "Cfg", "whole_recover", 0);	
		setTableBooleanElement(L, "Cfg", "system_recover", 1);	
		setTableBooleanElement(L, "Cfg", "user_recover", 0);		
	}
	// if user recovery
	else if (scheme[2]) {
		setTableBooleanElement(L, "Cfg", "whole_recover", 0);	
		setTableBooleanElement(L, "Cfg", "system_recover", 0);	
		setTableBooleanElement(L, "Cfg", "user_recover", 1);		
	}
	
	switch (misc.recover_from) {
	case RECO_FROM_UDISK:
		setTableBooleanElement(L, "Cfg", "reco_U", 1);	
		setTableBooleanElement(L, "Cfg", "reco_D", 0);	
		setTableBooleanElement(L, "Cfg", "reco_N", 0);		
		break;

	case RECO_FROM_LDISK:
		setTableBooleanElement(L, "Cfg", "reco_U", 0);	
		setTableBooleanElement(L, "Cfg", "reco_D", 1);	
		setTableBooleanElement(L, "Cfg", "reco_N", 0);		
		break;

	case RECO_FROM_NETWORK:
		setTableBooleanElement(L, "Cfg", "reco_U", 0);	
		setTableBooleanElement(L, "Cfg", "reco_D", 0);	
		setTableBooleanElement(L, "Cfg", "reco_N", 1);		
		break;
	}

	return 0;
}



void cb_button_start()
{
	SDL_Thread *thread;
	
	config_preprocess();
	
	STK_MsgBoxAddMsg(grec->msgbox, "现在开始还原。");
	STK_LabelSetText(grec->label_status, "系统正在还原中……             ");

        if ((thread = SDL_CreateThread(thread_bar, NULL)) == NULL ) {
	        fprintf(stderr, "Can't create thread: %s \n", SDL_GetError());
	        return;
        }

        if ((thread = SDL_CreateThread(thread_update, NULL)) == NULL ) {
	        fprintf(stderr, "Can't create thread: %s \n", SDL_GetError());
	        return;
        }
}

void cb_button_pro()
{
	if (!grec->rg || !grec->rb) {
		STK_MsgBoxSetSize(grec->msgbox, scr.w - 2*BORDER - 175, scr.h - 2*BORDER - 210);
		STK_WindowEventRedraw();
		grec->rg = draw_radiogroup(misc.rb_states, NSCHEME);
		grec->rb = draw_radiobutton(&misc.network);
	}
	else {
		memcpy(misc.rb_states, grec->rg->rb_states, (sizeof(int)*NSCHEME));
		STK_WidgetClose((STK_Widget *)grec->rg);
		grec->rg = NULL;
		
		misc.network = grec->rb->state;	
		STK_WidgetClose((STK_Widget *)grec->rb);
		grec->rb = NULL;	
		STK_MsgBoxSetSize(grec->msgbox, scr.w - 2*BORDER, scr.h - 2*BORDER - 210);
	}

}

void cb_rb_network()
{
	if (!misc.running) {
		// here is correct, this is the conflict between callback and redraw's order
		if (grec->rb->state == STK_RADIOBUTTON_RELEASE) {
			// printf("%d\n", grec->rb->state);
			misc.network = 1;
			STK_LabelSetText(grec->label_status, (char *)recoway[RECO_FROM_NETWORK]);
		}
		else {
			// printf("%d\n", grec->rb->state);
			misc.network = 0;
			STK_LabelSetText(grec->label_status, (char *)recoway[misc.recover_from]);		
		}
	}
}

