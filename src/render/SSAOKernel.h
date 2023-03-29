#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class SSAOKernel
{
public:
	SSAOKernel(GLuint InKernelSize = 64, GLuint InNoiseSize = 4);

public:

	GLuint KernelSize;

	GLuint NoiseSize;

	GLuint NoiseTex;

	std::vector<glm::vec3> KernelList;

private:



private:

	void GenKernelList();

	void GenNoiseTex();

	GLfloat LerpKernelScale(GLfloat a, GLfloat b, GLfloat f);
};

