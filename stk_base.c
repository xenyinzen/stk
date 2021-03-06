#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "stk_base.h"



// copy B to A
int STK_BaseRectCopy(SDL_Rect *rect_A, SDL_Rect *rect_B)
{
	if (!rect_A || !rect_B)
		return -1;
	
	rect_A->x = rect_B->x;
	rect_A->y = rect_B->y;
	rect_A->w = rect_B->w;
	rect_A->h = rect_B->h;
	
	return 0;
}

// assign value to rect
int STK_BaseRectAssign(SDL_Rect *rect, Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
	if (!rect)
		return -1;
		
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
	
	return 0;
}

// judge if two rects are equal
int STK_BaseRectEqual(SDL_Rect *rect_A, SDL_Rect *rect_B)
{
	if (!rect_A || !rect_B)
		return -1;

	if ( rect_A->x != rect_B->x
		|| rect_A->y != rect_B->y
		|| rect_A->w != rect_B->w
		|| rect_A->h != rect_B->h )
		return 0;

	return 1;
}

// adapter rect_A's width to rect_B
int STK_BaseRectAdapterWidth(SDL_Rect *rect_A, SDL_Rect *rect_B)
{
	if (!rect_A || !rect_B)
		return -1;

	if (rect_A->w != rect_B->w)
		rect_A->w = rect_B->w;
	
	return 0;
}

// adapter rect_A's height to rect_B
int STK_BaseRectAdapterHeight(SDL_Rect *rect_A, SDL_Rect *rect_B)
{
	if (!rect_A || !rect_B)
		return -1;

	if (rect_A->h != rect_B->h)
		rect_A->h = rect_B->h;
	
	return 0;
}


// adapter rect_A's width and height to rect_B
int STK_BaseRectAdapter(SDL_Rect *rect_A, SDL_Rect *rect_B)
{
	if (!rect_A || !rect_B)
		return -1;

	if (rect_A->w != rect_B->w)
		rect_A->w = rect_B->w;
	if (rect_A->h != rect_B->h)
		rect_A->h = rect_B->h;
	
	return 0;
}

int STK_BaseColorAssign(SDL_Color *color, Uint8 r, Uint8 g, Uint8 b)
{
	if (!color)
		return -1;
	
	color->r = r;
	color->g = g;
	color->b = b;

	return 0;
}



