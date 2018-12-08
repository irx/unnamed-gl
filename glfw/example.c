/**
 * Copyright 2018 Maksymilian Mruszczak <u at one u x dot o r g>
 *
 * You may do anything with this work that copyright law would normally
 * restrict, so long as you retain the above notice(s) and this license
 * in all redistributed copies and derived works.  There is no warranty.
 *
 *
 * Just a nice, colourful and animated quad
 * uses glfw3 obviously
 */

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/* SHADERS */
static const char *vert_shader_src = 
	"#version 120\n"
	"attribute vec2 position;\n"
	"attribute vec3 colour;\n"
	"uniform vec2 disp;\n"
	"varying vec3 Colour;\n"
	"void main()\n"
	"{\n"
	"	Colour = colour;\n"
	"	gl_Position = vec4(position.x*disp.x, position.y*disp.y, 0.0, 1.0);\n"
	"}\n";

static const char *frag_shader_src = 
	"#version 120\n"
	//"out vec4 outColor;\n"
	"uniform vec3 color;\n"
	"varying vec3 Colour;\n"
	"void main()\n"
	"{\n"
	//"	outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
	"	gl_FragColor = vec4(Colour+color, 1.0);\n"
	"}\n";


int
main(void)
{
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

	float vert[] = {
		 0.0f,  0.5f,  1.0f,  0.0f,  0.0f,
		 1.0f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.0f,  0.0f,  1.0f
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vert_shader_src, NULL);
	glCompileShader(vertex_shader);
	//GLint status;
	//glGetShderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	char buffer[512];
	glGetShaderInfoLog(vertex_shader, 512, NULL, buffer);
	printf("%s\n", buffer);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_shader_src, NULL);
	glCompileShader(fragment_shader);
	glGetShaderInfoLog(fragment_shader, 512, NULL, buffer);
	printf("%s\n", buffer);

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vertex_shader);
	glAttachShader(prog, fragment_shader);
	//glBindFragDataLocation(prog, 0, "outColor");
	glLinkProgram(prog);
	glUseProgram(prog);

	GLint position = glGetAttribLocation(prog, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glEnableVertexAttribArray(position);
	GLint colour = glGetAttribLocation(prog, "colour");
	glVertexAttribPointer(colour, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(2*sizeof(float)));
	glEnableVertexAttribArray(colour);

	GLint color = glGetUniformLocation(prog, "color");
	GLint disp = glGetUniformLocation(prog, "disp");
	//glUniform3f(color, 1.f, 0.f, 1.f);

	int width, height, frame = 0;
	GLfloat x = 0.0f;

	double start = glfwGetTime();
	double time, fps;

	while (!glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &width, &height);
		if (frame > 100) {
			time = glfwGetTime() - start;
			start = glfwGetTime();
			fps = (double)frame / time;
			frame = 0;
		}
		fprintf(stderr, "\rsize: %d %d  fps: %f", width, height, fps);
		fflush(stderr);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(prog);
		glUniform3f(color, 0.0f, (float)sin(x), 0.0f);
		glUniform2f(disp, (float)sin(x), (float)cos(x));
		glDrawArrays(GL_QUADS, 0, 4);
		glfwSwapBuffers(window);
		glfwPollEvents();
		x += 0.01f;
		++frame;
	}

	glfwTerminate();
	return 0;
}
