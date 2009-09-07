#ifndef _STK_FRAME_H_
#define _STK_FRAME_H_

typedef struct STK_Frame {
	
	Uint16 width;
	
	STK_Image *topleft;
	STK_Image *top;
	STK_Image *topright;
	STk_Image *left;
	STk_Image *right;
	STK_Image *bottomleft;
	STK_Image *bottom;
	STK_Image *bottomright;
}



#endif /* _STK_FRAME_H_ */