#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "RenderContext.h"
#include "Data.h"

RenderContext::RenderContext()
{
	BindVertexData();
}

RenderContext::~RenderContext()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}

void RenderContext::BindVertexData()
{
	// 创建VAO并绑定
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// 创建VBO并把顶点数组复制到缓冲中供OpenGL使用
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Rectangle_2D), Rectangle_2D, GL_STATIC_DRAW);

	// 创建EBO并复制到缓冲
	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Rectangle_2D_Indices), Rectangle_2D_Indices, GL_STATIC_DRAW);

	// 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
