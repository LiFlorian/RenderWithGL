#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"

using namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};

struct Texture {
	unsigned int ID;
	string type;
	aiString path;
};


class MeshRender
{
public:
	
	vector<Vertex> vertices;

	vector<unsigned int> indices;

	vector<Texture> textures;

public:

	MeshRender(float VertexList[], unsigned int VertexSize);

	MeshRender(vector<Vertex> InVertices);

	MeshRender(vector<Vertex> InVertices, vector<unsigned int> InIndices);
	
	MeshRender(vector<Vertex> InVertices, vector<unsigned int> InIndices, vector<Texture> InTextures);

	void Draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
private:
	
	unsigned int VAO, VBO, EBO;
	
	void SetupMesh();
};

