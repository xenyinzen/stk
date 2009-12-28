#ifndef _RECOVER_H_
#define _RECOVER_H_

typedef struct Recovery {
	STK_Label *label_head;
	STK_Label *label_status;
	STK_Button *button_start;
	STK_Button *button_exit;
	STK_MsgBox *msgbox;
	STK_ProgressBar *pb;
} Recovery;

typedef struct Screen {
	int w;
	int h;
} Screen;

typedef struct Misc {
	int progress;
	int recover_from;
	int rb_states[3];	
	int network;
} Misc;

enum {
	RECO_FROM_UDISK,
	RECO_FROM_LDISK,
	RECO_FROM_NETWORK
};

#endif
