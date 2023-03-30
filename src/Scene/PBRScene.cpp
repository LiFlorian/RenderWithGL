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


// 常数定义
const float SCR_WIDTH = 1920;
const float SCR_HEIGHT = 1080;


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
		Part G-Buffer
	----------------------------------------------------*/


	GBuffer* GBufferInst = new GBuffer(SCR_WIDTH, SCR_HEIGHT);

	// 绘制GBuffer所需Shader
	Shader* GBufferGenShader = new Shader("shader/SSAO/SSAOGBufferGen.vs", "shader/SSAO/SSAOGBufferGen.fs");
	GBufferGenShader->Use();
	GBufferGenShader->SetVec3("InColor", glm::vec3(0.9f));
	GBufferGenShader->SetBool("bInverseNormal", 0);


	// 铺屏四边形, 绘制屏幕纹理用
	vector<int> ScreenQuadAttri{ 3, 2 };
	SimpleRender* ScreenQuadRender = new SimpleRender(ScreenQuadAttri, GBufferQuadVertices, sizeof(GBufferQuadVertices), GBufferQuadIndices, sizeof(GBufferQuadIndices));



	/*----------------------------------------------------
		Part SSAO
	----------------------------------------------------*/


	GLuint KernelSize = 64;
	GLuint KernelNoiseSize = 4;
	SSAOKernel* SSAOKernelInst = new SSAOKernel(KernelSize, KernelNoiseSize); // SSAO样本生成


	// 基于GBuffer采样并生成SSAO纹理
	Shader* SSAOGenShader = new Shader("shader/SSAO/SSAOGen.vs", "shader/SSAO/SSAOGen.fs");
	SSAOGenShader->Use();
	SSAOGenShader->SetInt("kernelSize", KernelSize);
	SSAOGenShader->SetInt("noiseSize", KernelNoiseSize);
	SSAOGenShader->SetFloat("Screen_Width", SCR_WIDTH);
	SSAOGenShader->SetFloat("Screen_Height", SCR_HEIGHT);

	FrameBuffer* SSAOBuffer = new FrameBuffer(false, SCR_WIDTH, SCR_HEIGHT, GL_RED);


	// 对SSAO纹理进行模糊消除噪声
	Shader* SSAOBlurShader = new Shader("shader/SSAO/SSAOBlur.vs", "shader/SSAO/SSAOBlur.fs");
	FrameBuffer* SSAOBlurBuffer = new FrameBuffer(false, SCR_WIDTH, SCR_HEIGHT, GL_RED);


	// 渲染Obj用的Shader, 整合SSAO的Blinn-Phong计算
	Shader* SSAOBlinnShader = new Shader("shader/SSAO/SSAO_Blinn_Phong.vs", "shader/SSAO/SSAO_Blinn_Phong.fs");



	/*----------------------------------------------------
		Part 光源
	----------------------------------------------------*/

	// 光源Shader及静态参数
	Shader* SingleColorShader = new Shader("shader/SingleColor.vs", "shader/SingleColor.fs");
	SingleColorShader->Use();
	SingleColorShader->SetVec3("InColor", glm::vec3(1.0f));

	// 光源Obj
	MeshRender* LightRender = new MeshRender(Cube_NormalTexVert, sizeof(Cube_NormalTexVert) / sizeof(float));


	/*----------------------------------------------------
		Part Obj
	----------------------------------------------------*/


	// 模型Obj
	ModelRender* NanosuitRender = new ModelRender((char*)"res/model/nanosuit/nanosuit.obj");

	// 地板Obj
	vector<int> FloorCubeAttri{ 3, 3, 2 };
	SimpleRender* FloorRender = new SimpleRender(FloorCubeAttri, Cube_NormalTexVert, sizeof(Cube_NormalTexVert));


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
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(CurCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f); // 45度FOV, 视口长宽比, 近平面0.1, 远屏幕100
		// View矩阵
		glm::mat4 viewMatrix = CurCamera->LookAt();
		// Model矩阵
		glm::mat4 modelMatrix = glm::mat4(1.0f);


		/*------------------------------------------------------------------------------------------------------------------
			Loop GBuffer Pass
		--------------------------------------------------------------------------------------------------------------------*/


		glBindFramebuffer(GL_FRAMEBUFFER, GBufferInst->ID);


		glEnable(GL_DEPTH_TEST); // 开启深度测试

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 设置背景色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色及深度缓冲

		GBufferGenShader->Use();


		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, -1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(20.0f, 1.0f, 20.0f));
		FloorRender->Draw(GBufferGenShader, modelMatrix, viewMatrix, projectionMatrix);


		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 5.0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
		NanosuitRender->Draw(GBufferGenShader, modelMatrix, viewMatrix, projectionMatrix);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
		glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

		/*--------------------------------------------------------------------------------------------------------------
			Loop SSAO Buffer Gen
		--------------------------------------------------------------------------------------------------------------*/


		glBindFramebuffer(GL_FRAMEBUFFER, SSAOBuffer->FBO);
		glClear(GL_COLOR_BUFFER_BIT);


		SSAOGenShader->Use();
		SSAOGenShader->SetInt("gPosition", 0);
		SSAOGenShader->SetInt("gNormal", 1);
		SSAOGenShader->SetInt("texNoise", 2);

		// 设置GBuffer纹理及Kernel随机旋转轴纹理
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GBufferInst->gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GBufferInst->gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, SSAOKernelInst->NoiseTex);

		// 设置Kernel采样点
		for (GLuint i = 0; i < 64; ++i)
		{
			glUniform3fv(glGetUniformLocation(SSAOGenShader->ID, ("samples[" + std::to_string(i) + "]").c_str()), 1, &SSAOKernelInst->KernelList[i][0]);
		}

		glUniformMatrix4fv(glGetUniformLocation(SSAOGenShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		ScreenQuadRender->Draw(false);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);



		/*--------------------------------------------------------------------------------------------------------------
			Loop SSAO Buffer Blur
		--------------------------------------------------------------------------------------------------------------*/


		glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurBuffer->FBO);
		glClear(GL_COLOR_BUFFER_BIT);


		SSAOBlurShader->Use();
		SSAOBlurShader->SetInt("SSAOTex", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, SSAOBuffer->TexAttached);

		ScreenQuadRender->Draw(false);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		/*--------------------------------------------------------------------------------------------------------------
			Loop Light Pass
		--------------------------------------------------------------------------------------------------------------*/


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 重建深度缓冲
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, GBufferInst->ID);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
		//glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);


		SSAOBlinnShader->Use(); // 激活屏幕绘制Shader

		SSAOBlinnShader->SetInt("gPositionDepth", 0);
		SSAOBlinnShader->SetInt("gNormal", 1);
		SSAOBlinnShader->SetInt("gAlbedo", 2);
		SSAOBlinnShader->SetInt("ssao", 3);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GBufferInst->gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GBufferInst->gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, GBufferInst->gColorSpec);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, SSAOBlurBuffer->TexAttached);


		SSAOBlinnShader->SetVec3("light.Position", lightPos);
		SSAOBlinnShader->SetVec3("light.Color", lightColor);
		SSAOBlinnShader->SetFloat("light.Linear", 0.09f);
		SSAOBlinnShader->SetFloat("light.Quadratic", 0.032f);


		ScreenQuadRender->Draw(false); // 绘制铺屏四边形




		/*--------------------------------------------------------------------------------------------------------------
			Loop Debug Pass
		--------------------------------------------------------------------------------------------------------------*/



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





