#ifndef _STK_H_
#define _STK_H_

#include "SDL.h"
#include "stk_color.h"
#include "stk_font.h"
#include "stk_label.h"
#include "stk_button.h"
#include "stk_slidebar.h"
#include "stk_entry.h"
#include "stk_radiobutton.h"
#include "stk_radiogroup.h"
#include "stk_progressbar.h"
#include "stk_msgbox.h"

int STK_Init();
int STK_Main();
int STK_DispatchEvent(SDL_Event *event);
int STK_Quit();


#endif /* _STK_H_ */