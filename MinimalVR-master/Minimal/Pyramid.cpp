#include "Plane.h"
#include "Pyramid.h"

using namespace std;

Pyramid::Pyramid(vector<glm::vec3> corners)
{
	toWorld = glm::mat4(1.0f);
	for (auto point : corners)
		vertices.push_back(point);
	vertices.push_back(glm::vec3(0.0f));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Pyramid::setEye(glm::vec3 eye)
{
	vertices[4] = eye;

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Pyramid::draw(GLuint program, glm::mat4 V, glm::mat4 P, bool lr)
{
	glUseProgram(program);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(5.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &P[0][0]);
	if(!lr)
		glUniform3f(glGetUniformLocation(program, "color"), 0.0f, 1.0f, 0.0f);
	else
		glUniform3f(glGetUniformLocation(program, "color"), 1.0f, 0.0f, 0.0f);
	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(1.0f);
}


