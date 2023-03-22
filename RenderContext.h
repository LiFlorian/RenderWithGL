#pragma once

#include<iostream>
#include <vector>

#include "Shader.h"

enum EVertexType
{
	EPos,
	EPos_Color,
	EPos_Color_Tex,
	EPos_Tex,
	EPos_Normal,
	EPos_Normal_Tex,
};

enum EDrawType
{
	ETriangle,
	ERectangle,
	ECube,
};

class RenderContext
{
public:

	RenderContext(Shader* contextShader, EVertexType InVertexType, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize);
	RenderContext(Shader* contextShader, EVertexType InVertexType, float VertexList[], unsigned int VertexSize);

	~RenderContext();

	void DrawElements(bool bPolygonMode, EDrawType DrawType);

	void SetVertexAttri(EVertexType VertexType);

	void SetVertexPosAttri();

	void SetVertexPosColorAttri();

	void SetVertexPosColorTexAttri();

	void SetVertexPosTexAttri();

	void SetVertexPosNormalAttri();

	void SetVertexPosNormalTexAttri();

	void BindTexture(unsigned int Texture);

	void ActiveBindedTextures();

	void SetVertexTransform(glm::mat4 view, glm::mat4 projection);


public:
	unsigned int VAO;

	unsigned int VBO;

	unsigned int EBO;

	std::vector<unsigned int> TexList;

	Shader* CurShader;
};
