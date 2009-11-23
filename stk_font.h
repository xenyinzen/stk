#ifndef _STK_FONT_H_
#define _STK_FONT_H_

#include "SDL.h"
#include "SDL_ttf.h"

typedef struct STK_Font {
	char *name;
	TTF_Font *font;
	Uint32 size;
	Uint32 font_height;

} STK_Font;


int STK_FontInit();
STK_Font *STK_FontLoad(char *name, Uint32 num);
int STK_FontLoadFontSizes();

int STK_FontDraw(STK_Font *font, char *str, STK_Widget *widget, SDL_Rect *rect, SDL_Color *fg, SDL_Color *bg);
int STK_FontAdapter(STK_Font *font, SDL_Rect *rect, char *str);





#endif /* _STK_FONT_H_ */