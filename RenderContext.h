#pragma once

enum EVertexType
{
	EPos,
	EPos_Color,
	EPos_Color_Tex,
	EPos_Tex,
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

	RenderContext(EVertexType InVertexType, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize);
	RenderContext(EVertexType InVertexType, float VertexList[], unsigned int VertexSize);

	~RenderContext();

	void DrawElements(bool bPolygonMode, EDrawType DrawType);

	void SetVertexAttri(EVertexType VertexType);

	void SetVertexPosAttri();

	void SetVertexPosColorAttri();

	void SetVertexPosColorTexAttri();

	void SetVertexPosTexAttri();

	void ActiveTexture(unsigned int Texture);


public:
	unsigned int VAO;

	unsigned int VBO;

	unsigned int EBO;

	unsigned int TexIndex = 0;
};
