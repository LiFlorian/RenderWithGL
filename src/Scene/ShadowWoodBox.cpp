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

#include "../render/Shader.h"
#include "../render/ModelRender.h"
#include "../obj/Camera.h"
#include "../tool/TextureLoader.h"
#include "../render/FrameBuffer.h"
#include "../render/SimpleRender.h"


// 常数定义
const float SCR_WIDTH = 1920;
const float SCR_HEIGHT = 1080;
const GLuint SHADOW_WIDTH = 1024;
const GLuint SHADOW_HEIGHT = 1024;


// 函数声明
void InitGLFW();
GLFWwindow* CreateWindow();

// 窗口尺寸改变回调, 初始化时同样会执行
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// 输入回调函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(float deltaTime, GLFWwindow* window);

// 全局变量
Camera* CurCamera;

TextureLoader* TexLoader;

int main()
{
	/*----------------------------------------------------
		Part Init
	----------------------------------------------------*/



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

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback); // 鼠标滑动回调
	glfwSetScrollCallback(window, scroll_callback); // 鼠标滚轮回调

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	TexLoader = new TextureLoader();




	/*----------------------------------------------------
		Part Prepare Scene
	----------------------------------------------------*/



	unsigned int WoodTex = TexLoader->LoadTexture((char*)"res/textures/wood.png");

	vector<int> normalTexVertDivisor{ 3, 3, 2 };

	// 地板
	SimpleRender* FloorRender = new SimpleRender(normalTexVertDivisor, ShadowPlan, sizeof(ShadowPlan));
	FloorRender->BindTexture(WoodTex);

	// 立方体
	SimpleRender* CubeRender = new SimpleRender(normalTexVertDivisor, UnitCube, sizeof(UnitCube));
	CubeRender->BindTexture(WoodTex);

	float deltaTime = 0;
	float lastFrame = static_cast<float>(glfwGetTime());



	/*----------------------------------------------------
		Part Depth Map
	----------------------------------------------------*/



	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);




	/*----------------------------------------------------
		Part RenderQuad
	----------------------------------------------------*/


	// 铺屏四边形
	vector<int> ScreenQuadAttri{2, 2};
	SimpleRender* ScreenQuadRender = new SimpleRender(ScreenQuadAttri, quadVertices, sizeof(quadVertices));

	// 离屏shader绑定对应Texture
	ScreenQuadRender->BindTexture(depthMap);



	/*----------------------------------------------------
		Part Shader
	----------------------------------------------------*/


	Shader* DepthMapShader = new Shader("shader/Shadow/DepthMap.vs", "shader/Shadow/DepthMap.fs");

	GLfloat near_plane = 1.0f, far_plane = 7.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	DepthMapShader->Use();
	DepthMapShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);


	Shader* SingleTexShader = new Shader("shader/SingleTex.vs", "shader/SingleTex.fs");
	SingleTexShader->Use();
	SingleTexShader->SetInt("InTex", 0);


	Shader* DepthShowShader = new Shader("shader/Shadow/DepthShow.vs", "shader/Shadow/DepthShow.fs");
	DepthShowShader->Use();
	DepthShowShader->SetInt("RT", 0);


	/*----------------------------------------------------
		Part Render Loop
	----------------------------------------------------*/
	
	while (!glfwWindowShouldClose(window))
	{
		/*----------------------------------------------------
		Loop 帧间隔deltaTime刷新
		----------------------------------------------------*/


		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;



		/*----------------------------------------------------
		Loop 输入处理及相机位置更新
		计算通用View及Projection矩阵
		----------------------------------------------------*/


		processInput(deltaTime, window);

		// Projection矩阵
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(CurCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); // 45度FOV, 视口长宽比, 近平面0.1, 远屏幕100

		// View矩阵
		glm::mat4 viewMatrix = CurCamera->LookAt();


		/*----------------------------------------------------
		Loop 状态重置
		----------------------------------------------------*/


		glEnable(GL_DEPTH_TEST); // 开启深度测试

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 设置背景色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色及深度缓冲



		/*----------------------------------------------------
		Loop 深度图更新
		----------------------------------------------------*/


		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);


		DepthMapShader->Use();


		glm::mat4 model = glm::mat4(1.0f);
		DepthMapShader->SetMat4("model", model);
		FloorRender->DrawShape();

		// cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		DepthMapShader->SetMat4("model", model);
		CubeRender->DrawShape();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		DepthMapShader->SetMat4("model", model);
		CubeRender->DrawShape();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(0.25));
		DepthMapShader->SetMat4("model", model);
		CubeRender->DrawShape();


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		/*----------------------------------------------------
		Loop 深度图绘制
		----------------------------------------------------*/



		DepthShowShader->Use();
		ScreenQuadRender->Draw(false);



		/*----------------------------------------------------
		Loop 场景渲染
		----------------------------------------------------*/


		//SingleTexShader->Use();
		//SingleTexShader->SetMat4("view", viewMatrix);
		//SingleTexShader->SetMat4("projection", projectionMatrix);
		//

		//model = glm::mat4(1.0f);
		//SingleTexShader->SetMat4("model", model);

		//FloorRender->Draw(false);


		//// cubes
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		//model = glm::scale(model, glm::vec3(0.5f));
		//SingleTexShader->SetMat4("model", model);
		//CubeRender->Draw(false);

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		//model = glm::scale(model, glm::vec3(0.5f));
		//SingleTexShader->SetMat4("model", model);
		//CubeRender->Draw(false);

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
		//model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		//model = glm::scale(model, glm::vec3(0.25));
		//SingleTexShader->SetMat4("model", model);
		//CubeRender->Draw(false);



		/*----------------------------------------------------
		Loop End
		----------------------------------------------------*/

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
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
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





