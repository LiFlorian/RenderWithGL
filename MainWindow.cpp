#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MainWindow.h"
#include "RenderContext.h"
#include "Shader.h"
#include "Data.h"
#include "RenderUtil.h"
#include "Camera.h"


// 窗口尺寸定义
const float SCR_WIDTH = 1920;
const float SCR_HEIGHT = 1080;

// 输入回调函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(float deltaTime, GLFWwindow* window);

// 全局相机
Camera* curCamera;

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

	curCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Viewport聚焦时隐藏光标
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback); // 鼠标滑动回调
	glfwSetScrollCallback(window, scroll_callback); // 鼠标滚轮回调

    Shader* CurShader = new Shader("shader/Entry/Vertex_Base_3D.vs", "shader/Entry/Frag_Base_3D.fs");

	RenderContext* Context = new RenderContext(EVertexType::EPos_Tex, Cube_TexVert, sizeof(Cube_TexVert));
	unsigned int Texture1 = LoadTexture("res/textures/container.jpg", GL_RGB);
	unsigned int Texture2 = LoadTexture("res/textures/awesomeface.png", GL_RGBA, true);
	CurShader->Use();
	CurShader->SetInt("texture1", 0);
	CurShader->SetInt("texture2", 1);
	CurShader->SetFloat("texMix", 0.2);

	glEnable(GL_DEPTH_TEST); // 开启深度测试

	float deltaTime = 0;
	float lastFrame = static_cast<float>(glfwGetTime());

    // 绘制循环
    while (!glfwWindowShouldClose(window))
    {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(deltaTime, window);

        // 渲染背景
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		Context->ActiveTexture(Texture1);
		Context->ActiveTexture(Texture2);

		// 使用编译好的Shader程序
		CurShader->Use();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除深度缓冲

		// Model矩阵
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		int modelLoc = glGetUniformLocation(CurShader->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// View矩阵
		int viewLoc = glGetUniformLocation(CurShader->ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(curCamera->LookAt()));

		// Projection矩阵
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(curCamera->Zoom), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f); // 45度FOV, 视口长宽比, 近平面0.1, 远屏幕100

		int projectionLoc = glGetUniformLocation(CurShader->ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
		Context->DrawElements(false, EDrawType::ECube);

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


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(float deltaTime, GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		curCamera->ProcessKeyboard(EMoveDir::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		curCamera->ProcessKeyboard(EMoveDir::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		curCamera->ProcessKeyboard(EMoveDir::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		curCamera->ProcessKeyboard(EMoveDir::RIGHT, deltaTime);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
bool firstMouse = true;
double lastX, lastY;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	curCamera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	curCamera->ProcessMouseScroll(static_cast<float>(yoffset));
}




//// 备份用, 使用时复制到main函数中, 后续抽象重构
//void Base2DGrapheRender(GLFWwindow* window)
//{
//	Shader* CurShader = new Shader("shader/Entry/Triangle_2D.vs", "shader/Entry/Triangle_2D.fs");
//
//	// RenderContext* Context = new RenderContext(EVertexType::EPos, Triangle_2D, sizeof(Triangle_2D));
//	// 
//	// RenderContext* Context = new RenderContext(EVertexType::EPos, Rectangle_2D, sizeof(Rectangle_2D), Rectangle_2D_Indices, sizeof(Rectangle_2D_Indices));
//	// 
//	// RenderContext* Context = new RenderContext(EVertexType::EPos_Color, Triangle_ColorVert, sizeof(Triangle_ColorVert));
//
//	//RenderContext* Context = new RenderContext(EVertexType::EPos_Color_Tex, Rectangle_ColorTexVert, sizeof(Rectangle_ColorTexVert), Rectangle_2D_Indices, sizeof(Rectangle_2D_Indices));
//	//unsigned int Texture1 = LoadTexture("res/textures/awesomeface.png", GL_RGBA);
//
//	RenderContext* Context = new RenderContext(EVertexType::EPos_Color_Tex, Rectangle_ColorTexVert, sizeof(Rectangle_ColorTexVert), Rectangle_2D_Indices, sizeof(Rectangle_2D_Indices));
//	unsigned int Texture1 = LoadTexture("res/textures/container.jpg", GL_RGB);
//	unsigned int Texture2 = LoadTexture("res/textures/awesomeface.png", GL_RGBA, true);
//	CurShader->Use();
//	CurShader->SetInt("texture1", 0);
//	CurShader->SetInt("texture2", 1);
//	CurShader->SetFloat("texMix", 0.2);
//
//	// 绘制循环
//	while (!glfwWindowShouldClose(window))
//	{
//		// 渲染背景
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		Context->ActiveTexture(Texture1);
//		Context->ActiveTexture(Texture2);
//
//		// 使用编译好的Shader程序
//		CurShader->Use();
//
//		// 计算随时间旋转的矩阵
//		glm::mat4 trans = glm::mat4(1.0f);
//		//trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
//		//trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
//		//float scale = sin(glfwGetTime()) * 0.5 + 0.5;
//		//trans = glm::scale(trans, glm::vec3(scale));
//
//		unsigned int transformLoc = glGetUniformLocation(CurShader->ID, "transform");
//		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
//
//		Context->DrawElements(false, EDrawType::ERectangle);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//}
