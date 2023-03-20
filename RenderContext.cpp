#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "RenderContext.h"

RenderContext::RenderContext(ERenderMode RenderMode, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize)
{
	CurMode = RenderMode;

	// 创建VAO并绑定
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// 创建VBO并把顶点数组复制到缓冲中供OpenGL使用
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexList, GL_STATIC_DRAW);

	// 创建EBO并复制到缓冲
	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndicesSize, IndicesList, GL_STATIC_DRAW);

	SetVertexAttri();
}

RenderContext::RenderContext(ERenderMode RenderMode, float VertexList[], unsigned int VertexSize)
{
	CurMode = RenderMode;

	// 创建VAO并绑定
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// 创建VBO并把顶点数组复制到缓冲中供OpenGL使用
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexList, GL_STATIC_DRAW);

	SetVertexAttri();
}

RenderContext::~RenderContext()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}

void RenderContext::SetVertexAttri()
{
	switch (CurMode)
	{
	case EBasic_Triangle_2D:
		SetVertexPosAttri();
		break;
	case EBasic_Rectangle_2D:
		SetVertexPosAttri();
		break;
	case ETriangle_ColorVert:
		SetVertexPosColorAttri();
		break;
	default:
		SetVertexPosAttri();
		break;
	}
}

void RenderContext::SetVertexPosAttri()
{
	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void RenderContext::SetVertexPosColorAttri()
{
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void RenderContext::DrawElements(bool bPolygonMode)
{	
	glBindVertexArray(this->VAO);

	// 绘制线框模式, 调试用
	if (bPolygonMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	switch (CurMode)
	{
	case EBasic_Triangle_2D:
		glDrawArrays(GL_TRIANGLES, 0, 3);
		break;
	case EBasic_Rectangle_2D:
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		break;
	default:
		glDrawArrays(GL_TRIANGLES, 0, 3);
		break;
	}

	glBindVertexArray(0);
}
