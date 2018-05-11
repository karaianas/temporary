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
	
	void setViewport(int w0_, int h0_);
	void setEye(glm::vec3 eyePos);


private:
	GLuint program;
	GLuint program_sky;
	GLuint program_cont;
	Cube* cube;
	Cube* cube2;
	Skybox* skybox_l;
	Skybox* skybox_r;
	Skybox* skybox_x;
	vector<Skybox*> skyboxes;
	Model* controller;
	
	// Frame buffer rendering
	int w0, h0, w1, h1;
	GLuint program_plane;
	Plane* plane_L, *plane_R, *plane_B;
	GLuint FBO, TBO, RBO;
};