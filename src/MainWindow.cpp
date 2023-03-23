#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Data.h"

#include "MainWindow.h"
#include "render/RenderContext.h"
#include "render/Shader.h"
#include "render/RenderUtil.h"
#include "render/ModelRender.h"
#include "obj/Camera.h"
#include "obj/Light.h"



// 窗口尺寸定义
const float SCR_WIDTH = 1920;
const float SCR_HEIGHT = 1080;

// 输入回调函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(float deltaTime, GLFWwindow* window);

// 全局相机
Camera* CurCamera;

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

	// 创建场景相机
	CurCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glfwSetCursorPosCallback(window, mouse_callback); // 鼠标滑动回调
	glfwSetScrollCallback(window, scroll_callback); // 鼠标滚轮回调

	// 定义常数
	glm::vec3 LightPos = glm::vec3(1.2f, 1.0f, 2.0f);

	// 光源绘制所需的Shader及Obj
	Shader* LightShader = new Shader("shader/Light/Light_Base.vs", "shader/Light/Light_Base.fs");
	Light* OneLight = new Light(LightShader, Cube_NormalTexVert, sizeof(Cube_NormalTexVert), LightPos);

	// 创建模型
	ModelRender* model = new ModelRender((char*)"res/model/nanosuit/nanosuit.obj");

	// 模型绘制所需Shader以及Context
    Shader* ContextShader = new Shader("shader/Light/Phong_TextureMap.vs", "shader/Light/Phong_TextureMap.fs");
	ContextShader->Use();
	// 平行光参数
	ContextShader->SetVec3("paraLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	ContextShader->SetVec3("paraLight.ambient", glm::vec3(0.02f, 0.02f, 0.02f));
	ContextShader->SetVec3("paraLight.diffuse", glm::vec3(0.05f, 0.05f, 0.05f));
	ContextShader->SetVec3("paraLight.specular", glm::vec3(0.1f, 0.1f, 0.1f));
	// 点光源静态参数
	ContextShader->SetVec3("pointLight.position", LightPos);
	ContextShader->SetVec3("pointLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	ContextShader->SetVec3("pointLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	ContextShader->SetVec3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	ContextShader->SetFloat("pointLight.constant", 1.0f);
	ContextShader->SetFloat("pointLight.linear", 0.09f);
	ContextShader->SetFloat("pointLight.quadratic", 0.032f);
	// 投射光静态参数
	ContextShader->SetVec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	ContextShader->SetVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	ContextShader->SetFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
	ContextShader->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
	
	// 光泽度
	ContextShader->SetFloat("material.shininess", 32.0f);

	glEnable(GL_DEPTH_TEST); // 开启深度测试

	float deltaTime = 0;
	float lastFrame = static_cast<float>(glfwGetTime());

    // 绘制循环
    while (!glfwWindowShouldClose(window))
    {
		// 清除深度缓冲
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 渲染背景
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(deltaTime, window);

		// Projection矩阵
		glm::mat4 projectionMatrix = glm::mat4(1.0f);
		projectionMatrix = glm::perspective(glm::radians(CurCamera->Zoom), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f); // 45度FOV, 视口长宽比, 近平面0.1, 远屏幕100

		// View矩阵
		glm::mat4 viewMatrix = CurCamera->LookAt();

		// 绘制光源
		LightShader->Use();
		OneLight->Draw(viewMatrix, projectionMatrix);

		// 设置模型shader的动态参数
		ContextShader->Use();
		ContextShader->SetVec3("ViewPos", CurCamera->Pos);
		ContextShader->SetVec3("spotLight.position", CurCamera->Pos);
		ContextShader->SetVec3("spotLight.direction", CurCamera->Front);

		// Model矩阵
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, cubePositions[0]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));

		// 绘制模型
		model->Draw(ContextShader, modelMatrix, viewMatrix, projectionMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
		CurCamera->ProcessKeyboard(EMoveDir::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		CurCamera->ProcessKeyboard(EMoveDir::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		CurCamera->ProcessKeyboard(EMoveDir::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		CurCamera->ProcessKeyboard(EMoveDir::RIGHT, deltaTime);
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

	CurCamera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	CurCamera->ProcessMouseScroll(static_cast<float>(yoffset));
}





