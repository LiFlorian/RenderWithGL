#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GBuffer
{
public:
	GLuint ID;

	GLuint gPosition; // 位置纹理, 高精度
	GLuint gNormal; // 法线纹理, 高精度
	GLuint gColorSpec; // 颜色&高光纹理, 高光保存在Alpha通道中, 默认进度, 8位浮点数

	GLuint gDepthRBO; // 深度纹理, 使用RBO

public:

	GBuffer(float SCR_WIDTH, float SCR_HEIGHT);

	void ApplyBuffers();
};

