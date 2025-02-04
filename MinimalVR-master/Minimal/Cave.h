#pragma once
#include "Model.h"
#include "Cube.h"
#include "Skybox.h"
#include "shader.h"
#include "Plane.h"
#include "Pyramid.h"
#include "AvaHand.h"

using namespace std;

class Cave {
public:
	Cave();
	void createFB();
	void drawScene(glm::mat4 V, glm::mat4 P, bool isEC);
	void drawMainScene(glm::mat4 V, glm::mat4 P, int FBO_, int randEye, int randPlane);
	void drawTexture(glm::mat4 V, glm::mat4 P, int FBO_, int id, int randEye, int randPlane);
	void drawController(glm::mat4 M, glm::mat4 V, glm::mat4 P);
	void drawPyramid(glm::mat4 V, glm::mat4 P, glm::vec3 pos, bool lr);
	void drawHands(glm::mat4 view, glm::mat4 proj, glm::vec3 eyePos, ovrTrackingState trackState, ovrInputState inputState);

	void setViewport(int w0_, int h0_);
	void setEye(glm::vec3 eyePos);
	void setViewMatrix(glm::mat4 V);
	void changeCubeSize(float cubeSize);
	void moveCubePos(glm::vec3 t);



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
	Model* ucsd;
	GLuint saveFBO;
	GLuint FBO2;
	
	// Frame buffer rendering
	int w0, h0, w1, h1;
	GLuint program_plane;
	Plane* plane_L, *plane_R, *plane_B;
	vector<Plane*> planes;
	GLuint FBO, TBO, RBO;
	glm::mat4 V_prev;
	//Pyramid* pyramid;
	vector<Pyramid*> pyramids;

	AvaHand* avatarHands;
};