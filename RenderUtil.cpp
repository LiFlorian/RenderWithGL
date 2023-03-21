#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "RenderUtil.h"
#include "stb_image.h"


unsigned int LoadTexture(const char* ImagePath, int ColorMode, bool bFlip)
{
	if (bFlip) {
		stbi_set_flip_vertically_on_load(true);
	}

	unsigned int Texture;

	// 创建Texture缓冲并绑定
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	// 为当前绑定的纹理对象设置环绕
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// 为当前纹理设置mipmap过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(ImagePath, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, ColorMode, width, height, 0, ColorMode, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return Texture;
}
