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
#include "MeshRender.h"

using namespace std;

class ModelRender
{
public:
	/*  函数   */
	ModelRender(char* path);

	void Draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

	void DrawShape();

private:
	/*  模型数据  */
	vector<MeshRender> meshes;

	vector<Texture> textures_loaded;

	string directory;

	/*  函数   */
	void loadModel(string path);

	void processNode(aiNode* node, const aiScene* scene);

	MeshRender processMesh(aiMesh* mesh, const aiScene* scene);

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

