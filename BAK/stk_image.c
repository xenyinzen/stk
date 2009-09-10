#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_image.h"


Uint32 g_image_dividing_horizontal[2][2][2] = {
	{	// pattern 1
		{ 0x0000ff00, 0x00ff0000 }, // area 1, topcenter, two pixels
		{ 0x000000ff, 0x0000ff00 }, // area 3, bottomcenter, two pixels
	},
	{	// pattern 2
		{ 0x0000ff00, 0x00ff0000 }, // area 1, topcenter, two pixels
		{ 0x000000ff, 0x0000ff00 }, // area 3, bottomcenter, two pixels
	}
};

Uint32 g_image_dividing_vertical[2][2][2] = {
	{	// pattern 1
		{ 0x0000ff00, 0x00ff0000 }, // area 2, rightcenter, two pixels
		{ 0x0000ff00, 0x00ff0000 }  // area 4, leftcenter, two pixels
	},
	{	// pattern 2
		{ 0x0000ff00, 0x00ff0000 }, // area 2, rightcenter, two pixels
		{ 0x0000ff00, 0x00ff0000 }  // area 4, leftcenter, two pixels
	}
};

Uint32 g_image_dividing_matrix[2][4][4] = {
	{	// pattern 1
		{ 0x00ffff77, 0x00ff7777, 0x00ff0088, 0x00ffffff }, // area 5, topleft, 2x2 pixels
		{ 0x0000ff77, 0x00ff5566, 0x00ff0080, 0x00ff8080 }, // area 6, topright, 2x2 pixels
		{ 0x00ff8080, 0x00ffffff, 0x00808080, 0x00404040 }, // area 7, bottomright, 2x2 pixels
		{ 0x0000ff00, 0x00ffff00, 0x00008080, 0x00ff7799 }  // area 8, bottomleft, 2x2 pixels
	},
	{	// pattern 2
		{ 0x00ffff77, 0x00ff7777, 0x00ff0088, 0x00ffffff }, // area 5, topleft, 2x2 pixels
		{ 0x0000ff77, 0x00ff5566, 0x00ff0080, 0x00ff8080 }, // area 6, topright, 2x2 pixels
		{ 0x00ff8080, 0x00ffffff, 0x00808080, 0x00404040 }, // area 7, bottomright, 2x2 pixels
		{ 0x0000ff00, 0x00ffff00, 0x00008080, 0x00ff7799 }  // area 8, bottomleft, 2x2 pixels
	}
};


// here, pattern need to vertify furtherly
SDL_Surface *STK_ImageFillRect(SDL_Widget *widget, SDL_Image *image, SDL_Rect *rect, Uint32 pattern, Uint32 n)
{
	SDL_Surface *surface = widget->surface;
	Uint32 bgcolor = widget->bgcolor;
	int style = image->fillstyle;
	int i = 0;
	SDL_Rect r;
	
	switch (style) {
	case STK_IMAGESTYLE_NORMAL:
		if (pattern == 0)
			SDL_FillRect(surface, NULL, bgcolor);
		else if (pattern == 1)
			SDL_FillRect(surface, NULL, 0x00808080);
		break;
	case STK_IMAGESTYLE_HORIZONTAL:
		for (i = 0; i < rect.h ; i++) {
			r.x = rect->x;
			r.y = rect->y + i;
			r.w = rect->w;
			r.h = 1;
			SDL_FillRect(surface, &r, g_image_dividing_horizontal[pattern][n][i]);
		}
		break;
	case STK_IMAGESTYLE_VERTICAL:
		for (i = 0; i < rect.w; i++) {
			r.x = rect->x + i;
			r.y = rect->y;
			r.w = 1;
			r.h = rect->h;
			SDL_FileRect(surface, &r, g_image_dividing_vertical[pattern][n][i]);
		}
		break;
	case STK_IMAGESTYLE_MATRIX: 
		SDL_Surface *s;
		s = SDL_CreateRGBSurfaceFrom((void *)g_image_dividing_matrix[pattern][n], 
						2, 
						2, 
						32,
						8,
						0x00ff0000,
						0x0000ff00,
						0x000000ff,
						0xff000000);
		SDL_BlitSurface(s, NULL, surface, rect);
		break;
	case STK_IMAGESTYLE_PICTURE:
		SDL_Surface *s;
		SDL_Rect r;
		r.x = 0; 
		r.y = 0;
		r.w = rect->w;
		r.h = rect->h;
		s = SDL_LoadBMP(image->filename);
		// Attention: the size of the picture should match the desired size
		if (s) 
			SDL_BlitSurface(s, &r, surface, rect);			
		break;
	default:
		break;

	}
	
}
