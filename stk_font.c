#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_ttf.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_font.h"

#define STK_FONT_SIZENUM 4
#define STK_FONT_MAXSIZE 36

STK_Font font0[STK_FONT_SIZENUM] = { 0 };
STK_Font font1[STK_FONT_SIZENUM] = { 0 };

char *default_font = "msyh.ttf";
Uint32 default_fontsize_index = 1;
Uint32 fontsize_array[STK_FONT_SIZENUM] = {12, 16, 24, 36};

static void cleanup(TTF_Font *font, int exitcode)
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

	// after loading, font is open and referenced by font0, and default_fontsize
	STK_FontLoadFontSizes(default_font);
		
	return 0;
}

STK_Font *STK_FontLoad(char *name, Uint32 size)
{
	TTF_Font *font;
	STK_Font *stkfont;
	
	if (!name || (size >= STK_FONT_MAXSIZE))
		return -1;

	font = TTF_OpenFont(name, size);
	if ( font == NULL ) {
		fprintf(stderr, "Couldn't load %d pt font from %s: %s\n",
					size, name, SDL_GetError());
		cleanup(font, -1);
	}
	
	// create new STK_Font structure
	stkfont = (STK_Font *)STK_Malloc(sizeof(STK_Font));
	stkfont->name = name;
	stkfont->size = size;
	stkfont->font = font;
	stkfont->font_height = TTF_FontHeight(font);
	
	return stkfont;	
}

// default font is stored in font0
int STK_FontLoadFontSizes(char *name)
{
	int i;
	TTF_Font *font;
	
	for (i=0; i<STK_FONT_SIZENUM; i++) {
		font = TTF_OpenFont(name, fontsize_array[i]);
		if ( font == NULL ) {
			fprintf(stderr, "Couldn't load %d pt font from %s: %s\n",
						fontsize_array[i], name, SDL_GetError());
			cleanup(font, -1);
		}
		
		font0[i].name = name;
		font0[i].size = fontsize_array[i];
		font0[i].font = font;
		font0[i].font_height = TTF_FontHeight(font);	// calculate the height of one font
	}
	
	return 0;
}


STK_Font *STK_FontGetDefaultFont(Uint32 num)
{
	if (num > STK_FONT_SIZENUM)
		return NULL;
	
	return &font0[num];
}

STK_Font *STK_FontGetDefaultFontWithSize()
{
	return &font0[default_fontsize_index];
}

int STK_FontGetHeight(STK_Font *font)
{
	if (!font)
		return -1;
	return font->font_height;
}

// here, rect is the relative rectagle location against to the widget
int STK_FontDraw(STK_Font *font, char *str, STK_Widget *widget, SDL_Rect *rect, SDL_Color *fg, SDL_Color *bg)
{
	SDL_Surface *text;
	SDL_Rect src, dst;
	Uint32 colorkey_bg;

	STK_Window *win = STK_WindowGetTop();
	if (!win)
		return -1;
	if (!str || !strcmp(str, ""))
		return -1;
	
	text = TTF_RenderUTF8_Shaded(font->font, str, *fg, *bg);
//	text = TTF_RenderUTF8_Solid(font->font, str, *fg);
	
	dst.x = rect->x;
	dst.y = rect->y;
	// here, setting dst.w = rect->w means use widget's border as limits
	dst.w = (text->w <= rect->w ? text->w: rect->w); 
	dst.h = (text->h <= rect->h ? text->h: rect->h); 
	
	// set the background of label being transparent, magic num is for test only
	colorkey_bg = SDL_MapRGB(text->format, bg->r, bg->g, bg->b);
	SDL_SetColorKey(text, SDL_SRCCOLORKEY, colorkey_bg);
	
	src.x = 0;
	src.y = 0;
	src.w = dst.w;
	src.h = dst.h;
			
	SDL_BlitSurface(text, &src, widget->surface, &dst);
	SDL_FreeSurface(text);

	return 0;
}

int STK_FontAdapter(STK_Font *font, SDL_Rect *rect, char *str)
{
	int w = 0;
	int h = 0;
	
	if (!str || !strcmp(str, ""))
		return -1;
	
	TTF_SizeUTF8(font->font, str, &w, &h);
	if (!rect->w || rect->w != w ) {
		rect->w = w;
	}
	
	if (!rect->h || rect->h != h) {
		rect->h = h;
	}
	
	return 0;
}
