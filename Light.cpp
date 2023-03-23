#include "Light.h"

Light::Light(Shader* InLightShader, float VertexList[], unsigned int VertexSize, glm::vec3 InPos, glm::vec3 InColor)
{
	CurShader = InLightShader;
	Pos = InPos;
	Color = InColor;

	CurShader->Use();
	CurShader->SetVec3("LightColor", Color);

	// 创建VAO并绑定
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 创建VBO并把顶点数组复制到缓冲中供OpenGL使用
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexList, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

Light::~Light()
{
	glDeleteVertexArrays(1, &VAO);
}

void Light::Draw(glm::mat4 view, glm::mat4 projection)
{
	// Model矩阵
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, Pos);
	model = glm::scale(model, glm::vec3(0.2f));

	int modelLoc = glGetUniformLocation(CurShader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// View矩阵
	int viewLoc = glGetUniformLocation(CurShader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Projection矩阵
	int projectionLoc = glGetUniformLocation(CurShader->ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// 绘制
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
