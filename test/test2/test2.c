

#include "SDL.h"


int main(int argc,char **argv)
{

    	SDL_Surface *video;

    	SDL_Init(SDL_INIT_VIDEO);
    	video=SDL_SetVideoMode(600, 480, 32, SDL_HWSURFACE);
    	atexit(SDL_Quit);
	
	SDL_FillRect(video, NULL, 0x000000ff);
	SDL_UpdateRect(video, 0, 0, 0, 0);    

	SDL_Delay(5000);
	
	return 0;
}
