#include "Light.h"

Light::Light(Shader* InLightShader, glm::vec3 InPos, glm::vec3 InColor)
{
	CurShader = InLightShader;
	Pos = InPos;
	Color = InColor;

	CurShader->Use();
	CurShader->SetVec3("LightColor", Color);
}

Light::~Light()
{
	glDeleteVertexArrays(1, &VAO);
}

void Light::BindBufferData(unsigned int VBO)
{
	// 创建VAO并绑定
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
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
