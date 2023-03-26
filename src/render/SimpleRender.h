#pragma once

#include<iostream>
#include <vector>



class SimpleRender
{
public:

	SimpleRender(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize);

	SimpleRender(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize);

	~SimpleRender();

	void BindTexture(unsigned int Texture);

	void BindCubeMap(unsigned int Texture);

	void Draw(bool bPolygonMode);


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
