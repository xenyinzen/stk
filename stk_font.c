#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_ttf.h"
#include "stk_widget.h"
#include "stk_window.h"

#define FONT_SIZE	16
#define FONT_FILE	"msyh.ttf"


TTF_Font *font;

static void cleanup(int exitcode)
{
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	exit(exitcode);
}

int STK_FontInit()
{
	
	/* Initialize the TTF library */
	if ( TTF_Init() < 0 ) {
		fprintf(stderr, "Couldn't initialize TTF: %s\n", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	font = TTF_OpenFont(FONT_FILE, FONT_SIZE);
	if ( font == NULL ) {
		fprintf(stderr, "Couldn't load %d pt font from %s: %s\n",
					FONT_SIZE, FONT_FILE, SDL_GetError());
		cleanup(-1);
	}
		
	return 0;
}

// here, rect is the relative rectagle location against to the widget
int STK_FontDraw(STK_Widget *widget, char *str, SDL_Rect *rect, SDL_Color *fg, SDL_Color *bg)
{
	SDL_Surface *text;
	SDL_Rect dst;

	STK_Window *win = STK_WindowGetTop();
	if (!win)
		return 1;
	if (!str || !strcmp(str, ""))
		return 1;
	
	text = TTF_RenderUTF8_Shaded(font, str, *fg, *bg);
	
	dst.x = rect->x;
	dst.y = rect->y;
	dst.w = text->w;   // here or rect->w
	dst.h = text->h;   // here or rect->h
	
	SDL_BlitSurface(text, NULL, widget->surface, &dst);
	SDL_FreeSurface(text);

	return 0;
}

int STK_FontAdapter( SDL_Rect *rect, char *str)
{
	int w, h = 0;
	
	if (!str || !strcmp(str, ""))
		return 1;
	
	TTF_SizeUTF8(font, str, &w, &h);
	if (!rect->w || rect->w < w ) {
		rect->w = w;
	}
	
	if (!rect->h || rect->h < h) {
		rect->h = h;
	}
	
	return 0;
}

int STK_FontGetHeight()
{
	return TTF_FontHeight(font);
}
