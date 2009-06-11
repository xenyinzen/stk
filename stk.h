#ifndef _STK_H_
#define _STK_H_

#include "SDL.h"

int stk_init();
int stk_main();
int stk_quit();

enum {
	STK_EVENT = SDL_USEREVENT
};


#endif /* _STK_H_ */