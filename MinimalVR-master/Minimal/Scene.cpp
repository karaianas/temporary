#include "Scene.h"
#include <time.h> 

#define VERTEX_SHADER_PATH "shaders//model.vert"
#define FRAGMENT_SHADER_PATH "shaders//model.frag"

Scene::Scene()
{
	program = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

	sphere = new Model("models//sphere//sphere.obj");
	cursor = new Model("models//sphere//sphere.obj");
	popcorn = new Model("models//sphere//sphere.obj");


	radius = 0.07f/2.0f;
	float scaleFactor2 = 0.28f / 2.0f;
	cursorM = glm::scale(glm::mat4(1.0f), glm::vec3(radius));

	for (int i = -2; i <= 2; i++)
		for (int j = -2; j <= 2; j++)
			for (int k = -2; k <= 2; k++)
			{
				glm::mat4 T = glm::translate(glm::mat4(1.0f), scaleFactor2 * glm::vec3(float(i), float(j), float(k)));
				glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(radius));
				//glm::mat4 T2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));

				instances.push_back(T * S);
			}

	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
			for (int k = -1; k <= 1; k++)
			{
				glm::mat4 T = glm::translate(glm::mat4(1.0f), 0.04f * glm::vec3(float(i), float(j), float(k)));
				glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
				popcorns.push_back(T * S);
			}

	counter = 0;
	randomEasy();
}

void Scene::draw(glm::mat4 V, glm::mat4 P)
{
	glUseProgram(program);
	for(int i = 0; i < instances.size(); i++)
	//for (auto M : instances)
	{
		// normalize
		sphere->Draw(program, instances[i], V, P, colors[i]);// glm::normalize(glm::vec3(abs(M[3][0]), abs(M[3][1]), abs(M[3][2]))));
	}
}

void Scene::drawCursor(glm::mat4 V, glm::mat4 P, glm::vec3 rHandPos, bool trigState)
{
	glUseProgram(program);
	cursorM[3] = glm::vec4(rHandPos, 1.0f);
	if (trigState)
	{
		cursor->Draw(program, cursorM, V, P, cursorColor);
		testIntersection(rHandPos, V, P);
	}
	else
		cursor->Draw(program, cursorM, V, P, glm::vec3(0.0f));
}

void Scene::drawPopcorns(glm::mat4 M, glm::mat4 V, glm::mat4 P)
{
	glUseProgram(program);
	for (int i = 0; i < popcorns.size(); i++)
	{
		glm::mat4 T(1.0f);
		T[3] = M[3];
		popcorn->Draw(program, T * popcorns[i], V, P, colors[highlight]);
	}

	//// Wait
	//clock_t start = clock();
	//clock_t end = clock();
	//while ((end - start) / CLOCKS_PER_SEC < 3.0)
	//{
	//	end = clock();
	//	//cout << (start - end) / CLOCKS_PER_SEC << endl;
	//}

	// Reshuffle
	randomEasy();
}

void Scene::testIntersection(glm::vec3 rHandPos, glm::mat4 V, glm::mat4 P)
{
	glm::vec3 center(instances[highlight][3]);
	if (glm::length(center - rHandPos) < radius)
	{
		// Pop			
		drawPopcorns(instances[highlight], V, P);

		//instances.erase(instances.begin() + i);
		//colors.erase(colors.begin() + i);
	}
}

void Scene::randomEasy()
{
	++counter;
	vector<int> red;
	vector<int> green;
	vector<int> blue;

	for (int i = 0; i < 125; i++)
	{
		red.push_back(i);
		green.push_back(i);
		blue.push_back(i);
	}
	random_shuffle(red.begin(), red.end());
	random_shuffle(green.begin(), green.end());
	random_shuffle(blue.begin(), blue.end());

	float factor = 1.0f / 124.0f;
	srand(time(NULL));
	highlight = rand() % 125;

	colors.clear();
	for (int i = 0; i < 125; i++)
	{
		if (i == highlight)
			colors.push_back(glm::vec3(1.0f));
		else
			colors.push_back(factor * glm::vec3((float)red[i], (float)green[i], (float)blue[i]));
	}
	cursorColor = colors[highlight];
}
