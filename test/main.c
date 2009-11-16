
#include "stk.h"
#include "stk_label.h"
#include "stk_button.h"
#include "stk_slidebar.h"
#include "stk_entry.h"
#include "stk_radiobutton.h"
#include "stk_radiogroup.h"

STK_Widget *draw_label()
{
	
	STK_Label *label = (STK_Label *)STK_LabelNew("我爱你中国，我爱你家乡！", 400, 100);
//	STK_WidgetSetDims((STK_Widget *)label, 100, 100, 200, 100);
	STK_LabelSetAlignment((STK_Widget *)label, CENTER);
	STK_LabelSetColor((STK_Widget *)label, BACKGROUND, 0x00800080);
	STK_WidgetEventShow((STK_Widget *)label);
	
	return (STK_Widget *)label;
}

STK_Widget *draw_button()
{
	STK_Button *button = (STK_Button *)STK_ButtonNew("请点我哈哈哈哈哈", 200, 240, 100, 30);
//	STK_WindowAddWidget((STK_Widget *)button);
	STK_WidgetEventShow((STK_Widget *)button);
	return (STK_Widget *)button;
}

STK_Widget *draw_slidebar()
{
	STK_Slidebar *slider = (STK_Slidebar *)STK_SlidebarNew(50, 30, 30, 400);
	STK_WidgetEventShow((STK_Widget *)slider);
	return (STK_Widget *)slider;

}

STK_Widget *draw_entry()
{
	STK_Entry *entry = (STK_Entry *)STK_EntryNew(150, 30, 200, 50, 0, "hehe");
	STK_WidgetEventShow((STK_Widget *)entry);
	return (STK_Widget *)entry;

}

STK_Widget *draw_radiobutton()
{
	STK_RadioButton *rbutton = (STK_RadioButton *)STK_RadioButtonNew("I am Tang Gang", 300, 300, 200, 30);
	STK_WidgetEventShow((STK_Widget *)rbutton);
	return (STK_Widget *)rbutton;

}

char *str[] = {"aaaaaaaaaaaaaaaaaaaaaaaa", "BBBbbbbbbbbbbbbb", "CDdddddddddddddddd", "EFGHfffffffffffffffffffffff" };

STK_Widget *draw_radiogroup()
{
	STK_RadioGroup *rg = (STK_RadioGroup *)STK_RadioGroupNew(300, 350, 200, 100, str, sizeof(str)/sizeof(char *));
	STK_WidgetEventShow((STK_Widget *)rg);
	
	return (STK_Widget *)rg;
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
    	
    	draw_button();
    	draw_slidebar();
    	draw_entry();
    	draw_radiobutton();
    	draw_radiogroup();
    	
    	STK_WindowOpen();
    	
//    	SDL_Delay(5000);
    	
//    	STK_WidgetClose(label);
//   	SDL_Delay(5000);
    	
//    	label = draw_label();
//    	printf("Label: %x\n", label);

    	STK_Main();
}
