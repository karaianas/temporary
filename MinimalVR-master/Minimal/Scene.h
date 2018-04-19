#pragma once
#include "Model.h"

class Scene {
public:
	Scene();
	//~Scene();

	void draw(glm::mat4 V, glm::mat4 P);
	void drawCursor(glm::mat4 V, glm::mat4 P, glm::vec3 rHandPos, bool trigState);
	void testIntersection(glm::vec3 rHandPos);

private:
	GLuint program;

	float radius;

	vector<glm::mat4> instances;
	vector<glm::vec3> colors;
	Model* sphere;

	Model* cursor;
	glm::vec3 cursorColor;
	glm::mat4 cursorM;
};