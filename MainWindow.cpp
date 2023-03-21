#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "MainWindow.h"
#include "Input.h"
#include "RenderContext.h"
#include "Shader.h"
#include "Data.h"
#include "RenderUtil.h"

// 窗口尺寸定义
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

int main()
{
    InitGLFW();

    GLFWwindow* window = CreateWindow();

    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader* CurShader = new Shader("shader/Entry/Triangle_2D.vs", "shader/Entry/Triangle_2D.fs");

    // RenderContext* Context = new RenderContext(EVertexType::EPos, Triangle_2D, sizeof(Triangle_2D));
	// 
    // RenderContext* Context = new RenderContext(EVertexType::EPos, Rectangle_2D, sizeof(Rectangle_2D), Rectangle_2D_Indices, sizeof(Rectangle_2D_Indices));
	// 
	// RenderContext* Context = new RenderContext(EVertexType::EPos_Color, Triangle_ColorVert, sizeof(Triangle_ColorVert));

	//RenderContext* Context = new RenderContext(EVertexType::EPos_Color_Tex, Rectangle_ColorTexVert, sizeof(Rectangle_ColorTexVert), Rectangle_2D_Indices, sizeof(Rectangle_2D_Indices));
	//unsigned int Texture1 = LoadTexture("res/textures/awesomeface.png", GL_RGBA);

	RenderContext* Context = new RenderContext(EVertexType::EPos_Color_Tex, Rectangle_ColorTexVert, sizeof(Rectangle_ColorTexVert), Rectangle_2D_Indices, sizeof(Rectangle_2D_Indices));
	unsigned int Texture1 = LoadTexture("res/textures/container.jpg", GL_RGB);
	unsigned int Texture2 = LoadTexture("res/textures/awesomeface.png", GL_RGBA, true);
	CurShader->Use();
	CurShader->SetInt("texture1", 0);
	CurShader->SetInt("texture2", 1);
	CurShader->SetFloat("texMix", 1);

    // 绘制循环
    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        // 渲染背景
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		Context->ActiveTexture(Texture1);
		Context->ActiveTexture(Texture2);

		// 使用编译好的Shader程序
		CurShader->Use();
        
		Context->DrawElements(false, EDrawType::ERectangle);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete Context;

    // 退出程序
    glfwTerminate();
    return 0;
}


// 初始化GLFW
void InitGLFW() 
{
    glfwInit();

    // 设置OpenGL支持版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // 使用OpenGL核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


// 创建窗口对象
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

    // 设置窗口大小变化时的回调
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

// 窗口改变回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

