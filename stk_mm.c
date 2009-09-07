#include <stdio.h>
#include <malloc.h>

#include "stk_mm.h"


void *STK_Malloc( size_t n )
{
	void *p;
	
	if ((p = malloc(n)) == NULL) {
		fprintf(stderr, "Error while do stk_malloc.\n");
	}
	
	memset(p, 0, n);
	return p;
}
