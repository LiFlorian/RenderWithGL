#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

struct TexParam {
	GLenum precision;
	GLenum channel;
	GLenum wrapMode;
	GLenum mipMode;
	bool bMipmap;
};

class TextureAllocator
{
public:

	GLuint GenTex(GLuint width, GLuint height, unsigned char* data, TexParam* param);

	GLuint GenCubeMap(GLuint width, GLuint height, TexParam* param);
};

