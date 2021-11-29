/**
 * Copyright 2021 Max Mruszczak <u at one u x dot o r g>
 *
 * You may do anything with this work that copyright law would normally
 * restrict, so long as you retain the above notice(s) and this license
 * in all redistributed copies and derived works.  There is no warranty.
 *
 *
 * Test rendering images
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../ff/ff.h"
#include "render.h"

static int mypos = 0;
static int myposy = 0;
static int myposinc = 0;

int
main(void)
{
	Input user_input;
	Image *img;
	int cat, tux, harold;
	int width, height;
	int iter = 0;
	int pos = 0;
	Gc *gc = gc_new();
	Gc *gc2 = gc_new();
	if (gc == NULL)
		return 1;

	gc_init(gc);
	gc_init(gc2);
	gc_select(gc);

	img = ff_load("../ff/test.ff");
	if (img < 0) {
		perror("error loading sprite");
		return 1;
	}
	cat = gc_create_sprite(gc, img);
	gc_select(gc2);
	gc_create_sprite(gc2, img);
	gc_select(gc);
	free(img->d);
	free(img);

	img = ff_load("./tux.ff");
	if (img < 0) {
		perror("error loading sprite");
		return 1;
	}
	tux = gc_create_sprite(gc, img);

	img = ff_load("./harold.ff");
	harold = gc_create_sprite(gc, img);
	free(img->d);
	free(img);

	gc_bind_input(gc);

	while (gc_alive(gc)) {
		user_input = gc_poll_input();
		mypos += (int)user_input.dx;
		myposy -= (int)user_input.dy;
		gc_clear(gc);
		gc_draw(gc, 15, 0, cat);
		gc_draw(gc, 5, 10, tux);
		gc_draw(gc, 100*cos((float)iter/20.f), 100*sin((float)iter++/20.f), cat);
		gc_draw(gc, -100, ++pos, cat);
		gc_draw(gc, mypos, myposy, harold);
		gc_commit(gc);
		gc_select(gc2);
		gc_clear(gc2);
		gc_draw(gc2, 0, 0, cat);
		gc_commit(gc2);
		gc_select(gc);
	}
	putchar('\n');
	return 0;
}