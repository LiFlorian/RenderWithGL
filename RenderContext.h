#pragma once

enum ERenderMode
{
	EBasic_Triangle_2D,
	EBasic_Rectangle_2D,
};

class RenderContext
{
public:

	RenderContext(ERenderMode RenderMode, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize);
	RenderContext(ERenderMode RenderMode, float VertexList[], unsigned int VertexSize);

	~RenderContext();

	void SetVertexAttri();

	void SetSimpleTriangleAttri();

	void DrawElements(bool bPolygonMode);

public:
	unsigned int VAO;

	unsigned int VBO;

	unsigned int EBO;

	ERenderMode CurMode;
};
