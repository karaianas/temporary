#pragma once
#ifndef MODEL_H
#define MODEL_H

//#include <glad/glad.h> // holds all OpenGL type declarations
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

//unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
	/*  Model Data */
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}

	// draws the model, and thus all its meshes
	void Draw(GLuint program, glm::mat4 M, glm::mat4 V, glm::mat4 P, glm::vec3 color)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(program, M, V, P, color);
	}

	void Draw2(GLuint program, glm::mat4 M, glm::mat4 V, glm::mat4 P, glm::vec3 color)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw2(program, M, V, P, color);
	}


	//GLuint getSkybox(vector<const char*> faces);
private:
	/*  Functions   */
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const &path);

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene);

	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);

	//unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
};

#endif
