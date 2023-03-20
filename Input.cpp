#include <GLFW/glfw3.h>

#include "Input.h"

// 监听输入
void ProcessInput(GLFWwindow* window)
{
    // Escape按钮关闭窗口
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}