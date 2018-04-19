#include "Scene.h"

#define VERTEX_SHADER_PATH "..//shaders//model.vert"
#define FRAGMENT_SHADER_PATH "..//shaders//model.frag"

Scene::Scene()
{
	program = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	sphere = new Model("..//models//sphere.obj");
	float scaleFactor = 0.07f;
	for (int i = -2; i <= 2; i++)
		for (int j = -2; j <= 2; j++)
			for (int k = -2; k <= 2; k++)
			{
				glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(float(i) * 0.28, float(j) * 0.28, float(k) * 0.28));
				glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor));

				instances.push_back(T * S);
			}
}
