#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

using namespace std;

class Pyramid {
public:
	Pyramid(vector<glm::vec3> corners);

	void setEye(glm::vec3 eye);
	void draw(GLuint program, glm::mat4 V, glm::mat4 P, bool lr);

private:
	glm::mat4 toWorld;
	GLuint VAO, VBO, EBO;
	vector<glm::vec3> vertices;


	const GLuint indices[4][3] = {
	{0, 1, 4},
	{1, 2, 4},
	{2, 3, 4},
	{3, 0, 4},
	};
};