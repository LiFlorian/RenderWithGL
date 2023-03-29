#include "SSAOKernel.h"
#include <random>

SSAOKernel::SSAOKernel(GLuint InKernelSize, GLuint InNoiseSize)
{
	KernelSize = InKernelSize;

	NoiseSize = InNoiseSize;

	GenKernelList();

	GenNoiseTex();
}

void SSAOKernel::GenKernelList()
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	for (GLuint i = 0; i < KernelSize; ++i)
	{	
		// x, y分量为[-1, 1], z分量为[0, 1]， 保证样本位于切线空间的立方体内
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);

		// nomalize之后，所有的样本将进入半球范围
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);

		// 使样本尽可能靠近圆心, 提升采样效果
		GLfloat scale = GLfloat(i) / KernelSize;
		scale = LerpKernelScale(0.1f, 1.0f, scale * scale);
		sample *= scale;

		KernelList.push_back(sample);
	}
}

void SSAOKernel::GenNoiseTex()
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	// 生成切线空间绕Z轴随机转动列表
	std::vector<glm::vec3> ssaoNoise;
	for (GLuint i = 0; i < NoiseSize * NoiseSize; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);

		ssaoNoise.push_back(noise);
	}

	// 生成REPEATE形式的铺屏Noise纹理
	glGenTextures(1, &NoiseTex);
	glBindTexture(GL_TEXTURE_2D, NoiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, NoiseSize, NoiseSize, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

GLfloat SSAOKernel::LerpKernelScale(GLfloat a, GLfloat b, GLfloat f)
{
	return a + f * (b - a);
}
