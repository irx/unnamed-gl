/**
 * Copyright 2021 Max Mruszczak <u at one u x dot o r g>
 *
 * You may do anything with this work that copyright law would normally
 * restrict, so long as you retain the above notice(s) and this license
 * in all redistributed copies and derived works.  There is no warranty.
 *
 *
 * Read farbfeld image
 */

#include <arpa/inet.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ff.h"

#define MUL_BOUND(x, y) (y > INT_MAX / x || y < INT_MIN / x)

Image *
ff_load(const char *path)
{
	FILE *f;
	uint8_t hdr[16];
	uint16_t *rowbuf;
	size_t rowsiz, i, j, offs, cur;
	Image *img;

	if (!(f = fopen(path, "rb"))) {
		perror("couldn't open file");
		return NULL;
	}
	if (fread(hdr, 1, 16, f) != 16) {
		perror("couldn't read header");
		fclose(f);
		return NULL;
	}
	if (memcmp(hdr, "farbfeld", 8)) {
		perror("not a farbfeld file");
		fclose(f);
		return NULL;
	}

	if (!(img = malloc(sizeof(Image)))) {
		perror("failed to allocate image struct");
		fclose(f);
		return NULL;
	}
	img->w = ntohl(((uint32_t *)hdr)[2]);
	img->h = ntohl(((uint32_t *)hdr)[3]);
	/*
	if (MUL_BOUND(img->w, img->h)) {
		perror("image size out of boundaries");
		free(img);
		fclose(f);
		return NULL;
	}
	*/
	img->siz = img->w * img->h * 4;
	if (!(img->d = malloc(sizeof(float) * img->siz))) {
		perror("failed to allocate image struct");
		free(img);
		fclose(f);
		return NULL;
	}

	rowsiz = sizeof(uint16_t) * 4 * img->w;
	if (!(rowbuf = malloc(rowsiz))) {
		perror("failed to allocate rowbuf struct");
		free(img->d);
		free(img);
		fclose(f);
		return NULL;
	}

	cur = 0;
	for (i = 0; i < img->h; ++i) {
		if (fread(rowbuf, 1, rowsiz, f) != rowsiz) {
			perror("failed to read image row");
			free(rowbuf);
			free(img->d);
			free(img);
			fclose(f);
			return NULL;
		}
		for (j = 0; j < img->w; ++j) {
			offs = j * 4;
			img->d[cur++] = (float)ntohs(rowbuf[offs]) / 65536.f;
			img->d[cur++] = (float)ntohs(rowbuf[offs+1]) / 65536.f;
			img->d[cur++] = (float)ntohs(rowbuf[offs+2]) / 65536.f;
			img->d[cur++] = (float)ntohs(rowbuf[offs+3]) / 65536.f;
		}
	}

	free(rowbuf);
	fclose(f);
	return img;
}
