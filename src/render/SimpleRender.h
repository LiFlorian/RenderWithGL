#pragma once

#include<iostream>
#include <vector>
#include "Shader.h"



class SimpleRender
{
public:
	
	SimpleRender();

	SimpleRender(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize);

	SimpleRender(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize);

	~SimpleRender();

	void BindVertexList(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize);

	void BindVertexList(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize);

	void BindTexture(unsigned int Texture);

	void BindCubeMap(unsigned int Texture);

	void Draw(bool bPolygonMode);

	void DrawShape();

	virtual void Draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);


public:
	unsigned int VAO;

	unsigned int VBO;

	unsigned int EBO;

	std::vector<unsigned int> TexList;

	unsigned int CubeMapID = 0;

private:

	unsigned int VertNum = 0;

	unsigned int IndicesNum = 0;

	void ActiveBindedTextures();

	int SetVertexAttri(std::vector<int> AttriDivisor);
};
