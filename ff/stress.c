#include <stdio.h>
#include <stdlib.h>

#include "ff.h"

int
main(void)
{
	int i;
	size_t tsiz;
	Image *imgs[320];
	Image *img = ff_load("test.ff");

	if (!img)
		return 1;
	printf("%lu x %lu\n", img->w, img->h);

	tsiz = 0;
	for (i = 0; i < 320; ++i) {
		imgs[i] = ff_load("test.ff");
		if (!imgs[i])
			return 1;
		tsiz += imgs[i]->siz * sizeof(float);
		printf("\r%3d/320  %luMB", i+1, tsiz/1000000);
		fflush(stdout);
	}

	putchar('\n');

	return 0;
}
