#include "SDL.h"

Uint32 print(Uint32 interval, void *parm)
{
	static i = 0;
	printf("I am Tang Gang: %d.\n", i++);
/*	
	SDL_Event event;
	SDL_UserEvent uevent;
	
	uevent.type = SDL_USEREVENT;
	uevent.code = 0;
	uevent.data1 = NULL;
	uevent.data2 = NULL;
	
	event.type = SDL_USEREVENT;
	event.user = uevent;
	
	SDL_PushEvent(&event);
	
*/	
	return 1000;
}


int main(int argc,char **argv)
{
        SDL_Surface *video;
	SDL_TimerID id;
	SDL_Event event;

        SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
        video=SDL_SetVideoMode(600, 480, 32, SDL_SWSURFACE);
        atexit(SDL_Quit);

        SDL_FillRect(video, NULL, 0x000000ff);
        SDL_UpdateRect(video, 0, 0, 0, 0);

	id = SDL_AddTimer(1000, print, NULL);
	
	while (SDL_WaitEvent(&event)) {
		switch(event.type) {
			case SDL_USEREVENT: {
				printf("I am here, haha\n");
				break;
			}
			
		}	
	}	

        return 0;
}
