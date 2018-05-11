#include "Calibration.h"

#define VERTEX_SHADER_PATH "shaders//model.vert"
#define FRAGMENT_SHADER_PATH "shaders//model.frag"
#define VERTEX_SHADER_PATH2 "shaders//skybox.vert"
#define FRAGMENT_SHADER_PATH2 "shaders//skybox.frag"
#define VERTEX_SHADER_PATH3 "shaders//cont.vert"
#define FRAGMENT_SHADER_PATH3 "shaders//cont.frag"
#define PVERTEX_SHADER_PATH "shaders//pass.vert"
#define PFRAGMENT_SHADER_PATH "shaders//pass.frag"

Calibration::Calibration()
{
	program = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	program_sky = LoadShaders(VERTEX_SHADER_PATH2, FRAGMENT_SHADER_PATH2);
	program_cont = LoadShaders(VERTEX_SHADER_PATH3, FRAGMENT_SHADER_PATH3);
	program_plane = LoadShaders(PVERTEX_SHADER_PATH, PFRAGMENT_SHADER_PATH);

	cube = new Cube();
	cube2 = new Cube();
	skybox_l = new Skybox();
	skybox_r = new Skybox();
	skybox_x = new Skybox();
	controller = new Model("models//sphere//sphere2.obj");

	plane = new Plane();
	createFB();

	glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	glm::mat4 S2 = glm::scale(glm::mat4(1.0f), glm::vec3(5.f, 5.f, 5.f));
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), 3.141592f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));//-0.4f
	cube->toWorld = T * cube->toWorld * S;
	cube2->toWorld = T * cube->toWorld;
	skybox_l->toWorld = R * S2;
	skybox_r->toWorld = R * S2;
	skybox_x->toWorld = R * S2;

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

	vector<const char*> faces_x;
	faces_x.push_back("textures//room//px.ppm");
	faces_x.push_back("textures//room//nx.ppm");
	faces_x.push_back("textures//room//py.ppm");
	faces_x.push_back("textures//room//ny.ppm");
	faces_x.push_back("textures//room//pz.ppm");
	faces_x.push_back("textures//room//nz.ppm");
	skybox_x->loadTexture(faces_x);

	skyboxes.push_back(skybox_l);
	skyboxes.push_back(skybox_r);
	skyboxes.push_back(skybox_x);
}

void Calibration::createFB()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create a color attachment texture
	glGenTextures(1, &TBO);
	glBindTexture(GL_TEXTURE_2D, TBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1000, 1000, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TBO, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1000, 1000); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it
																								  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Calibration::draw(glm::mat4 V, glm::mat4 P, int eye, bool obj, bool myScene)
{
	if (eye == 0)
	{
		glUseProgram(program_sky);
		if (!myScene)
			skybox_l->draw(program, V, P);
		else
			skybox_x->draw(program, V, P);
	}
	else if (eye == 1)
	{
		glUseProgram(program_sky);
		if (!myScene)
			skybox_r->draw(program, V, P);
		else
			skybox_x->draw(program, V, P);
	}

	if (obj)
	{
		glUseProgram(program);
		cube->draw(program, V, P);
		cube2->draw(program, V, P);
	}
}

void Calibration::drawLeftEye(glm::mat4 V, glm::mat4 P, int skybox)
{
	glUseProgram(program_sky);
	skyboxes[skybox]->draw(program_sky, V, P);
}

void Calibration::drawRightEye(glm::mat4 V, glm::mat4 P, int skybox)
{
	glUseProgram(program_sky);
	skyboxes[skybox]->draw(program_sky, V, P);
}

void Calibration::drawCube(glm::mat4 V, glm::mat4 P, int eye)
{
	cube->draw(program, V, P);
	cube2->draw(program, V, P);
}

void Calibration::drawController(glm::mat4 M, glm::mat4 V, glm::mat4 P)
{
	glUseProgram(program_cont);
	controller->Draw(program_cont, M, V, P, glm::vec3(1.0f));
}

void Calibration::drawPlane(glm::mat4 V, glm::mat4 P, int tex)
{
	plane->draw(program_plane, V, P, TBO);
}

void Calibration::changeCubeSize(float cubeSize)
{
	cube->update(cubeSize);
	cube2->update(cubeSize);
}

void Calibration::startRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Calibration::stopRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); 
	glClear(GL_COLOR_BUFFER_BIT);
}
