#pragma once

enum ERenderMode
{
	EBasic_Triangle_2D,
	EBasic_Rectangle_2D,
	ETriangle_ColorVert,
};

class RenderContext
{
public:

	RenderContext(ERenderMode RenderMode, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize);
	RenderContext(ERenderMode RenderMode, float VertexList[], unsigned int VertexSize);

	~RenderContext();

	void DrawElements(bool bPolygonMode);

	void SetVertexAttri();

	void SetVertexPosAttri();

	void SetVertexPosColorAttri();

public:
	unsigned int VAO;

	unsigned int VBO;

	unsigned int EBO;

	ERenderMode CurMode;
};
