#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "RenderLoop.h"

void RenderLoop(GLFWwindow* window)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}