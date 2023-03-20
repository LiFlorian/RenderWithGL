#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "MainWindow.h"
#include "Input.h"
#include "RenderLoop.h"

// ���ڳߴ綨��
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

int main()
{
    InitGLFW();

    GLFWwindow* window = CreateWindow();

    // ��ʼ��glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ����ѭ��
    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        RenderLoop(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �˳�����
    glfwTerminate();
    return 0;
}


// ��ʼ��GLFW
void InitGLFW() 
{
    glfwInit();

    // ����OpenGL֧�ְ汾
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // ʹ��OpenGL����ģʽ
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


// �������ڶ���
GLFWwindow* CreateWindow()
{
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SoftRendererGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // ���ô��ڴ�С�仯ʱ�Ļص�
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

// ���ڸı�ص�
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

