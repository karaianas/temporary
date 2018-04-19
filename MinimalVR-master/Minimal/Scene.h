#pragma once
#include "Model.h"

class Scene {
public:
	Scene();
	~Scene();

private:
	GLuint program;
	Model* sphere;
	vector<glm::mat4> instances;
};