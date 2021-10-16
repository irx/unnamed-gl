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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../ff/ff.h"

#define SPRITE_LIMIT 512


/* SHADERS */
static const char *vert_shader_src = 
	"#version 120\n"
	"attribute vec2 position;\n"
	"attribute vec3 colour;\n"
	"attribute vec2 texture;\n"
	"uniform vec2 disp;\n"
	"uniform mat4 tfm;\n"
	"varying vec2 Texcoord;\n"
	"varying vec3 Colour;\n"
	"void main()\n"
	"{\n"
	"	Texcoord = texture;\n"
	"	Colour = colour;\n"
	"	gl_Position = vec4(position.x+disp.x, position.y+disp.y, 1.0, 1.0) * tfm;\n"
	"}\n";

static const char *frag_shader_src =
	"#version 120\n"
	//"out vec4 outColor;\n"
	"uniform vec3 color;\n"
	"varying vec3 Colour;\n"
	"varying vec2 Texcoord;\n"
	"uniform sampler2D tex;\n"
	"void main()\n"
	"{\n"
	//"	outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
	"	gl_FragColor = texture2D(tex, Texcoord);// * vec4(Colour+color, 1.0);\n"
	"	//if (gl_FragColor.x + gl_FragColor.y + gl_FragColor.z > 2.7)\n"
	"		//gl_FragColor.a = 0.0;\n"
	"}\n";

typedef struct {
	float *vert;
	GLuint vao, vbo, vertex_shader, fragment_shader, prog,
	       texture, position, colour, pos, col, tfm;
	char *v_shd_src, *f_shd_src;
	GLuint sprites[SPRITE_LIMIT];
	size_t nsprites, spritew[SPRITE_LIMIT], spriteh[SPRITE_LIMIT];
	int w, h;
	GLFWwindow *window;
} Gc;

float vert[] = {
	 0.1f,  0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.1f, -0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.1f, -0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f
};

int
gc_init(Gc *gc)
{
	/*
	GLFWwindow *window;
	GLuint vao, vbo, vertex_shader, fragment_shader, prog,
	       position, colour, pos, col;
	*/
	char err[512];

	gc->nsprites = 0;
	gc->w = 640;
	gc->h = 480;

	if (!glfwInit())
		return -1;

	gc->window = glfwCreateWindow(gc->w, gc->h, "Hello World", NULL, NULL);

	if (!gc->window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(gc->window);
	glfwSwapInterval(1);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Wrangler failed!\n");
		return -1;
	}

	glGenVertexArrays(1, &gc->vao);
	glBindVertexArray(gc->vao);
	glGenBuffers(1, &gc->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gc->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), gc->vert, GL_STATIC_DRAW);

	/* transparency */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glOrtho(0, 640, 0, 480, -1, 1);
	glViewport(0, 0, 640, 480);

	gc->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gc->vertex_shader, 1, &vert_shader_src, NULL);
	glCompileShader(gc->vertex_shader);
	glGetShaderInfoLog(gc->vertex_shader, 512, NULL, err);
	fprintf(stderr, "%s\n", err);

	gc->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gc->fragment_shader, 1, &frag_shader_src, NULL);
	glCompileShader(gc->fragment_shader);
	glGetShaderInfoLog(gc->fragment_shader, 512, NULL, err);
	fprintf(stderr, "%s\n", err);

	gc->prog = glCreateProgram();
	glAttachShader(gc->prog, gc->vertex_shader);
	glAttachShader(gc->prog, gc->fragment_shader);
	//glBindFragDataLocation(prog, 0, "outColor");
	glLinkProgram(gc->prog);
	glUseProgram(gc->prog);

	gc->position = glGetAttribLocation(gc->prog, "position");
	gc->colour = glGetAttribLocation(gc->prog, "colour");
	gc->texture = glGetAttribLocation(gc->prog, "texture");
	glVertexAttribPointer(gc->position, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
	glVertexAttribPointer(gc->colour, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void *)(2*sizeof(float)));
	glVertexAttribPointer(gc->texture, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void *)(5*sizeof(float)));
	glEnableVertexAttribArray(gc->position);
	glEnableVertexAttribArray(gc->colour);
	glEnableVertexAttribArray(gc->texture);

	gc->col = glGetUniformLocation(gc->prog, "color");
	gc->pos = glGetUniformLocation(gc->prog, "disp");
	gc->tfm = glGetUniformLocation(gc->prog, "tfm");

	return 0;
}

int
gc_create_sprite(Gc *gc, const Image *img)
{
	GLuint tex;

	if (gc->nsprites > SPRITE_LIMIT)
		return -1;

	glUseProgram(gc->prog);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_FLOAT, img->d);

	gc->sprites[gc->nsprites] = tex;
	gc->spritew[gc->nsprites] = img->w;
	gc->spriteh[gc->nsprites] = img->h;
	return gc->nsprites++;
}

// DEBUG

void
gc_draw(Gc *gc, int x, int y, int col, int sprite)
{
	float fx, fy;
	float tfm[] = {
		 1.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};
	fx = (float)x/100.f;
	fy = (float)y/100.f;
	/* translate */
	tfm[3] = fx;
	tfm[7] = fy;
	/* scale */
	tfm[0] = (float)gc->spritew[sprite]/(float)gc->w;
	tfm[5] = (float)gc->spriteh[sprite]/(float)gc->h;
	glUseProgram(gc->prog);
	glBindTexture(GL_TEXTURE_2D, gc->sprites[sprite]);
	//glUniform3f(gc->col, 0.0f, (float)sin(col/100), 0.0f);
	//glUniform2f(gc->pos, fx, fy);
	glUniformMatrix4fv(gc->tfm, 1, GL_FALSE, tfm);
	glDrawArrays(GL_QUADS, 0, 4);
}

void
gc_clear(Gc *gc)
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void
gc_commit(Gc *gc)
{
	glfwSwapBuffers(gc->window);
	glfwPollEvents();
}

int
main(void)
{
	Image *img;
	int cat, tux;
	int width, height;
	int iter = 0;
	int pos = 0;
	Gc *gc = malloc(sizeof(Gc));
	if (gc == NULL)
		return 1;

	gc->vert = malloc(sizeof(vert));
	if (gc->vert == NULL)
		return 1;
	memcpy(gc->vert, vert, sizeof(vert));
	gc->v_shd_src = strdup(vert_shader_src);
	gc->f_shd_src = strdup(frag_shader_src);

	gc_init(gc);

	img = ff_load("../ff/test.ff");
	if (img < 0) {
		perror("error loading sprite");
		return 1;
	}
	cat = gc_create_sprite(gc, img);
	free(img->d);
	free(img);

	img = ff_load("./tux.ff");
	if (img < 0) {
		perror("error loading sprite");
		return 1;
	}
	tux = gc_create_sprite(gc, img);

	printf("%f %f\n", gc->vert[1], gc->vert[2]);

	while (!glfwWindowShouldClose(gc->window)) {
		glfwGetFramebufferSize(gc->window, &width, &height);
		printf("\r%d x %d", width, height);
		fflush(stdout);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, width, height, 0.0f, 0.0f, 1.0f);
		gc_clear(gc);
		gc_draw(gc, 15, 0, iter++, cat);
		gc_draw(gc, 5, 10, iter, tux);
		gc_draw(gc, 100*cos((float)iter/20.f), 100*sin((float)iter/20.f), iter, cat);
		gc_draw(gc, -100, ++pos, iter, cat);
		gc_commit(gc);
	}
	putchar('\n');
	return 0;
}
