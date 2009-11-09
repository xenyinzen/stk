#ifndef _STK_TEXT_H_
#define _STK_TEXT_H_

#include "SDL.h"

typedef struct STK_Text {
	char *data;
	Uint32 length;		// the length of string
	Uint32 size;		// the size of the string buffer
	Uint32 charcount;	// the char numbers (may containing ASCII char and Chinese character)
} STK_Text;

typedef struct STK_TextPara {
	STK_Text text;
	int linenum;
	
	struct STK_TextPara *next;
	struct STK_TextPara *prev;
} STK_TextPara;


#endif /* _STK_FRAME_H_ */
