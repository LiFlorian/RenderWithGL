#pragma once
class FrameBuffer
{
public:
	unsigned int FBO;

	unsigned int TexAttached;

	unsigned int RBO;

public:

	// 普通FBO构造
	FrameBuffer(bool bAttachRBO, const float SCR_WIDTH, const float SCR_HEIGHT);

	// MSAA专属FBO构造
	FrameBuffer(bool bAttachRBO, int sampleNum, const float SCR_WIDTH, const float SCR_HEIGHT);
};

