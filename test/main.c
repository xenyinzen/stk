

#include "stk.h"
#include "stk_label.h"

STK_Widget *draw_label()
{
	
	STK_Label *label = (STK_Label *)STK_LabelNew("我爱你中国", 100, 200);
	STK_WidgetSetDims((STK_Widget *)label, 100, 100, 200, 100);
	STK_LabelSetAlignment((STK_Widget *)label, CENTER);
	STK_LabelSetColor((STK_Widget *)label, BACKGROUND, 0x00800080);
	STK_WidgetEventShow((STK_Widget *)label);
	
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
    
    	STK_Init();

    	STK_WindowNew(0, 0, 600, 480);
    	
    	STK_Widget *label = draw_label();
    	printf("Label: %x\n", label);
    	
    	STK_WindowOpen();
    	SDL_Delay(5000);
    	
    	STK_WidgetClose(label);
   	SDL_Delay(5000);
    	
    	label = draw_label();
    	printf("Label: %x\n", label);

    	STK_Main();
}
