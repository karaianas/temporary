#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>


using namespace std;

class Cube
{
public:
	Cube();
	~Cube();

	glm::mat4 toWorld;
	float scaleFactor;

	void draw(GLuint program, glm::mat4 V, glm::mat4 P);
	void update(float cubeSize);
	void translate(glm::vec3 t);

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO;
	GLuint texID;

	GLuint loadTexture(vector<const char*> faces);
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


