#include "FrameObj.h"

FrameObj::FrameObj(GLuint width, GLuint height, FrameParam* frameParam)
{
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);

	glRenderbufferStorage(GL_RENDERBUFFER, frameParam->RBOFormat, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, frameParam->RBOType, GL_RENDERBUFFER, RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameObj::BindTexAttached(GLuint InTex, GLuint mipLevel)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);


	TexAttachList.push_back(InTex);
	GLuint CurIndex = TexAttachList.size() - 1;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + CurIndex, GL_TEXTURE_2D, InTex, mipLevel);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
