#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>

void InitGLFW();

GLFWwindow* CreateWindow();

// ���ڳߴ�ı�ص�, ��ʼ��ʱͬ����ִ��
void framebuffer_size_callback(GLFWwindow* window, int width, int height);



