#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "stk_prim.h"



int stk_prim_drawLine(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	int pixx, pixy;
	int x, y;
	int dx, dy;
	int ax, ay;
	int sx, sy;
	int swaptmp;
	Uint8 *pixel;
	Uint8 *colorptr;
	
	if (!(clipLine(dst, &x1, &y1, &x2, &y2)))
		return 0;
		
	if (x1 == x2) {
		if (y1 < y2) {
			return vlineColor(dst, x1, y1, y2, color);
		}
		else if (y1 > y2) {
			return vlineColor(dst, x1, y2, y1, color);
		}
		else {
			return pixelColor(dst, x1, y1, color);
		}
	}
	
	if (y1 == y2) {
		if (x1 < x2) {
			return hlineColor(dst, x1, x2, y1, color);
		}
		else if (x1 > x2) {
			return hlineColor(dst, x2, x1, y1, color);
		}
	}
	
	dx = x2 - x1;
	dy = y2 - y1;
	sx = (dx >= 0) ? 1 : -1;
	sy = (dy >= 0) ? 1 : -1;
	
	if (SDL_MUSTLOCK(dst))
		if (SDL_LockSurface(dst) < 0)
			return -1;
	
	if ((color & 0xff) == 0xff) {
		colorptr = (Uint8 *)&color;
		color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
	
		dx = sx * dx + 1;
		dy = sy * dy + 1;
		pixx = dst->format->BytesPerPixel;
		pixy = dst->pitch;
		
		pixel = ((Uint8 *)dst->pixels) + pixx * (int)x1 + pixy * (int)y1;
		pixx *= sx;
		pixy *= sy;
		if (dx < dy) {
			swaptmp = dx;
			dx = dy;
			dy = swaptmp;
			
			swaptmp = pixx;
			pixx = pixy;
			pixy = swaptmp;
		}
		
		
		x = 0; y = 0;
		switch (dst->format->BytesPerPixel) {
		case 1:
			for (; x < dx; x++, pixel += pixx) {
				*(Uint8 *)pixel = color;
				y += dy;
				if (y >= dx) {
					y -= dx;
					pixel += pixy;
				}
			}
			break;
		case 2:
			for (; x < dx; x++, pixel += pixx) {
				*(Uint16 *)pixel = color;
				y += dy;
				if (y >= dx) {
					y -= dx;
					pixel += pixy;
				}
			}
			break;
		case 3:
			for (; x < dx; x++, pixel += pixx) {

				pixel[0] = color & 0xff;
				pixel[1] = (color >> 8) & 0xff;
				pixel[2] = (color >> 16) & 0xff;
				
				y += dy;
				if (y >= dx) {
					y -= dx;
					pixel += pixy;
				}
			}
			break;
		case 4:
			for (; x < dx; x++, pixel += pixx) {
				*(Uint32 *)pixel = color;
				y += dy;
				if (y >= dx) {
					y -= dx;
					pixel += pixy;
				}
			}
			break;
		}
	}
	
	if (SDL_MUSTLOCK(dst))
		SDL_UnlockSurface()dst;
		
	return 1;	
}

int stk_prim_drawCircle(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 r, Uint32 color)
{
	Sint16 left, right, top, bottom;
	int result;
	Sint16 x1, y1, x2, y2;
	Sint16 cx = 0;
	Sint16 cy = r;
	Sint16 ocx = (Sint16)0xffff;
	Sint16 ocy = (Sint16)0xffff;
	Sint16 df = 1 - r;
	Sint16 d_e = 3;
	Sint16 d_se = -2 * r + 5;
	Sint16 xpcx, xmcx, xpcy, xmcy;
	Sint16 ypcy, ymcy, ypcx, ymcx;

	if (r < 0)
		return -1;
	
	if (r == 0)
		return pixelColor(dst, x, y, color);
	
	// get clip boundary
	left = dst->clip_rect.x;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	top = dst->clip_rect.y;
	bottom = dst->clip_rect.y + dst->clip_rect.h -1;
	
	x1 = x - r;
	x2 = x + r;
	y1 = y - r;
	y2 = y + r;
	if (x1 < left && x2 < left)
		return 0;
	if (x1 > right && x2 > right)
		return 0;
	if (y1 < top && y2 < top)
		return 0;
	if (y1 > bottom && y2 > bottom)
		return 0;
	
	// begin to draw
	result = 0;
	do {
		xpcx = x + cx;
		xmcx = x - cx;
		xpcy = x + cy;
		xmcy = x - cy;
		if (ocy != cy) {
			if (cy > 0) {
				ypcy = y + cy;
				ymcy = y - cy;
				result |= hlineColor(dst, xmcx, xpcx, ypcy, color);
				result |= hlineColor(dst, xmcx, xpcx, ymcy, color);
			}
			else {
				result |= hlineColor(dst, xmcx, xpcx, y, color);
			}
			ocy = cy;
		}
		
		if (ocx != cx) {
			if (cx != cy) {
				if (cx > 0) {
					ypcx = y + cx;
					ymcx = y - cx;
					result |= hlineColor(dst, xmcy, xpcy, ymcx, color);
					result |= hlineColor(dst, xmcy, xpcy, ypcx, color);
				}
				else {
					result |= hlineColor(dst, xmcy, xpcy, y, color);
				}
			}
			ocx = cx;
		}
		
		if (df < 0) {
			df += d_e;
			d_e += 2;
			d_se += 2;
		}
		else {
			df += d_se;
			d_e += 2;
			d_se += 4;
			cy--;
		}
		
		cx++;	
	} while (cx <= cy);
	
	return result;
}

