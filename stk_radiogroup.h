#ifndef _STK_RADIOGROUP_H_
#define _STK_RADIOGROUP_H_

#include "SDL.h"
#include "stk_radiobutton.h"

typedef struct STK_RadioButtonListNode {
	STK_RadioButton *rb;
	struct STK_RadioButtonListNode *next;
} STK_RadioButtonListNode;


typedef struct STK_RadioGroup {
	STK_Widget widget;
	STK_RadioButtonListNode *rblist_head;	// point to the head of radio button array
	
	char **name;			// the pointer to radio buttons' names
	Uint32 n;			// the number of radio buttons
	Uint32 pos;			// current position on radio arrays

	Uint32 border;			// border thickness 
	Uint32 interval;		// the interval between radio buttons
	Uint32 nchoice;			// record how many have chose
	
	int mono;			// flag: mononical or multiple choice
	int border_on;			// flag: if have border displayed?
	int fixed;			// flag: fixed or extended?

} STK_RadioGroup;

STK_Widget *STK_RadioGroupNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, char *radiostr[], int num);
void STK_RadioGroupDraw(STK_Widget *widget);
void STK_RadioGroupClose(STK_Widget *widget);
int STK_RadioGroupRegisterType();

#endif /* _STK_RADIOGROUP_H_ */
