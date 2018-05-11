#pragma once
//#include <GL/glew.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
//#include <glm/gtc/constants.hpp>
#include "Cube.h"
#include "Skybox.h"
#include "shader.h"
#include "Plane.h"

class Calibration {
public:
	Calibration();

	void createFB();

	void draw(glm::mat4 V, glm::mat4 P, int eye, bool obj, bool myScene);

	void drawLeftEye(glm::mat4 V, glm::mat4 P, int skybox);
	void drawRightEye(glm::mat4 V, glm::mat4 P, int skybox);
	void drawCube(glm::mat4 V, glm::mat4 P, int eye);
	void drawController(glm::mat4 M, glm::mat4 V, glm::mat4 P);
	void drawPlane(glm::mat4 V, glm::mat4 P, int tex);
	void changeCubeSize(float cubeSize);

	void startRender();
	void stopRender();

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

	// ---------------
	GLuint FBO, TBO, RBO;
};