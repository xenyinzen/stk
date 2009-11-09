#ifndef _STK_ENTRY_H_
#define _STK_ENTRY_H_

#include "stk_text.h"

//
typedef struct STK_Entry { 
	STK_Widget widget;
//	SDL_Font   *Font;
	STK_Text *text;
	SDL_Rect textarea;
//	SDL_Rect cur_text_rect;		// current char rectangle area 
	
//	Uint32 cur_pos;			// current char position in the output box
//	Uint32 start_char;		// the start char address in the char buffer
//	Uint32 end_char;		// then end char address in the char buffer
//	Uint32 start_redraw;		// redraw start position
//	Uint32 end_redraw;		// redraw end postion
	
	Uint32 cursor_pos;		// current cursor position (pixel value)
	Uint32 cursor_height;
//	Uint32 cursor_pos_old;		// the old cursor position (pixel value)
//	Uint32 distance;		// the relative position of char rect on drawing start
//	Uint32 charskip;		// the dividing space between two chars in a line
	
	Uint32 fgcolor;
	Uint32 bgcolor;
	Uint32 cursor_color;
	
	Uint16 charwidth[255];		// used to record char's width map
	Uint16 inputkey[4];		// used to store input char at every input
			
} STK_Entry;


STK_Widget *STK_EntryNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint16 limited_num, char *initial_str);
int STK_EntryRegisterType();
void STK_EntryDraw(STK_Widget *widget);
void STK_EntryClose(STK_Widget *widget);


#endif /* _STK_ENTRY_H_ */

