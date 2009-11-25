#ifndef _STK_RADIOGROUP_H_
#define _STK_RADIOGROUP_H_

#include "SDL.h"
#include "stk_radiobutton.h"

enum STK_RadioGroupMode {
	STK_RADIOGROUP_MODE_MONO,
	STK_RADIOGROUP_MODE_MULT
};

typedef struct STK_RadioButtonListNode {
	STK_RadioButton *rb;
	Uint32 i;
	struct STK_RadioButtonListNode *next;
} STK_RadioButtonListNode;


typedef struct STK_RadioGroup {
	STK_Widget widget;
	STK_RadioButtonListNode *rblist_head;	// point to the head of radio button array
	
	char **name;			// the pointer to radio buttons' names
	Uint32 n;			// the number of radio buttons
	Uint32 pos;			// current position on radio arrays

	Uint32 item_height;		// each radio button's height
	Uint32 interval;		// the interval between radio buttons
	Uint32 nchoice;			// record how many have chose
	
	Uint32 mode;			// flag: mononical or multiple choice

} STK_RadioGroup;

STK_RadioGroup *STK_RadioGroupNew(char *radiostr[], int num, Uint16 x, Uint16 y, Uint16 w, Uint16 h);
void STK_RadioGroupDraw(STK_Widget *widget);
void STK_RadioGroupClose(STK_Widget *widget);
int STK_RadioGroupRegisterType();

#endif /* _STK_RADIOGROUP_H_ */
