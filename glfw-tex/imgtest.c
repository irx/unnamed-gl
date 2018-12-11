/**
 * Copyright 2018 Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * You may do anything with this work that copyright law would normally
 * restrict, so long as you retain the above notice(s) and this license
 * in all redistributed copies and derived works.  There is no warranty.
 *
 *
 * Generate gradient image for testing purposes
 */

#include <stdlib.h>
#include "imgiolib.h"

int
main(void)
{
	Image *img = create_image();
	img->h = 255;
	img->w = 255;
	img->d = (Rgba *)malloc(sizeof(Rgba)*255*255);
	for (int i=0; i < 255; ++i)
		for (int j=0; j < 255; ++j) {
			img->d[255*i+j].r = i;
			img->d[255*i+j].g = j;
			img->d[255*i+j].b = 255 - i;
		}
	int ret = save_JPEG(img, "fuck.jpg", 100);
	destroy_image(img);
	return ret;
}
