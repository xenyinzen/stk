

#include "stk.h"

void draw_label()
{
	
	STK_Label *label = (STK_Label *)stk_label_new();
	stk_widget_setDims((STK_Widget *)label, 100, 100, 100, 50);
	stk_signal_connect((STK_Widget *)label, "show", (F_Signal_Callback)stk_label_draw, NULL);
	stk_widget_EventShow((STK_Widget *)label);
}




int main(int argc,char **argv)
{

    	SDL_Surface *video;
    	STK_Widget  *button;

    	SDL_Init(SDL_INIT_VIDEO);
    	video=SDL_SetVideoMode(600, 480, 32, SDL_SWSURFACE);
    	atexit(SDL_Quit);
    
    	stk_init();

    	stk_window_new(0, 0, 600, 480);
    	
    	draw_label();
    	
    	stk_window_open();
    

    	stk_Main();
}
