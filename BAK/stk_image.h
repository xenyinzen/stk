#ifndef _STK_IMAGE_H_
#define _STK_IMAGE_H_

// for STK_Image.flags.
#define STK_IMAGE_USERDEFINE	0x00000001


typedef struct STK_Image {

	SDL_Surface *surface;
	SDL_Rect rect;
	
	Uint32 bgcolor;
	Uint32 flags;	

	char *filename;		// filename to load a picture
} STK_Image;




#endif /* _STK_IMAGE_H_ */