#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace std;

class Cube
{
public:
	Cube();
	~Cube();

	glm::mat4 toWorld;

	void draw(GLuint program, glm::mat4 V, glm::mat4 P);
	//void update();
	//void spin(float);

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview;

	void loadTex();
	unsigned char* Cube::loadPPM(const char* filename, int& width, int& height);
};

const GLfloat vertices[8][3] = {
	// "Front" vertices
	{ -1.0, -1.0,  1.0 },{ 1.0, -1.0,  1.0 },{ 1.0,  1.0,  1.0 },{ -1.0,  1.0,  1.0 },
	// "Back" vertices
	{ -1.0, -1.0, -1.0 },{ 1.0, -1.0, -1.0 },{ 1.0,  1.0, -1.0 },{ -1.0,  1.0, -1.0 }
};


const GLuint indices[6][6] = {
	// Front face
	{ 0, 1, 2, 2, 3, 0 },
	// Top face
	{ 1, 5, 6, 6, 2, 1 },
	// Back face
	{ 7, 6, 5, 5, 4, 7 },
	// Bottom face
	{ 4, 0, 3, 3, 7, 4 },
	// Left face
	{ 4, 5, 1, 1, 0, 4 },
	// Right face
	{ 3, 2, 6, 6, 7, 3 }
};
