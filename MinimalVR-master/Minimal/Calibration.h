#pragma once
//#include <GL/glew.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

//#include "Model.h"

#include "Cube.h"
#include "shader.h"

class Calibration {
public:
	Calibration();

	void draw(glm::mat4 V, glm::mat4 P);

private:
	GLuint program;
	Cube* cube;
	Cube* cube2;
};