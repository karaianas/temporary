#include "Calibration.h"

#define VERTEX_SHADER_PATH "shaders//model.vert"
#define FRAGMENT_SHADER_PATH "shaders//model.frag"

Calibration::Calibration()
{
	program = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	cube = new Cube();
	cube2 = new Cube();

	glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.4f));
	cube->toWorld = T * cube->toWorld * S;
	cube2->toWorld = T * cube->toWorld;
}

void Calibration::draw(glm::mat4 V, glm::mat4 P)
{
	glUseProgram(program);
	cube->draw(program, V, P);
	cube2->draw(program, V, P);
}
