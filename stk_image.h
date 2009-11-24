#ifndef _STK_IMAGE_H_
#define _STK_IMAGE_H_

#include "SDL.h"
// for STK_Image flag.
#define STK_IMAGE_USERDEFINE	0x00000001

enum STK_ImageKind {
	STK_IMAGE_KIND_FRAME = 0,
	STK_IMAGE_KIND_BOX

};

enum STK_ImageFramePattern {
	STK_IMAGE_FRAME_CONVEX = 0,
	STK_IMAGE_FRAME_CONCAVE,
	STK_IMAGE_FRAME_SINGLELINE,
	STK_IMAGE_FRAME_DOUBLELINE
};

enum STK_ImageBoxPattern {
	STK_IMAGE_BOX_HOLLOW = 0,
	STK_IMAGE_BOX_SOLID
};


enum STK_ImageStyle {
	STK_IMAGE_FILLSTYLE_NORMAL = 0,
	STK_IMAGE_FILLSTYLE_HORIZONTAL,
	STK_IMAGE_FILLSTYLE_VERTICAL,
	STK_IMAGE_FILLSTYLE_MATRIX,
	STK_IMAGE_FILLSTYLE_PICTURE,
};


typedef struct STK_Image {

	Uint32 bgcolor;
	Uint32 flags;
	enum STK_ImageStyle fillstyle;	

	char *filename;		// filename to load a picture
} STK_Image;

int STK_ImageFillRect(SDL_Surface *surface, SDL_Rect *rect, Uint32 kind, Uint32 pattern, STK_Image *image, Uint32 index);


#endif /* _STK_IMAGE_H_ */