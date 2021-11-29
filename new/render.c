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
#include "render.h"

#define SPRITE_LIMIT 512

/* SHADERS */
static const char *vert_shader_src = 
	"#version 120\n"
	"attribute vec2 position;\n"
	"attribute vec2 texture;\n"
	"uniform mat4 tfm;\n"
	"varying vec2 Texcoord;\n"
	"void main()\n"
	"{\n"
	"	Texcoord = texture;\n"
	"	gl_Position = vec4(position.x, position.y, 1.0, 1.0) * tfm;\n"
	"}\n";

static const char *frag_shader_src =
	"#version 120\n"
	"varying vec2 Texcoord;\n"
	"uniform sampler2D tex;\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = texture2D(tex, Texcoord);\n"
	"}\n";

struct gc {
	float *vert;
	GLuint vao, vbo, vertex_shader, fragment_shader, prog,
	       texture, position, tfm;
	char *v_shd_src, *f_shd_src;
	GLuint sprites[SPRITE_LIMIT];
	size_t nsprites, spritew[SPRITE_LIMIT], spriteh[SPRITE_LIMIT];
	int w, h;
	GLFWwindow *window;
};

static void key_callback(GLFWwindow *, int, int, int, int);

static Input global_input;

static float vert[] = {
	 0.1f,  0.1f,  1.0f,  0.0f,
	 0.1f, -0.1f,  1.0f,  1.0f,
	-0.1f, -0.1f,  0.0f,  1.0f,
	-0.1f,  0.1f,  0.0f,  0.0f
};

Gc *
gc_new(void)
{
	return malloc(sizeof(Gc));
}

int
gc_init(Gc *gc)
{
	char err[512];

	gc->nsprites = 0;
	gc->w = 640;
	gc->h = 480;

	gc->vert = malloc(sizeof(vert));
	if (gc->vert == NULL)
		return -1;
	memcpy(gc->vert, vert, sizeof(vert));
	gc->v_shd_src = strdup(vert_shader_src);
	gc->f_shd_src = strdup(frag_shader_src);

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
	glLinkProgram(gc->prog);
	glUseProgram(gc->prog);

	gc->position = glGetAttribLocation(gc->prog, "position");
	gc->texture = glGetAttribLocation(gc->prog, "texture");
	glVertexAttribPointer(gc->position, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	glVertexAttribPointer(gc->texture, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)(2*sizeof(float)));
	glEnableVertexAttribArray(gc->position);
	glEnableVertexAttribArray(gc->texture);

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
gc_draw(Gc *gc, int x, int y, int sprite)
{
	float tfm[] = {
		 1.0f,  0.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,  0.0f,
		 0.0f,  0.0f,  1.0f,  0.0f,
		 0.0f,  0.0f,  0.0f,  1.0f
	};
	/* translate */
	tfm[3] = (float)x/100.f;
	tfm[7] = (float)y/100.f;
	/* scale */
	tfm[0] = (float)gc->spritew[sprite]/(float)gc->w;
	tfm[5] = (float)gc->spriteh[sprite]/(float)gc->h;
	glUseProgram(gc->prog);
	glBindTexture(GL_TEXTURE_2D, gc->sprites[sprite]);
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

GLFWwindow *
gc_get_window(const Gc *gc)
{
	return gc->window;
}

int
gc_alive(const Gc *gc)
{
	return !glfwWindowShouldClose(gc->window);
}

void
gc_select(const Gc *gc)
{
	glfwMakeContextCurrent(gc->window);
}

void
gc_bind_input(const Gc *gc)
{
	glfwSetKeyCallback(gc_get_window(gc), key_callback);
}

Input
gc_poll_input(void)
{
	glfwPollEvents();
	return global_input;
}

static void
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		global_input.dx = 1.f;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		global_input.dx = 0.f;
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		global_input.dx = -1.f;
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		global_input.dx = 0.f;
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		global_input.dy = -1.f;
	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		global_input.dy = 0.f;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		global_input.dy = 1.f;
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		global_input.dy = 0.f;
}
