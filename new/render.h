/**
 * Copyright 2021 Max Mruszczak <u at one u x dot o r g>
 *
 * You may do anything with this work that copyright law would normally
 * restrict, so long as you retain the above notice(s) and this license
 * in all redistributed copies and derived works.  There is no warranty.
 *
 *
 * Render images
 */


typedef struct gc Gc;

typedef struct {
	float dx, dy;
} Input;

Gc * gc_new(void);
int gc_init(Gc *);
int gc_create_sprite(Gc *, const Image *);
void gc_draw(Gc *, int, int, int);
void gc_clear(Gc *);
void gc_commit(Gc *);
//GLFWwindow * gc_get_window(const Gc *);
int gc_alive(const Gc *);
void gc_select(const Gc *);
void gc_bind_input(const Gc *);
Input gc_poll_input(void);
