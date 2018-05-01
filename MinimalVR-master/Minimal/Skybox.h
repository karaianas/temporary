#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>


using namespace std;

class Skybox {
public:
	Skybox();
	~Skybox();

	glm::mat4 toWorld;

	void draw(GLuint program, glm::mat4 V, glm::mat4 P);

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
	{7, 4, 5, 5, 6, 7},
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
	/*
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
	*/
	};
};

