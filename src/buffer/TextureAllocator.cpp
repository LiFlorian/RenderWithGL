#include "TextureAllocator.h"

GLuint TextureAllocator::GenTex(GLuint width, GLuint height, unsigned char* data, TexParam* param)
{
	GLuint TextureID;

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, param->precision, width, height, 0, param->channel, GL_FLOAT, data);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, param->precision, width, height, 0, param->channel, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param->wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param->wrapMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param->mipMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param->mipMode);

	if (param->bMipmap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	};

	glBindTexture(GL_TEXTURE_2D, 0);

	return TextureID;
}

GLuint TextureAllocator::GenCubeMap(GLuint width, GLuint height, TexParam* param)
{
	GLuint CubeMapID;

	glGenTextures(1, &CubeMapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapID);

	for (unsigned int i = 0; i < 6; ++i)
	{	
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, param->precision, width, height, 0, param->channel, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, param->wrapMode);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, param->wrapMode);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, param->wrapMode);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, param->mipMode);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, param->mipMode);

	if (param->bMipmap)
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	};

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return CubeMapID;
}
