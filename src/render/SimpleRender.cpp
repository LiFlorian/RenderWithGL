#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SimpleRender.h"

SimpleRender::SimpleRender()
{

}

SimpleRender::SimpleRender(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize)
{
	BindVertexList(AttriDivisor, VertexList, VertexSize);
}

SimpleRender::SimpleRender(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize)
{
	BindVertexList(AttriDivisor, VertexList, VertexSize, IndicesList, IndicesSize);
}

void SimpleRender::BindVertexList(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize)
{
	// 创建VAO并绑定
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 创建VBO并把顶点数组复制到缓冲中供OpenGL使用
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexList, GL_STATIC_DRAW);

	int totalAttriNum = SetVertexAttri(AttriDivisor);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned singleVertSize = totalAttriNum * sizeof(float);
	VertNum = VertexSize / singleVertSize;
}

void SimpleRender::BindVertexList(std::vector<int> AttriDivisor, float VertexList[], unsigned int VertexSize, unsigned int IndicesList[], unsigned int IndicesSize)
{
	// 创建VAO并绑定
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 创建VBO并把顶点数组复制到缓冲中供OpenGL使用
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexList, GL_STATIC_DRAW);

	// 创建EBO并复制到缓冲
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndicesSize, IndicesList, GL_STATIC_DRAW);

	int totalAttriNum = SetVertexAttri(AttriDivisor);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned singleVertSize = totalAttriNum * sizeof(float);
	VertNum = VertexSize / singleVertSize;

	IndicesNum = IndicesSize / sizeof(float);
}


int SimpleRender::SetVertexAttri(std::vector<int> AttriDivisor)
{
	int totalAttriNum = 0;
	for (int i = 0; i < AttriDivisor.size(); i++)
	{
		totalAttriNum += AttriDivisor[i];
	}

	int startPos = 0;
	for (int i = 0; i < AttriDivisor.size(); i++)
	{
		glVertexAttribPointer(i, AttriDivisor[i], GL_FLOAT, GL_FALSE, totalAttriNum * sizeof(float), (void*)(startPos * sizeof(float)));
		glEnableVertexAttribArray(i);

		startPos += AttriDivisor[i];
	}

	return totalAttriNum;
}


SimpleRender::~SimpleRender()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}

void SimpleRender::BindTexture(unsigned int Texture)
{
	TexList.push_back(Texture);
}

void SimpleRender::BindCubeMap(unsigned int Texture)
{
	CubeMapID = Texture;
}

void SimpleRender::ActiveBindedTextures()
{
	for (int i = 0; i < TexList.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, TexList.at(i));
	}

	if (CubeMapID > 0)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapID);
	}
}

void SimpleRender::Draw(bool bPolygonMode)
{
	ActiveBindedTextures();

	// 绘制线框模式, 调试用
	if (bPolygonMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	DrawShape();

	if (bPolygonMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

// 仅绘制顶点
void SimpleRender::DrawShape()
{
	glBindVertexArray(VAO);

	if (IndicesNum > 0)
	{
		glDrawElements(GL_TRIANGLES, IndicesNum, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, VertNum);
	}

	glBindVertexArray(0);
}
