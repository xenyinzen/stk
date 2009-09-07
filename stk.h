#ifndef _STK_H_
#define _STK_H_

#include "SDL.h"

int STK_Init();
int STK_Main();
int STK_Quit();

enum {
	STK_EVENT = SDL_USEREVENT
};


#endif /* _STK_H_ */