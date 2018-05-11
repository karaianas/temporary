#pragma once
#include "Model.h"
#include "Cube.h"
#include "Skybox.h"
#include "shader.h"
#include "Plane.h"

using namespace std;

class Cave {
public:
	Cave();
	void createFB();
	void drawMainScene(glm::mat4 V, glm::mat4 P, int test);
	void drawTexture(glm::mat4 V, glm::mat4 P, int test);

private:
	GLuint program;
	GLuint program_sky;
	GLuint program_cont;
	GLuint program_plane;
	Cube* cube;
	Cube* cube2;
	Skybox* skybox_l;
	Skybox* skybox_r;
	Skybox* skybox_x;
	vector<Skybox*> skyboxes;
	Model* controller;
	Plane* plane;

	GLuint FBO, TBO, RBO;
};