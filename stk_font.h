#ifndef _STK_FONT_H_
#define _STK_FONT_H_

int STK_FontInit();
int STK_FontDraw(STK_Widget *widget, char *str, SDL_Rect *rect, SDL_Color *fg, SDL_Color *bg);
int STK_FontAdapter( SDL_Rect *rect, char *str);





#endif /* _STK_FONT_H_ */