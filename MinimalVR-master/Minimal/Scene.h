#pragma once
#include "Model.h"

class Scene {
public:
	Scene();
	//~Scene();

	void draw(glm::mat4 V, glm::mat4 P);
	bool drawCursor(glm::mat4 V, glm::mat4 P, glm::vec3 rHandPos, bool trigState);
	void drawPopcorns(glm::mat4 M, glm::mat4 V, glm::mat4 P);

	bool testIntersection(glm::vec3 rHandPos, glm::mat4 V, glm::mat4 P);

	void randomEasy();
	void randomMedium();
	void randomHard();
	void randomCrazy();

	int counter;
	int level;
	bool touched;
	glm::mat4 M;

private:
	GLuint program;

	float radius;

	Model* sphere;
	vector<glm::mat4> instances;
	vector<glm::vec3> colors;

	Model* cursor;
	glm::vec3 cursorColor;
	glm::mat4 cursorM;

	Model* popcorn;
	vector<glm::mat4> popcorns;
	vector<glm::vec3> pcolors;

	int highlight;

};