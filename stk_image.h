#ifndef _STK_IMAGE_H_
#define _STK_IMAGE_H_

#include "SDL.h"
// for STK_Image flag.
#define STK_IMAGE_USERDEFINE	0x00000001

typedef enum STK_ImageStyle {
	STK_IMAGESTYLE_NORMAL,
	STK_IMAGESTYLE_HORIZONTAL,
	STK_IMAGESTYLE_VERTICAL,
	STK_IMAGESTYLE_MATRIX,
	STK_IMAGESTYLE_PICTURE,
} STK_ImageStyle;


typedef struct STK_Image {

	Uint32 bgcolor;
	Uint32 flags;
	STK_ImageStyle fillstyle;	

	char *filename;		// filename to load a picture
} STK_Image;

int STK_ImageFillRect(SDL_Surface *surface, STK_Image *image, SDL_Rect *rect, Uint32 pattern, Uint32 n);



#endif /* _STK_IMAGE_H_ */