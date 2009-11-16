
#include <stdio.h>

char *a[] = {
	"AAA",
	"B",
	"CCC",
	"DDD",
	"EEEfff",
};

int main()
{
	int i = 0;
	for (i=0; i<5; i++) {
		printf("%i  %s\n", i, a[i] );
	}
	printf("n = %d\n", sizeof(a)/sizeof(char *));
	
	return 0;
}
