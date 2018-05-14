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

	void draw(GLuint program, glm::mat4 V, glm::mat4 P, int test);
	
	void setPoints();
	void setBasis();
	void setEye(glm::vec3 eye);

	void offAxisComputation();
	glm::mat4 computeP();
	glm::mat4 computeM();
	glm::mat4 computeT();

	void print(glm::vec3 v);
	
//private:
	GLuint VAO, VBO, EBO;
	glm::mat4 toWorld;

	glm::vec3 pa, pb, pc, pd;
	glm::vec3 vr, vu, vn;
	glm::vec3 pe;

	//glm::mat4 P_;
	//glm::mat4 M_;
	//glm::mat4 T_;
	glm::mat4 P_final;
};

static const GLfloat g_quad_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
};