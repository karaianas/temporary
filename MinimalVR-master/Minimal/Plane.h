#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "shader.h"

class Plane {
public:
	Plane();

	bool createFrameBuffer();
	void createTexture();
	void draw(GLuint program, glm::mat4 V, glm::mat4 P, int test);

//private:
	//GLuint program;
	GLuint FBO, VAO, VBO, EBO;
	GLuint textureID;
	GLuint texID;
	GLenum DrawBuffers[1];
	glm::mat4 toWorld;

	GLuint test2;

	GLuint loadTexture();
	unsigned char* loadPPM(const char* filename, int& width, int& height);

	const GLfloat vertices[8][3] = {
	// "Front" vertices
	{ -1.0, -1.0,  1.0 },{ 1.0, -1.0,  1.0 },{ 1.0,  1.0,  1.0 },{ -1.0,  1.0,  1.0 },
	// "Back" vertices
	{ -1.0, -1.0, -1.0 },{ 1.0, -1.0, -1.0 },{ 1.0,  1.0, -1.0 },{ -1.0,  1.0, -1.0 }
	};


	const GLuint indices[6][6] = {
	// Front
	{ 7, 4, 5, 5, 6, 7 },
	// Back
	{ 0, 3, 2, 2, 1, 0 },
	// Left
	{ 0, 4, 7, 7, 3, 0 },
	// Right
	{ 5, 1, 2, 2, 6, 5 },
	// Top
	{ 7, 6, 2, 2, 3, 7 },
	// Bottom
	{ 4, 0, 5, 5, 0, 1 }
	};
};

static const GLfloat g_quad_vertex_buffer_data[] = {
	-0.5f, -0.5f, 0.f,
	0.5f, -0.5f, 0.f,
	-0.5f,  0.5f, 0.f,
	-0.5f,  0.5f, 0.f,
	0.5f, -0.5f, 0.f,
	0.5f,  0.5f, 0.f,
};