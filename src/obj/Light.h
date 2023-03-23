#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../render/Shader.h"

class Light
{
public:
	unsigned int VAO;

	unsigned int VBO;

	glm::vec3 Pos;

	glm::vec3 Color;

	Shader* CurShader;

public:
	Light(Shader* InLightShader, float VertexList[], unsigned int VertexSize, glm::vec3 InPos = glm::vec3(0), glm::vec3 InColor = glm::vec3(1.0));

	~Light();

	void Draw(glm::mat4 view, glm::mat4 projection);
};

