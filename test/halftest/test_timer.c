#include "SDL.h"

Uint32 print(Uint32 interval, void *parm)
{
	static i = 0;
	printf("I am Tang Gang: %d.\n", i++);
	return 0;
}


int main(int argc,char **argv)
{
        SDL_Surface *video;
	SDL_TimerID id;

        SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
        video=SDL_SetVideoMode(600, 480, 32, SDL_HWSURFACE);
        atexit(SDL_Quit);

        SDL_FillRect(video, NULL, 0x000000ff);
        SDL_UpdateRect(video, 0, 0, 0, 0);

	id = SDL_AddTimer(1000, print, 0);
	
	while (1) 
	{}	

        return 0;
}
