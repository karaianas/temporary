#include "Scene.h"
#include <time.h> 

#define VERTEX_SHADER_PATH "shaders//model.vert"
#define FRAGMENT_SHADER_PATH "shaders//model.frag"

Scene::Scene()
{
	program = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	sphere = new Model("models//sphere//sphere.obj");
	//cursor = new Model("models//nanosuit//nanosuit.obj");
	cursor = new Model("models//sphere//sphere.obj");

	srand(time(NULL));
	int highlight = rand() % 125;
	//cout << "random number: " << highlight << endl;
	int counter = 0;

	radius = 0.07f/2.0f;
	float scaleFactor2 = 0.28f / 2.0f;
	cursorM = glm::scale(glm::mat4(1.0f), glm::vec3(radius));
	//cursorColor = glm::vec3(0.0f);

	for (int i = -2; i <= 2; i++)
		for (int j = -2; j <= 2; j++)
			for (int k = -2; k <= 2; k++)
			{
				glm::mat4 T = glm::translate(glm::mat4(1.0f), scaleFactor2 * glm::vec3(float(i), float(j), float(k)));
				glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(radius));
				//glm::mat4 T2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));

				instances.push_back(T * S);
				counter++;

				if (counter == highlight)
				{
					glm::vec3 color(1.0f, 0.0f, 0.0f);
					colors.push_back(color);
					cursorColor = color;
				}
				else
				{
					float r = ((float)rand() / (RAND_MAX)) + 1;
					float g = ((float)rand() / (RAND_MAX)) + 1;
					float b = ((float)rand() / (RAND_MAX)) + 1;
					colors.push_back(glm::vec3(r, g, b));
				}
				//colors.push_back(glm::vec3(i, j, k) + glm::vec3(2.0f));
			}
}

void Scene::draw(glm::mat4 V, glm::mat4 P)
{
	glUseProgram(program);
	for(int i = 0; i < instances.size(); i++)
	//for (auto M : instances)
	{
		sphere->Draw(program, instances[i], V, P, glm::normalize(colors[i]));// glm::normalize(glm::vec3(abs(M[3][0]), abs(M[3][1]), abs(M[3][2]))));
	}
}

void Scene::drawCursor(glm::mat4 V, glm::mat4 P, glm::vec3 rHandPos, bool trigState)
{
	glUseProgram(program);
	cursorM[3] = glm::vec4(rHandPos, 1.0f);
	if (trigState)
	{
		cursor->Draw(program, cursorM, V, P, cursorColor);
		testIntersection(rHandPos);
	}
	else
		cursor->Draw(program, cursorM, V, P, glm::vec3(0.0f));
}

void Scene::testIntersection(glm::vec3 rHandPos)
{
	for(int i = 0; i < instances.size(); i++)
	//for (auto M : instances)
	{
		glm::vec3 center(instances[i][3]);
		if (glm::length(center - rHandPos) < radius)
			colors[i] = glm::vec3(0.0f);
	}
}
