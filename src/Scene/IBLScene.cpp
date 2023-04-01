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
#include "../render/GBuffer.h"
#include "../render/SSAOKernel.h"
#include "../render/SphereRender.h"
#include "../render/CubeMap.h"


// 常数定义
const float SCR_WIDTH = 1920;
const float SCR_HEIGHT = 1080;
const float NEAR_PLAN = 0.1f;
const float FAR_PLAN = 100.0f;


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
		Part Debug Quad
	----------------------------------------------------*/


	// 屏幕纹理渲染缓冲数据, 用于直接展示中间过程生成的各种纹理, Debug用
	vector<int> DebugQuadAttri{ 2, 2 };
	SimpleRender* DebugQuadRender = new SimpleRender(DebugQuadAttri, quadVertices, sizeof(quadVertices));

	Shader* RTShader = new Shader("shader/PostProcess/Primitive.vs", "shader/PostProcess/Primitive.fs");
	RTShader->Use();
	RTShader->SetInt("RT", 0);


	/*----------------------------------------------------
		Part 光源
	----------------------------------------------------*/

	vector<glm::vec3> lightPositions{
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};

	vector<glm::vec3> lightColors{
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};


	Shader* SingleColorShader = new Shader("shader/SingleColor.vs", "shader/SingleColor.fs");
	SingleColorShader->Use();

	/*----------------------------------------------------
		Part Skybox
	----------------------------------------------------*/


	Shader* SkyboxShader = new Shader("shader/SkyBox.vs", "shader/SkyBox.fs");


	/*----------------------------------------------------
		Part ERP Sample
	----------------------------------------------------*/


	GLuint ERPTex = TexLoader->LoadHDRTexture((char*)"res/hdr/newport_loft.hdr");
	GLuint CaptureWidth = 512, CaptureHeight = 512;

	vector<int> PosNormalTexAttri{ 3, 3, 2 };
	SimpleRender* UnitCubeRender = new SimpleRender(PosNormalTexAttri, UnitCube, sizeof(UnitCube));

	CubeMap* ERPCubeMap = new CubeMap(CaptureWidth, CaptureHeight);

	//Shader* ERPDrawShader = new Shader("shader/PBR/IBL/ERPDraw.vs", "shader/PBR/IBL/ERPDraw.fs");
	Shader* ERPCaptureShader = new Shader("shader/PBR/IBL/ERPCapture.vs", "shader/PBR/IBL/ERPCapture.fs");

	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CaptureWidth, CaptureHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// ERP采样
	glViewport(0, 0, CaptureWidth, CaptureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	ERPCaptureShader->Use();
	ERPCaptureShader->SetInt("equirectangularMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ERPTex);

	// 在立方体中心分别以六个viewMatrix各渲染一次, 这样就可以摘出立方体的六个面来
	ERPCaptureShader->SetMat4("projection", ERPCubeMap->captureProjection);
	for (unsigned int i = 0; i < 6; ++i)
	{
		ERPCaptureShader->SetMat4("view", ERPCubeMap->captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ERPCubeMap->ID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		UnitCubeRender->Draw();
	}


	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/*----------------------------------------------------
		Part ERP Convolution
	----------------------------------------------------*/

	GLuint ConvoWidth = 32, ConvoHeight = 32;
	CubeMap* ERPConvoCubeMap = new CubeMap(ConvoWidth, ConvoHeight);

	Shader* ERPConvoShader = new Shader("shader/PBR/IBL/ERPConv.vs", "shader/PBR/IBL/ERPConv.fs");

	unsigned int convoFBO, convoRBO;
	glGenFramebuffers(1, &convoFBO);
	glGenRenderbuffers(1, &convoRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, convoFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, convoRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, ConvoWidth, ConvoHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, convoRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	

	// 卷积计算
	glViewport(0, 0, ConvoWidth, ConvoHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, convoFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	ERPConvoShader->Use();
	ERPConvoShader->SetInt("ERPCubeTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ERPCubeMap->ID);

	ERPConvoShader->SetMat4("projection", ERPConvoCubeMap->captureProjection);
	for (unsigned int i = 0; i < 6; ++i)
	{
		ERPConvoShader->SetMat4("view", ERPConvoCubeMap->captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ERPConvoCubeMap->ID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		UnitCubeRender->Draw();
	}


	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	/*----------------------------------------------------
		Part Obj
	----------------------------------------------------*/

	SphereRender* Sphere = new SphereRender();

	// 模型排列数据
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	Shader* PBRShader = new Shader("shader/PBR/PBR.vs", "shader/PBR/PBR.fs");
	PBRShader->Use();
	PBRShader->SetVec3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
	PBRShader->SetFloat("ao", 1.0f);


	/*----------------------------------------------------
		Part Render Loop
	----------------------------------------------------*/

	float deltaTime = 0;
	float lastFrame = static_cast<float>(glfwGetTime());

	// 绘制循环
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
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(CurCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLAN, FAR_PLAN); // 45度FOV, 视口长宽比, 近平面0.1, 远屏幕100
		// View矩阵
		glm::mat4 viewMatrix = CurCamera->LookAt();
		// Model矩阵
		glm::mat4 modelMatrix = glm::mat4(1.0f);


		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);


		/*------------------------------------------------------------------------------------------------------------------
			Loop Skybox
		--------------------------------------------------------------------------------------------------------------------*/

		glDepthFunc(GL_LEQUAL);


		SkyboxShader->Use();
		SkyboxShader->SetMat4("view", viewMatrix);
		SkyboxShader->SetMat4("projection", projectionMatrix);
		SkyboxShader->SetInt("skyboxTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ERPConvoCubeMap->ID);

		UnitCubeRender->Draw();


		glDepthFunc(GL_LESS);

		/*------------------------------------------------------------------------------------------------------------------
			Loop Light Obj
		--------------------------------------------------------------------------------------------------------------------*/

		for (unsigned int i = 0; i < lightPositions.size(); ++i)
		{
			lightPositions[i] = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 3.0) * 3.0, 0.0, 0.0);
		}

		SingleColorShader->Use();

		for (int i = 0; i < lightPositions.size(); i++)
		{
			SingleColorShader->SetVec3("InColor", lightColors[i]);

			modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, lightPositions[i]);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
			Sphere->Draw(SingleColorShader, modelMatrix, viewMatrix, projectionMatrix);
		}


		/*--------------------------------------------------------------------------------------------------------------
			Loop Render Obj
		--------------------------------------------------------------------------------------------------------------*/


		PBRShader->Use();
		PBRShader->SetVec3("ViewPos", CurCamera->Pos);
		for (unsigned int i = 0; i < lightPositions.size(); ++i)
		{
			PBRShader->SetVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
			PBRShader->SetVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
		}

		// 绘制球体
		for (int row = 0; row < nrRows; ++row)
		{
			// 从上到下金属度递增
			PBRShader->SetFloat("metallic", (float)row / (float)nrRows);

			for (int col = 0; col < nrColumns; ++col)
			{
				// 从左到右粗糙度递增
				PBRShader->SetFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

				modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(
					(col - (nrColumns / 2)) * spacing,
					(row - (nrRows / 2)) * spacing,
					0.0f
				));

				// 球体法线是在local空间生成的, 需要将其转换至World空间
				PBRShader->SetMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelMatrix))));

				Sphere->Draw(PBRShader, modelMatrix, viewMatrix, projectionMatrix);
			}
		}



		/*--------------------------------------------------------------------------------------------------------------
			Loop Debug Pass
		--------------------------------------------------------------------------------------------------------------*/


		//// 场景中渲染立方体直接查看ERP贴图
		//ERPDrawShader->Use();
		//ERPDrawShader->SetInt("equirectangularMap", 0);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, ERPTex);

		//modelMatrix = glm::mat4(1.0f);
		//modelMatrix = glm::scale(modelMatrix, glm::vec3(5, 5, 5));
		//ERPDrawShader->SetMat4("model", modelMatrix);
		//ERPDrawShader->SetMat4("view", viewMatrix);
		//ERPDrawShader->SetMat4("projection", projectionMatrix);
		//CubeRender->Draw(false);



		// Debug代码, 直接显示各种中间结果的纹理附件
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//RTShader->Use();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, SSAOBlurBuffer->TexAttached);

		//DebugQuadRender->Draw(false);


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





