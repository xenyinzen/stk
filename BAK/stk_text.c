#include <stdio.h>
#include <stdlib.h>

#include "stk_mm.h"
#include "stk_text.h"

// 64 bytes
#define MEM_BLOCK_UNIT	64

STK_Text *STK_TextNew(char *str)
{
	STK_Text *text;
	int c;
	char *p;
	
	text = (STK_Text *)STK_Malloc(sizeof(STK_Text));
	if (str) {
		text->length = strlen(str);
		
		c = text->length / MEM_BLOCK_UNIT;
		text->size = (c + 1) * MEM_BLOCK_UNIT;
		
		p = (char *)STK_Malloc(text->size);
		strcpy(p, str);
		text->data = p;
	}
	
	return text;
}

/*
STK_Text *STK_TextNewFixed(char *str, Uint16 size)
{
	STK_Text *text;
	int nn = 1024;
	int len = 0;
	
	len = strlen(str);
	if (size <= len) {
		fprintf(stderr, "Size is too small.");
		return -1;	
	}	
	if (size >= 64 && size < nn)
		nn = size;
	
	text = (STK_Text *)STK_Malloc(sizeof(STK_Text));
	text->data = STK_Malloc(nn);
	if (str) {
		sprintf(text->data. "%s", str);
		text->length = strlen(str);
		text->size = nn;
		text->flag = 1;
	}

	return text;
}
*/

int STK_TextFree(STK_Text *text)
{
	if (text) {
		if (text->data)
			free(text->data);
		free(text);
	}
	
	return 0;
}

int STK_TextSetText(STK_Text *text, char *str)
{
	int len;

	if (!text || !str ) {
		fprintf(stderr, "STK_TextSetText: text or str is NULL.\n");
		return 1;
	}
	len = strlen(str);
	// if string buffer has variable length
	else if (text->flag == 0) {
		char *p;
		int c;
		while (len > text->size) {
			// expand to twice of the original memory block size
			text->size = text->size * 2;	
		}
		
		p = (char *)STK_Malloc(text->size);
		// free old data buffer
		free(text->data);
		text->data = p;
		text->length = len;
	}

	return 0;	
}

Uint32 STK_TextGetLength(STK_Text *text)
{
	if (!text)
		return -1;
	
	return text->length;
}

STK_Text *STK_TextSubText(STK_Text *text, int start, Uint32 count)
{
	STK_Text *subtext;
	char *p, *pn;
	
	if (!text)
		return -1;
	if (start >= text->length)
		return -1;
		
	p = (char *)text->data + start;
	pn = (char *)STK_Malloc(count+1);
	// here, it will judge whether count is too big internally
	strncpy(pn, p, count); 
	pn[count] = '\0';
	
	subtext = STK_TextNew(pn);
	free(pn);

	return subtext;
}

int STK_TextAppendStr(STK_Text *text, char *str)
{
	


}

int STK_TextInsertStr(STK_Text *text, char *str, int pos)
{
	char *p, *pn;
	int len;
	int newlen;
	
	if (!text || !str ) {
		fprintf(stderr, "STK_TextInsertStr: text or str is NULL.\n");
		return -1;
	}
	
	if (pos >= text->length) {
		fprintf(stderr, "STK_TextInsertStr: pos is too large.\n");
		return -1;
	}
	
	len = strlen(str);
	newlen = text->length + len;
	// need to alloc new memory block
	if (newlen >= text->size) {
		while (newlen >= text->size) {
			text->size = text->size * 2;
		}
		
		// insert buffer
		pn = (char *)STK_Malloc(newlen + 1);
		strncpy(pn, text->data, pos);
		strncpy(pn + pos, str, len);
		strcpy(pn + pos + len, text->data + pos);
		// free old data buffer
		free(text->data);
		// assign new data buffer
		text->data = pn;
		text->length = newlen;
	}
	// not neccessory to alloc new memory block
	else {
		int i;
		// move data after specified position
		for (i = text->length; i >= pos; i--) {
			text->data[i + 1] = text->data[i];
		}
		strncpy(text->data + pos, str, len);

		text->length = newlen;
	}
	return 0;
}

int STK_TextDeleteStr(STK_Text *text, int pos, int count)
{
	if (pos >= text->length || count >= length) {
		fprintf(stderr, "STK_TextDeleteStr: pos or count is too large.\n");
		return -1;	
	}
	
	// here, we don't alloc new buffer for effective
	if (pos + count <= text->length)
		strcpy(text->data + pos, text->data + pos + count);
	else {
		text->data[pos] = '\0';
		text->length = pos;
		text->size = text->length;
	}
	
	return 0;
}

