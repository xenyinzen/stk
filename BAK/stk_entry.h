#ifndef _STK_ENTRY_H_
#define _STK_ENTRY_H_

//
typedef struct STK_Entry {
	STK_Widget  widget;
//	SDL_Font   *Font;
	STK_Text text;
	SDL_Rect textarea;
	
	Uint32 cur_pos;		// current char position in the output frame
	Uint32 start_char;	// the start char address in the char buffer
	Uint32 end_char;	// then end char address in the char buffer
	
	Uint32 distance;	// the relative position of char rect on drawing start
	Uint32 charskip;	// the dividing space between two chars in a line
	SDL_Rect text_rect;	// current char rectangle area 
	
	Uint32 fgcolor;
	Uint32 bgcolor;
	Uint32 cursor_color;
	
	// used to record char's width map
	Uint16 charwidth[255];
		
} STK_Entry;



#endif /* _STK_ENTRY_H_ */
