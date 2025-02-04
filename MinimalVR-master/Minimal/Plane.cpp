#include "Plane.h"

using namespace std;

Plane::Plane()
{
	toWorld = glm::mat4(1.0f);


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Plane::draw(GLuint program, glm::mat4 V, glm::mat4 P, int test)
{
	glUseProgram(program);

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, test);

	//glm::mat4 temp = P;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &P[0][0]);
	glUniform3f(glGetUniformLocation(program, "eyePos"), pe.x, pe.y, pe.z);
	glUniform3f(glGetUniformLocation(program, "planeN"), vn.x, vn.y, vn.z);

	// Draw the triangles !
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glBindVertexArray(0);
	glUseProgram(0);
}

void Plane::setPoints()
{
	pa = glm::vec3(toWorld * glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f));
	pb = glm::vec3(toWorld * glm::vec4(1.0f, -1.0f, 0.0f, 1.0f));
	pc = glm::vec3(toWorld * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f));
	pd = glm::vec3(toWorld * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	//print(pa);
	//print(pb);
	//print(pc);
}

void Plane::setBasis()
{
	vr = glm::normalize(pb - pa);
	vu = glm::normalize(pc - pa);
	vn = glm::normalize(glm::cross(vr, vu));

	//cout << vn.x << " " << vn.y << " " << vn.z << endl;

	//print(vr);
	//print(vu);
	//print(vn);
}

void Plane::setEye(glm::vec3 eye)
{
	pe = eye;
	//print(pe);
}

void Plane::offAxisComputation()
{
	P_final = computeP() * computeM() * computeT();
}

glm::mat4 Plane::computeP()
{
	glm::mat4 P_(0.0f);

	glm::vec3 va, vb, vc;
	va = pa - pe;
	vb = pb - pe;
	vc = pc - pe;

	float d = -glm::dot(vn, va);
	//cout << d << endl;
	float n = 0.01f;//-------------------------------???
	float f = 1000.0f;//-------------------------------???

	float l = glm::dot(vr, va) * n / d;
	float r = glm::dot(vr, vb) * n / d;
	float b = glm::dot(vu, va) * n / d;
	float t = glm::dot(vu, vc) * n / d;

	//P_ = glFrustum(l, r, b, t, n, f);
	P_[0][0] = 2 * n / (r - l);
	P_[1][1] = 2 * n / (t - b);
	P_[2][0] = (r + l) / (r - l);
	P_[2][1] = (t + b) / (t - b);
	P_[2][2] = -(f + n) / (f - n);
	P_[2][3] = -1.0f;
	P_[3][2] = -(2 * f * n) / (f - n);

	return P_;
}

glm::mat4 Plane::computeM()
{
	glm::mat4 M_(1.0f);
	M_[0] = glm::vec4(vr, 0.0f);
	M_[1] = glm::vec4(vu, 0.0f);
	M_[2] = glm::vec4(vn, 0.0f);
	M_ = glm::transpose(M_);

	return M_;
}

glm::mat4 Plane::computeT()
{
	glm::mat4 T_(1.0f);
	T_[3] = glm::vec4(-pe, 1.0f);
	return T_;
}

void Plane::print(glm::vec3 v)
{
	cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << endl;
}
