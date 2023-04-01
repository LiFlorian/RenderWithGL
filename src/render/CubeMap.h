#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class CubeMap
{
public:
	GLuint ID;

	GLuint Width;

	GLuint Height;

	float NearPlane;

	float FarPlane;

	glm::mat4 captureProjection;

	std::vector<glm::mat4> captureViews;

public:

	CubeMap(GLuint InWidth = 512, GLuint InHeight = 512, float InNearPlane = 0.1f, float InFarPlane = 10.0f);

private:

	void GenCubeMap();

	void GenMatrix();
};