static int clipLine(SDL_Surface *dst, Sint16 *x1, Sint16 *y1, Sint16 *x2, Sint16 *y2)
{	
	Sint16 left, right, top, bottom;
	int code1, code2;
	int draw = 0;
	Sint16 swaptmp;
	float m;
	
	// get clipping boundary
	left = dst->clip_rect.x;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	top = dst->clip_rect.y;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	
	while (1) {
		code1 = clipEncode(*x1, *y1, left, right, top, bottom);
		code2 = clipEncode(*x2, *y2, left, right, top, bottom);
		if (CLIP_ACCEPT(code1, code2)) {
			draw = 1;
			break;
		}
		else if (CLIP_REJECT(code1, code2)) {
			break;
		}
		else {
			if (CLIP_INSIDE(code1)) {
				swaptmp = *x2;
				*x2 = *x1;
				*x1 = swaptmp;
				
				swaptmp = *y2;
				*y2 = *y1;
				*y1 = swaptmp;
				
				swaptmp = code2;
				code2 = code1;
				code1 = swaptmp;
			}
		
			if (*x2 != *x1) {
				m = (*y2 - *y1) / (float)(*x2 - *x1);
			}
			else {
				m = 1.0f;
			}
			
			if (code1 & CLIP_LEFT_EDGE) {
				*y1 += (Sint16)((left - *x1) * m);
				*x1 = left;			
			}
			else if (code1 & CLIP_RIGHT_EDGE) {
				*y1 += (Sint16)((right - *x1) * m);
				*x1 = right;
			}
			else if (code1 & CLIP_TOP_EDGE) {
				if (*x2 != *x1) {
					*x1 = (Sint16)((top - *y1) / m);
				}
				*y1 = top;
			}
			else if (code1 & CLIP_BOTTOM_EDGE) {
				if (*x2 != *x1) {
					*x1 = (Sint16)((bottom - *y1) / m);
				}
				*y1 = bottom;
			}
		}
	
	}
	
	return draw;
}

static int clipEncode(Sint16 x, Sint16 y, Sint16 left, Sint16 top, Sint16 right, Sint16 bottom)
{
	int code = 0;
	if (x < left) {
		code |= CLIP_LEFT_EDGE;
	}
	else if (x > right) {
		code |= CLIP_RIGHT_EDGE;
	}
	
	if (y < top) {
		code |= CLIP_TOP_EDGE;
	}
	else if (y > bottom) {
		code |= CLIP_BOTTOM_EDGE;
	}

	return code;
}

int hlineColor(SDL_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color)
{
	Sint16 left, right, top, bottom;
	Uint8 *pixel, *pixellast;
	int dx;
	int pixx, pixy;
	Sint16 w;
	Sint16 xtmp;
	int result = -1;
	Uint8 *colorptr;
	
	// Get clipping boundary
	left = dst->clip_rect.x;
	right = dst->clip_rect.x + dst->clip_rect.w - 1;
	top = dst->clip_rect.y;
	bottom = dst->clip_rect.y + dst->clip_rect.h - 1;
	
	// Check visibility of hline
	if ((x1 < left) && (x2 < left))
		return 0;
	if ((x1 > right) && (x2 > right))
		return 0;
	if ((y < top) || (y > bottom))
		return 0;

	// clip x
	if (x1 < left)
		x1 = left;
	if (x2 > right)
		x2 = right;
		
	// swap x1, x2 if needed
	if (x1 > x2) {
		xtmp = x1;
		x1 = x2;
		x2 = xtmp;
	}
	
	// calculate width
	w = x2 - x1;
	if (w < 0)
		return 0;
		
	// alpha check
	if ((color & 0xff) == 0xff) {
		// No alpha-blending required
		
		colorptr = (Uint8 *)&color;
		color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
	
		SDL_LockSurface(dst);
		
		dx = w;
		pixx = dst->format->BytesPerPixel;
		pixy = dst->pitch;
		pixel = ((Uint8 *)dst->pixels) + pixx * (int)x1 + pixy * (int)y;
		
		switch (dst->format->BytesPerPixel) {
		case 1:
			memset(pixel, color, dx);
			break;
		case 2:
			pixellast = pixel + dx + dx;
			for (; pixel <= pixellast; pixel += pixx) {
				*(Uint16 *)pixel = color;
			}
			break;
		case 3:
			pixellast = pixel + dx + dx + dx;
			for (; pixel <= pixellast; pixel += pixx) {
				pixel[0] = color & 0xff;
				pixel[1] = (color >> 8) & 0xff;
				pixel[2] = (color >> 16) & 0xff;
			}
			break;
		case 4:
			dx = dx + dx;
			pixellast = pixel + dx + dx;
			for (; pixel <= pixellast; pixel += pixx) {
				*(Uint32 *)pixel = color;
			}
			break;
		default:
			break;			
		
		}
		
		SDL_UnlockSurface(dst);
		result = 0;
	}
	
	return 0;		
}

int vlineColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 y2, Uint32 color)
{
	


}



