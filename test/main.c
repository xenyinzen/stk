
#include "stk.h"
#include "stk_color.h"
#include "stk_label.h"
#include "stk_button.h"
#include "stk_slidebar.h"
#include "stk_entry.h"
#include "stk_radiobutton.h"
#include "stk_radiogroup.h"
#include "stk_progressbar.h"
#include "stk_msgbox.h"

STK_Widget *draw_label()
{
	
	STK_Label *label = (STK_Label *)STK_LabelNew("I am Tang", 400, 100);
//	STK_WidgetSetDims((STK_Widget *)label, 100, 100, 200, 100);
	STK_LabelSetAlignment(label, STK_LABEL_CENTER);
	STK_LabelSetColor(label, STK_COLOR_BACKGROUND, 0x80, 0x00, 0x80);
	STK_WidgetEventShow((STK_Widget *)label);
	
	return (STK_Widget *)label;
}

STK_Widget *draw_button()
{
	STK_Button *button = (STK_Button *)STK_ButtonNew("点我", 200, 240, 60, 40);
//	STK_WindowAddWidget((STK_Widget *)button);
	STK_WidgetEventShow((STK_Widget *)button);
	return (STK_Widget *)button;
}

STK_Widget *draw_slidebar()
{
	STK_Slidebar *slider = (STK_Slidebar *)STK_SlidebarNew(50, 30, 30, 200);
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
	STK_RadioButton *rbutton = (STK_RadioButton *)STK_RadioButtonNew("I", 300, 300, 200, 30);
	STK_WidgetEventShow((STK_Widget *)rbutton);
	return (STK_Widget *)rbutton;

}

STK_Widget *draw_msgbox()
{
	STK_MsgBox *msgbox = (STK_MsgBox *)STK_MsgBoxNew(20, 300, 250, 150, "first line");
	STK_WidgetEventShow((STK_Widget *)msgbox);
	
	STK_MsgBoxAddMsg(msgbox, "The second line");
	STK_MsgBoxAddMsg(msgbox, "The 3rd line");
//	STK_MsgBoxAddMsg(msgbox, "The 4th line");
//	STK_MsgBoxAddMsg(msgbox, "The 5th line");
//	STK_MsgBoxAddMsg(msgbox, "The 6th line");
	STK_MsgBoxAddMsg(msgbox, "The 7th line");
	STK_MsgBoxAddMsg(msgbox, "The 8th line");
	
	
	return (STK_Widget *)msgbox;
}

char *str[] = {"aaaaaaaaaaaaaaaaaaaaaaaa", "BBBbbbbbbbbbbbbb", "CDdddddddddddddddd", "EFGHfffffffffffffffffffffff" };

STK_Widget *draw_radiogroup()
{
	STK_RadioGroup *rg = (STK_RadioGroup *)STK_RadioGroupNew(300, 350, 200, 100, str, sizeof(str)/sizeof(char *));
	STK_WidgetEventShow((STK_Widget *)rg);
	
	return (STK_Widget *)rg;
}

STK_Widget *draw_progressbar(Uint32 *p)
{
	STK_ProgressBar *pb = (STK_ProgressBar *)STK_ProgressBarNew(400, 20, 200, 50, p);
	STK_WidgetEventShow((STK_Widget *)pb);
	
	return (STK_Widget *)pb;
}


int main(int argc,char **argv)
{

    	SDL_Surface *video;
    	STK_Widget  *button;
	STK_MsgBox *msgbox;
	
    	Uint32 value = 10;
    	
    	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);

    	video=SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
	if ( video ==  NULL ) {
		fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n",
							SDL_GetError());
		SDL_Quit();
	}
    	atexit(SDL_Quit);
    
    	STK_Init();

    	STK_WindowNew(0, 0, 640, 480);
    	
    	STK_Widget *label = draw_label();
    	printf("Label: %x\n", label);
    	
    	draw_button();
    	draw_slidebar();
    	draw_entry();
    	draw_radiobutton();
    	draw_radiogroup();
	draw_progressbar(&value);
	msgbox = (STK_MsgBox *)draw_msgbox();
	STK_MsgBoxAddMsg(msgbox, "This 9th string.");
 		    	
    	STK_WindowOpen();
    	
//    	SDL_Delay(5000);
    	
//    	STK_WidgetClose(label);
//   	SDL_Delay(5000);
    	
//    	label = draw_label();
//    	printf("Label: %x\n", label);

    	STK_Main();
}
