#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>

void InitGLFW();

GLFWwindow* CreateWindow();

// 窗口尺寸改变回调, 初始化时同样会执行
void framebuffer_size_callback(GLFWwindow* window, int width, int height);



