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
	/*  网格数据  */
	vector<Vertex> vertices;

	vector<unsigned int> indices;

	vector<Texture> textures;
	/*  函数  */
	MeshRender(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

	void Draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
private:
	/*  渲染数据  */
	unsigned int VAO, VBO, EBO;
	/*  函数  */
	void SetupMesh();
};

