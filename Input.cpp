#include <GLFW/glfw3.h>

#include "Input.h"

// ��������
void ProcessInput(GLFWwindow* window)
{
    // Escape��ť�رմ���
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}