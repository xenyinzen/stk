#ifndef _STK_PRIM_H_
#define _STK_PRIM_H_




#define CLIP_ACCEPT(a, b)  	(!(a|b))
#define CLIP_REJECT(a, b)  	(a&b)
#define CLIP_INSIDE(a)		(!a)

#define CLIP_LEFT_EDGE		0x01
#define CLIP_RIGHT_EDGE		0x02
#define CLIP_TOP_EDGE		0x08
#define CLIP_BOTTOM_EDGE	0x04


#endif /* _STK_PRIM_H_ */