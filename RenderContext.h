#pragma once

enum ERenderMode
{
	EBasic_Triangle_2D,
	EBasic_Rectangle_2D,
};

class RenderContext
{
public:

	RenderContext();

	~RenderContext();

	void BindVertexData();

public:
	unsigned int VAO;

	unsigned int VBO;

	unsigned int EBO;
};