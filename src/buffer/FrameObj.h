#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>

struct FrameParam {
	GLenum RBOFormat;
	GLenum RBOType;
};

class FrameObj
{
public:

	GLuint FBO;

	GLuint RBO;

	std::vector<GLuint> TexAttachList;

public:

	FrameObj(GLuint width, GLuint height, FrameParam* frameParam);

	void BindTexAttached(GLuint InTex, GLuint mipLevel = 0);
};

