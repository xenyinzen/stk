#ifndef _RECOVER_H_
#define _RECOVER_H_


#define BORDER	5
#define NSCHEME 3
#define NRECOWAY 3

typedef struct Recovery {
	STK_Label *label_head;
	STK_Label *label_status;
	STK_Button *button_start;
	STK_Button *button_exit;
	STK_Button *button_pro;
	STK_MsgBox *msgbox;
	STK_RadioGroup *rg;
	STK_RadioButton *rb;
	STK_ProgressBar *pb;
} Recovery;

typedef struct Screen {
	int w;
	int h;
} Screen;

typedef struct Misc {
	int running;
	int progress;
	int recover_from;
	int recover_scheme;
	int rb_states[NSCHEME];	
	int network;
	int autostart;
} Misc;

enum {
	RECO_FROM_UDISK,
	RECO_FROM_LDISK,
	RECO_FROM_NETWORK
};


STK_Label *draw_label_head();
STK_Label *draw_label_status();
STK_Button *draw_button_start();
STK_Button *draw_button_exit();
STK_Button *draw_button_pro();
STK_RadioGroup *draw_radiogroup(int rb_states[], int n);
STK_RadioButton *draw_radiobutton();
STK_MsgBox *draw_msgbox();
STK_ProgressBar *draw_progressbar(Uint32 *p);

#endif
