#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <regex>

#include "TextureLoader.h"
#include "stb_image.h"

using namespace std;

unsigned int TextureLoader::LoadTexture(char* ImagePath, bool bFlip)
{
	if (TexturePool.find(ImagePath) != TexturePool.end()) 
	{
		return TexturePool.find(ImagePath)->second;
	}

	if (bFlip) {
		stbi_set_flip_vertically_on_load(true);
	}

	// 创建Texture缓冲
	unsigned int TextureID;
	glGenTextures(1, &TextureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(ImagePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// 为当前绑定的纹理对象设置环绕
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// 为当前纹理设置mipmap过滤
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	TexturePool.insert(pair<char*, unsigned int>(ImagePath, TextureID));

	return TextureID;
}

unsigned int TextureLoader::LoadTextureWithChannel(char* ImagePath, int channelCount, bool bFlip)
{
	if (TexturePool.find(ImagePath) != TexturePool.end())
	{
		return TexturePool.find(ImagePath)->second;
	}

	if (bFlip) {
		stbi_set_flip_vertically_on_load(true);
	}

	// 创建Texture缓冲
	unsigned int TextureID;
	glGenTextures(1, &TextureID);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(ImagePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (channelCount == 1)
			format = GL_RED;
		else if (channelCount == 2)
			format = GL_RG;
		else if (channelCount == 3)
			format = GL_RGB;
		else if (channelCount == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// 为当前绑定的纹理对象设置环绕
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// 为当前纹理设置mipmap过滤
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	TexturePool.insert(pair<char*, unsigned int>(ImagePath, TextureID));

	return TextureID;
}

unsigned int TextureLoader::LoadCubeMap(std::vector<std::string> faceList)
{	
	// 生成CubeMap纹理并绑定
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// 遍历加载CubeMap
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faceList.size(); i++)
	{
		unsigned char* data = stbi_load(faceList[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faceList[i] << std::endl;
		}

		stbi_image_free(data);
	}

	// 设置环绕格式
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
