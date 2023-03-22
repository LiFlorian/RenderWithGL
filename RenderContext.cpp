#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderContext.h"
#include "stb_image.h"

RenderContext::RenderContext(Shader* ContextShader, EVertexType VertexType, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize)
{
	CurShader = ContextShader;

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

	SetVertexAttri(VertexType);
}

RenderContext::RenderContext(Shader* ContextShader, EVertexType VertexType, float VertexList[], unsigned int VertexSize)
{
	CurShader = ContextShader;

	// 创建VAO并绑定
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	// 创建VBO并把顶点数组复制到缓冲中供OpenGL使用
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexList, GL_STATIC_DRAW);

	SetVertexAttri(VertexType);
}

RenderContext::~RenderContext()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}

void RenderContext::SetVertexAttri(EVertexType VertexType)
{
	switch (VertexType)
	{
	case EPos:
		SetVertexPosAttri();
		break;
	case EPos_Color:
		SetVertexPosColorAttri();
		break;
	case EPos_Color_Tex:
		SetVertexPosColorTexAttri();
		break;
	case EPos_Tex:
		SetVertexPosTexAttri();
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

void RenderContext::SetVertexPosColorTexAttri()
{
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 纹理属性
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void RenderContext::SetVertexPosTexAttri()
{
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 纹理属性
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void RenderContext::ActiveTexture(unsigned int Texture)
{
	glActiveTexture(GL_TEXTURE0 + this->TexIndex);
	glBindTexture(GL_TEXTURE_2D, Texture);

	this->TexIndex += 1;
}

void RenderContext::SetVertexTransform(glm::mat4 view, glm::mat4 projection)
{
	// Model矩阵
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	int modelLoc = glGetUniformLocation(CurShader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// View矩阵
	int viewLoc = glGetUniformLocation(CurShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Projection矩阵
	int projectionLoc = glGetUniformLocation(CurShader->ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void RenderContext::DrawElements(bool bPolygonMode, EDrawType DrawType)
{	
	glBindVertexArray(this->VAO);

	// 绘制线框模式, 调试用
	if (bPolygonMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	switch (DrawType)
	{
	case ETriangle:
		glDrawArrays(GL_TRIANGLES, 0, 3);
		break;
	case ERectangle:
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		break;
	case ECube:
		glDrawArrays(GL_TRIANGLES, 0, 36);
		break;
	default:
		glDrawArrays(GL_TRIANGLES, 0, 3);
		break;
	}

	glBindVertexArray(0);
}
