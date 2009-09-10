#ifndef _STK_IMAGE_H_
#define _STK_IMAGE_H_

// for STK_Image flag.
#define STK_IMAGE_USERDEFINE	0x00000001

#define STK_IMAGESTYLE_BORADER_THICK   2

typedef enum STK_ImageStyle {
	STK_IMAGESTYLE_NORMAL,
	STK_IMAGESTYLE_HORIZONTAL,
	STK_IMAGESTYLE_VERTICAL,
	STK_IMAGESTYLE_BORDER,
} STK_ImageStyle;


typedef struct STK_Image {

	Uint32 bgcolor;
	Uint32 flags;
	STK_ImageStyle fillstyle;	

	char *filename;		// filename to load a picture
} STK_Image;




#endif /* _STK_IMAGE_H_ */