#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(bool bAttachRBO, const float SCR_WIDTH, const float SCR_HEIGHT)
{
	// 创建帧缓冲对象并绑定
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// 生成帧缓冲附加纹理
	glGenTextures(1, &TexAttached);

	glBindTexture(GL_TEXTURE_2D, TexAttached);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TexAttached, 0); // 将它附加到当前绑定的帧缓冲对象

	if (bAttachRBO)
	{
		// 创建渲染缓冲对象并绑定
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // 将渲染缓冲对象设置为帧缓冲对象的深度以及模板缓冲附件
	}

	// 检查帧缓冲对象完整性
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	// 解绑帧缓冲对象操作
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::FrameBuffer(bool bAttachRBO, int sampleNum, const float SCR_WIDTH, const float SCR_HEIGHT)
{
	// 创建帧缓冲对象并绑定
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// 生成帧缓冲附加纹理
	glGenTextures(1, &TexAttached);
	// MSAA设置
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, TexAttached);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleNum, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, TexAttached, 0); // 将它附加到当前绑定的帧缓冲对象

	if (bAttachRBO)
	{
		// 创建渲染缓冲对象并绑定
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);

		// MSAA设置
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleNum, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // 将渲染缓冲对象设置为帧缓冲对象的深度以及模板缓冲附件
	}

	// 检查帧缓冲对象完整性
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	// 解绑帧缓冲对象操作
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
