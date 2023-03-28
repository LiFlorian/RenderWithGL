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
		Part Const Definition
	----------------------------------------------------*/


	GLfloat near_plane = 1.0f, far_plane = 25.0f;

	vector<int> normalTexVertDivisor{ 3, 3, 2 };

	unsigned int WoodTex = TexLoader->LoadTexture((char*)"res/textures/wood.png");

	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);


	/*----------------------------------------------------
		Part Depth Map
	----------------------------------------------------*/



	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	// 创建CubeMap作为点光源的六面深度缓冲纹理
	GLuint depthCubeMap;
	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// CubeMap绑定至depthMapFBO
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// 创建六个方向的投影矩阵
	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);

	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj *
					 glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
					 glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
					 glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj *
					 glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj *
					 glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
					 glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));




	/*----------------------------------------------------
		Part Prepare Scene
	----------------------------------------------------*/
	

	// 立方体
	SimpleRender* CubeRender = new SimpleRender(normalTexVertDivisor, Cube_NormalTexVert, sizeof(Cube_NormalTexVert));
	CubeRender->BindTexture(WoodTex);

	// 点光源
	SimpleRender* PointLightRender = new SimpleRender(normalTexVertDivisor, Cube_NormalTexVert, sizeof(Cube_NormalTexVert));



	/*----------------------------------------------------
		Part Shader
	----------------------------------------------------*/


	
	// 绘制深度CubeMap用
	Shader* DepthMapShader = new Shader("shader/Shadow/Perspective/PerspectiveDepthMap.vs", "shader/Shadow/Perspective/PerspectiveDepthMap.fs", "shader/Shadow/Perspective/PerspectiveDepthMap.gs");
	DepthMapShader->Use();
	DepthMapShader->SetVec3("lightPos", lightPos);
	DepthMapShader->SetFloat("far_plane", far_plane);

	for (GLuint i = 0; i < 6; i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(DepthMapShader->ID, ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
	}
	

	// 绘制带阴影的场景用
	Shader* BlinnPhongShader = new Shader("shader/Shadow/Perspective/Blinn_Phong_Point_Shadow.vs", "shader/Shadow/Perspective/Blinn_Phong_Point_Shadow.fs");
	BlinnPhongShader->Use();
	BlinnPhongShader->SetVec3("LightPos", lightPos);
	BlinnPhongShader->SetFloat("far_plane", far_plane);
	BlinnPhongShader->SetInt("diffuseTex", 0);


	// 绘制点光源用
	Shader* PointLightShader = new Shader("shader/SingleColor.vs", "shader/SingleColor.fs");
	PointLightShader->Use();
	PointLightShader->SetVec3("InColor", glm::vec3(0.5));
	


	/*----------------------------------------------------
		Part Render Loop
	----------------------------------------------------*/
	float deltaTime = 0;
	float lastFrame = static_cast<float>(glfwGetTime());

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
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(CurCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane); // 45度FOV, 视口长宽比, 近平面, 远屏幕
		// View矩阵
		glm::mat4 viewMatrix = CurCamera->LookAt();
		// Model矩阵
		glm::mat4 modelMatrix = glm::mat4(1.0f);


		/*----------------------------------------------------
		Loop 状态重置
		----------------------------------------------------*/


		glEnable(GL_DEPTH_TEST); // 开启深度测试

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 设置背景色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色及深度缓冲



		/*----------------------------------------------------
		Loop 深度图更新
		----------------------------------------------------*/


		// 开启正面剔除, 防止深度偏移导致的漂浮现象
		//glCullFace(GL_FRONT);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);


		DepthMapShader->Use();
		

		// 包围Cube
		//glDisable(GL_CULL_FACE);
		//modelMatrix = glm::mat4(1.0f);
		//modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0));
		//DepthMapShader->SetMat4("model", modelMatrix);
		//CubeRender->DrawShape();
		//glEnable(GL_CULL_FACE);

		// cubes
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(4.0f, -3.5f, 0.0));
		DepthMapShader->SetMat4("model", modelMatrix);
		CubeRender->DrawShape();

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 3.0f, 1.0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
		DepthMapShader->SetMat4("model", modelMatrix);
		CubeRender->DrawShape();

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.0f, -1.0f, 0.0));
		DepthMapShader->SetMat4("model", modelMatrix);
		CubeRender->DrawShape();

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.5f, 1.0f, 1.5));
		DepthMapShader->SetMat4("model", modelMatrix);
		CubeRender->DrawShape();

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.5f, 2.0f, -3.0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5));
		DepthMapShader->SetMat4("model", modelMatrix);
		CubeRender->DrawShape();


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 还原剔除设置, 准备正常
		//glCullFace(GL_BACK);



		/*----------------------------------------------------
		Loop 场景渲染
		----------------------------------------------------*/


		BlinnPhongShader->Use();
		BlinnPhongShader->SetMat4("view", viewMatrix);
		BlinnPhongShader->SetMat4("projection", projectionMatrix);
		BlinnPhongShader->SetVec3("ViewPos", CurCamera->Pos);

		//// 绑定深度CubeMap纹理
		BlinnPhongShader->SetInt("depthCubeMap", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

		// 包围Cube
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0));
		BlinnPhongShader->SetMat4("model", modelMatrix);

		glDisable(GL_CULL_FACE);
		BlinnPhongShader->SetBool("bReverseNormal", 1);
		CubeRender->Draw(false);
		BlinnPhongShader->SetBool("bReverseNormal", 0);
		glEnable(GL_CULL_FACE);

		// cubes
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(4.0f, -3.5f, 0.0));
		BlinnPhongShader->SetMat4("model", modelMatrix);
		CubeRender->Draw(false);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 3.0f, 1.0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
		BlinnPhongShader->SetMat4("model", modelMatrix);
		CubeRender->Draw(false);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.0f, -1.0f, 0.0));
		BlinnPhongShader->SetMat4("model", modelMatrix);
		CubeRender->Draw(false);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.5f, 1.0f, 1.5));
		BlinnPhongShader->SetMat4("model", modelMatrix);
		CubeRender->Draw(false);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.5f, 2.0f, -3.0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5));
		BlinnPhongShader->SetMat4("model", modelMatrix);
		CubeRender->Draw(false);





		// 光源模型
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5));
		PointLightShader->Use();
		PointLightShader->SetMat4("model", modelMatrix);
		PointLightShader->SetMat4("view", viewMatrix);
		PointLightShader->SetMat4("projection", projectionMatrix);
		PointLightRender->Draw(false);



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





