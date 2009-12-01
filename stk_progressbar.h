#ifndef _STK_PROGRESSBAR_H_
#define _STK_PROGRESSBAR_H_

#include "SDL.h"


typedef struct STK_ProgressBar {
	STK_Widget widget;
	
	Uint32 value;			// the current progress value
	Uint32 *pvalue;			// the pointer of the binding variable

	SDL_TimerID timer;		// the timer id
} STK_ProgressBar;

STK_ProgressBar *STK_ProgressBarNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint32 *pvalue);
void STK_ProgressBarDraw(STK_Widget *widget);
void STK_ProgressBarClose(STK_Widget *widget);
int STK_ProgressBarRegisterType();

#endif /* _STK_PROGRESSBAR_H_ */
