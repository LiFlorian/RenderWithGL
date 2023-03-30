#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class SphereRender
{
public:

	GLuint XSegments;

	GLuint YSegments;

	GLuint IndexCount;

	float Radius;

public:

	SphereRender(float InRadius = 1.0f, GLuint InXSegments = 64, GLuint InYSegments = 64);

	void Draw(Shader* shader, glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

private:

	GLuint VAO;

	GLuint VBO;

	GLuint EBO;

private:

	void SetupMesh();
};

