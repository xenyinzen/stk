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
	// TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
		
	return 1;
}


int STK_FontDraw( char *str, SDL_Rect *rect, SDL_Color *fg, SDL_Color *bg)
{
	SDL_Surface *text, *video;
	SDL_Rect dst;

	STK_Window *win = STK_WindowGetTop();
	if (!win)
		return 0;
	
	text = TTF_RenderUTF8_Shaded(font, str, *fg, *bg);
	
	dst.x = rect->x + win->widget.rect.x;
	dst.y = rect->y + win->widget.rect.y;
	dst.w = text->w;
	dst.h = text->h;
	video = SDL_GetVideoSurface();
	SDL_BlitSurface(text, NULL, video, &dst);
	SDL_FreeSurface(text);

	return 1;
}

int STK_FontAdapter( SDL_Rect *rect, char *str)
{
	int w, h = 0;
	TTF_SizeUTF8(font, str, &w, &h);
	if (!rect->w || rect->w < w ) {
		rect->w = w;
	}
	
	if (!rect->h || rect->h < h) {
		rect->h = h;
	}
	
	return 1;
}
