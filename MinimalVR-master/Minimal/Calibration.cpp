#include "Calibration.h"

#define VERTEX_SHADER_PATH "shaders//model.vert"
#define FRAGMENT_SHADER_PATH "shaders//model.frag"
#define VERTEX_SHADER_PATH2 "shaders//skybox.vert"
#define FRAGMENT_SHADER_PATH2 "shaders//skybox.frag"

Calibration::Calibration()
{
	program = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	program_sky = LoadShaders(VERTEX_SHADER_PATH2, FRAGMENT_SHADER_PATH2);

	cube = new Cube();
	cube2 = new Cube();
	skybox_l = new Skybox();
	skybox_r = new Skybox();

	glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	glm::mat4 S2 = glm::scale(glm::mat4(1.0f), glm::vec3(5.f, 5.f, 5.f));
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), 3.141592f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.4f));
	cube->toWorld = T * cube->toWorld * S;
	cube2->toWorld = T * cube->toWorld;
	skybox_l->toWorld = R * S2;
	skybox_r->toWorld = R * S2;

	vector<const char*> faces;
	faces.push_back("textures//vr_test_pattern.ppm");
	faces.push_back("textures//vr_test_pattern.ppm");
	faces.push_back("textures//vr_test_pattern.ppm");
	faces.push_back("textures//vr_test_pattern.ppm");
	faces.push_back("textures//vr_test_pattern.ppm");
	faces.push_back("textures//vr_test_pattern.ppm");
	cube->loadTexture(faces);
	cube2->loadTexture(faces);

	vector<const char*> faces_l;
	faces_l.push_back("textures//bear-stereo-cubemaps//left-ppm//px.ppm");
	faces_l.push_back("textures//bear-stereo-cubemaps//left-ppm//nx.ppm");
	faces_l.push_back("textures//bear-stereo-cubemaps//left-ppm//py.ppm");
	faces_l.push_back("textures//bear-stereo-cubemaps//left-ppm//ny.ppm");
	faces_l.push_back("textures//bear-stereo-cubemaps//left-ppm//pz.ppm");
	faces_l.push_back("textures//bear-stereo-cubemaps//left-ppm//nz.ppm");
	skybox_l->loadTexture(faces_l);

	vector<const char*> faces_r;
	faces_r.push_back("textures//bear-stereo-cubemaps//right-ppm//px.ppm");
	faces_r.push_back("textures//bear-stereo-cubemaps//right-ppm//nx.ppm");
	faces_r.push_back("textures//bear-stereo-cubemaps//right-ppm//py.ppm");
	faces_r.push_back("textures//bear-stereo-cubemaps//right-ppm//ny.ppm");
	faces_r.push_back("textures//bear-stereo-cubemaps//right-ppm//pz.ppm");
	faces_r.push_back("textures//bear-stereo-cubemaps//right-ppm//nz.ppm");
	skybox_r->loadTexture(faces_r);
}

void Calibration::draw(glm::mat4 V, glm::mat4 P, int eye, bool obj)
{
	if (eye == 0)
	{
		glUseProgram(program_sky);
		skybox_l->draw(program, V, P);
	}
	else if (eye == 1)
	{
		glUseProgram(program_sky);
		skybox_r->draw(program, V, P);
	}

	if (obj)
	{
		glUseProgram(program);
		cube->draw(program, V, P);
		cube2->draw(program, V, P);
	}


	//glUseProgram(program_sky);
	//skybox_l->draw(program, V, P);
	//skybox_r->draw(program, V, P);
}
