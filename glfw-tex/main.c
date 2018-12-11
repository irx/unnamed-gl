/**
 * Copyright 2018 Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * You may do anything with this work that copyright law would normally
 * restrict, so long as you retain the above notice(s) and this license
 * in all redistributed copies and derived works.  There is no warranty.
 *
 *
 * Draw rotating picture of a cat
 */

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "imgiolib.h"


int
main(void)
{
	char errorb[512];
	int width, height;

	/* Shader sources */
	static const char *vshd_src =
		"#version 120\n"
		"attribute vec3 position;\n"
		"attribute vec2 texture;\n"
		"uniform float time;\n"
		"varying vec2 Texcoord;\n"
		"varying vec3 Colour;\n"
		"float phase;\n"
		"float magn;\n"
		"vec3 pos;\n"
		"void main()\n"
		"{\n"
		"	Texcoord = texture;\n"
		"	Colour = position+1.0;\n"
		/*
		"	phase = atan(position.y/position.x);\n"
		"	magn = 1.0; //sqrt(pow(position.x, 2)+pow(position.z, 2));\n"
		"	if (position.x < 0.0)\n"
		"		if (pos.y > 0.0)\n"
		"			phase = 3.14+phase;\n"
		"		else\n"
		"			phase = phase+3.14;\n"
		"	if (position.x > 0.0 && position.y < 0.0)\n"
		"		phase = 6.28-phase;\n"
		"	pos = vec3(magn*cos(phase+time), magn*sin(phase+time), position.z);\n"
		"	phase = atan(pos.z/pos.x);\n"
		"	if (pos.x < 0.0)\n"
		"		if (pos.z > 0.0)\n"
		"			phase = 3.14+phase;\n"
		"		else\n"
		"			phase = phase+3.14;\n"
		"	if (position.x > 0.0 && position.z < 0.0)\n"
		"		phase = 6.28+phase;\n"
		"	//gl_Position = vec4(magn*cos(phase+time), pos.y, magn*sin(phase+time), 1.0);\n"
		*/
		"	phase = ((3.14*0.25+0.25*3.14*(1.0-position.x))*position.y)+(3.14*(1.0-position.y));\n"
		"	gl_Position = vec4(cos(phase+time), sin(phase+time), position.z, 1.0);\n"
		"}\n";

	static const char *fshd_src = 
		"#version 120\n"
		"varying vec2 Texcoord;\n"
		"varying vec3 Colour;\n"
		"uniform sampler2D tex;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = texture2D(tex, Texcoord) * vec4(Colour, 1.0);\n"
		"}\n";

	float cube[] = {
		 1.f,  1.f,  1.f,   1.f,  1.f, // 0
		 1.f, -1.f,  1.f,   1.f,  0.f, // 1
		-1.f, -1.f,  1.f,   0.f,  0.f, // 2
		-1.f,  1.f,  1.f,   0.f,  1.f, // 3
		 1.f,  1.f, -1.f,  -1.f,  1.f, // 4
		 1.f, -1.f, -1.f,  -1.f, -1.f, // 5
		-1.f, -1.f, -1.f,   1.f, -1.f, // 6
		-1.f,  1.f, -1.f,   1.f,  1.f  // 7
	};

	/* above vertices were meant for cube but plane will suffice here */
	GLuint elems[] = {
		0, 1, 2, 3 // front
		/*
		4, 5, 1, 0, // right
		7, 6, 5, 4, // back
		3, 2, 6, 7, // left
		4, 0, 3, 7, // top
		1, 5, 6, 2  // bottom
		*/
	};

	/* Context */
	GLFWwindow *window;
	if (!glfwInit())
		return -1;
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	/* VAO */
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* VBO */
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	/* Elem VBO */
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

	/* Load JPEG */
	Image *pic = create_image();
	load_JPEG(pic, "cat.jpg");
	float *pixels = image2fv(pic);

	/* Textures */
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pic->w, pic->h, 0, GL_RGBA, GL_FLOAT, pixels);

	destroy_image(pic);
	free(pixels);

	/* Shaders */
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vshd_src, NULL);
	glCompileShader(vertex_shader);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fshd_src, NULL);
	glCompileShader(fragment_shader);

	/* Report compile errors */
	glGetShaderInfoLog(vertex_shader, 512, NULL, errorb);
	fprintf(stderr, "%s\n", errorb);
	glGetShaderInfoLog(fragment_shader, 512, NULL, errorb);
	fprintf(stderr, "%s\n", errorb);

	/* Program */
	GLuint prog = glCreateProgram();
	glAttachShader(prog, vertex_shader);
	glAttachShader(prog, fragment_shader);
	glLinkProgram(prog);
	glUseProgram(prog);

	/* Attributes */
	GLint position = glGetAttribLocation(prog, "position");
	GLint texture = glGetAttribLocation(prog, "texture");
	glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(texture);

	GLint time = glGetUniformLocation(prog, "time");

	/* Main loop */
	while (!glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &width, &height);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(prog);
		glUniform1f(time, glfwGetTime());
		glDrawElements(GL_QUADS, 32, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
