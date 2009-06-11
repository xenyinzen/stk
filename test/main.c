

#include "stk.h"
#include "stk_label.h"

STK_Widget *draw_label()
{
	
	STK_Label *label = (STK_Label *)stk_label_new("我爱你中国", 100, 200);
	stk_widget_setDims((STK_Widget *)label, 100, 100, 200, 100);
	stk_label_setAlignment((STK_Widget *)label, CENTER);
	stk_label_setColor((STK_Widget *)label, BACKGROUND, 0x00800080);
	stk_widget_EventShow((STK_Widget *)label);
	
	return (STK_Widget *)label;
}




int main(int argc,char **argv)
{

    	SDL_Surface *video;
    	STK_Widget  *button;

    	SDL_Init(SDL_INIT_VIDEO);

    	video=SDL_SetVideoMode(600, 480, 32, SDL_HWSURFACE);
	if ( video == NULL ) {
		fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n",
							SDL_GetError());
		SDL_Quit();
	}
    	atexit(SDL_Quit);
    
    	stk_init();

    	stk_window_new(0, 0, 600, 480);
    	
    	STK_Widget *label = draw_label();
    	printf("Label: %x\n", label);
    	
    	stk_window_open();
    	SDL_Delay(5000);
    	
    	stk_widget_close(label);
   	SDL_Delay(5000);
    	
    	label = draw_label();
    	printf("Label: %x\n", label);

    	stk_main();
}
