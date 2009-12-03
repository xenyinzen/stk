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


#endif
