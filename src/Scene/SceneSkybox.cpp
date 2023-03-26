//#include <GLFW/glfw3.h>
//#include <glad/glad.h>
//#include <iostream>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//#include "Data.h"
//
//#include "../render/Shader.h"
//#include "../render/ModelRender.h"
//#include "../obj/Camera.h"
//#include "../tool/TextureLoader.h"
//#include "../render/FrameBuffer.h"
//#include "../render/SimpleRender.h"
//
//
//// 常数定义
//const float SCR_WIDTH = 1920;
//const float SCR_HEIGHT = 1080;
//
//
//// 函数声明
//void InitGLFW();
//GLFWwindow* CreateWindow();
//
//// 窗口尺寸改变回调, 初始化时同样会执行
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//// 输入回调函数声明
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(float deltaTime, GLFWwindow* window);
//
//// 全局变量
//Camera* CurCamera;
//
//TextureLoader* TexLoader;
//
//int main()
//{
//    InitGLFW();
//
//    GLFWwindow* window = CreateWindow();
//
//    // 初始化glad
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//	// 创建场景相机
//	CurCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//
//	glfwMakeContextCurrent(window);
//	glfwSetCursorPosCallback(window, mouse_callback); // 鼠标滑动回调
//	glfwSetScrollCallback(window, scroll_callback); // 鼠标滚轮回调
//
//	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	TexLoader = new TextureLoader();
//
//
//
//	/*----------------------------------------------------
//		Part 天空盒
//	----------------------------------------------------*/
//
//
//	std::vector<std::string> faceList =
//	{
//		"res/cubemap/right.jpg",
//		"res/cubemap/left.jpg",
//		"res/cubemap/top.jpg",
//		"res/cubemap/bottom.jpg",
//		"res/cubemap/front.jpg",
//		"res/cubemap/back.jpg",
//	};
//
//	Shader* SkyboxShader = new Shader("shader/SkyBox.vs", "shader/SkyBox.fs");
//	SkyboxShader->Use();
//	SkyboxShader->SetInt("skybox", 0);
//
//	vector<int> SkyboxAttriDivisor{3};
//	SimpleRender* SkyboxRender = new SimpleRender(SkyboxAttriDivisor, skyboxVertices, sizeof(skyboxVertices));
//	unsigned int SkyboxTex = TexLoader->LoadCubeMap(faceList);
//	SkyboxRender->BindCubeMap(SkyboxTex);
//
//
//	/*----------------------------------------------------
//		Part 场景
//	----------------------------------------------------*/
//
//
//	Shader* SingleTexShader = new Shader("shader/SingleTex.vs", "shader/SingleTex.fs");
//
//	// 平面
//	MeshRender* Plan = new MeshRender(Plan_TexNormalVert, sizeof(Plan_TexNormalVert) / sizeof(float));
//	unsigned int PlanTex = TexLoader->LoadTexture((char*)"res/textures/metal.png");
//	Plan->AddCustomTexture(PlanTex, "InTex");
//
//
//	/*----------------------------------------------------
//		Part 光源
//	----------------------------------------------------*/
//
//
//	// 定义常数
//	glm::vec3 LightPos = glm::vec3(1.2f, 1.0f, 2.0f);
//
//	// 光源Shader及静态参数
//	Shader* SingleColorShader = new Shader("shader/SingleColor.vs", "shader/SingleColor.fs");
//	SingleColorShader->Use();
//	SingleColorShader->SetVec3("InColor", glm::vec3(1.0f));
//
//	// 光源Obj
//	MeshRender* LightObj = new MeshRender(Cube_NormalTexVert, sizeof(Cube_NormalTexVert) / sizeof(float));
//
//
//
//	/*----------------------------------------------------
//		Part Obj
//	----------------------------------------------------*/
//
//
//	// 模型Shader及静态参数
//	Shader* ModelPhongShader = new Shader("shader/Blinn_Phong_Model.vs", "shader/Blinn_Phong_Model.fs");
//	//Shader* ModelNormalShader = new Shader("shader/Geometry/NormalShow.vs", "shader/Geometry/NormalShow.fs", "shader/Geometry/NormalShow.gs"); // 绘制法线shader
//	//Shader* ModelPhongShader = new Shader("shader/Geometry/NormalExplode.vs", "shader/Geometry/NormalExplode.fs", "shader/Geometry/NormalExplode.gs"); // 法线爆破Shader
//
//	ModelPhongShader->Use();
//	ModelPhongShader->SetFloat("material.shininess", 32.0f);
//	ModelPhongShader->SetParaLightParams();
//	ModelPhongShader->SetPointLightParams(LightPos);
//	ModelPhongShader->SetSpotLightParams();
//
//	// 模型Obj
//	ModelRender* Obj = new ModelRender((char*)"res/model/nanosuit/nanosuit.obj");
//
//	//立方体Obj
//	MeshRender* Cube = new MeshRender(Cube_NormalTexVert, sizeof(Cube_NormalTexVert) / sizeof(float));
//	unsigned int CubeTex = TexLoader->LoadTexture((char*)"res/textures/marble.jpg");
//	Cube->AddCustomTexture(CubeTex, "InTex");
//
//
//
//	/*----------------------------------------------------
//		Part AlphaBlending
//	----------------------------------------------------*/
//
//	vector<glm::vec3> Windows_Pos
//	{
//		glm::vec3(-1.5f, 0.0f, -0.48f),
//		glm::vec3(1.5f, 0.0f, 0.51f),
//		glm::vec3(0.0f, 0.0f, 0.7f),
//		glm::vec3(-0.3f, 0.0f, -2.3f),
//		glm::vec3(0.5f, 0.0f, -0.6f)
//	};
//
//	MeshRender* Window = new MeshRender(Window_TexNormalVert, sizeof(Window_TexNormalVert) / sizeof(float));
//	unsigned int WindowTex = TexLoader->LoadTexture((char*)"res/textures/blending_window.png");
//	Window->AddCustomTexture(WindowTex, "InTex");
//
//
//	/*----------------------------------------------------
//		Part Render Target & Post Processing
//	----------------------------------------------------*/
//
//
//	// 屏幕纹理渲染缓冲数据
//	Shader* RTShader = new Shader("shader/PostProcess/Primitive.vs", "shader/PostProcess/Primitive.fs");
//	RTShader->Use();
//	RTShader->SetInt("RT", 0);
//
//	// MSAA预处理需要的FrameBuffer
//	FrameBuffer* MSAA_FB = new FrameBuffer(true, 4, SCR_WIDTH, SCR_HEIGHT);
//
//	// 后处理需要的FrameBuffer
//	FrameBuffer* PostProcess_FB = new FrameBuffer(false, SCR_WIDTH, SCR_HEIGHT);
//
//	// 铺屏四边形
//	vector<int> ScreenQuadAttri{2, 2};
//	SimpleRender* ScreenQuadRender = new SimpleRender(ScreenQuadAttri, quadVertices, sizeof(quadVertices));
//
//	// 离屏shader绑定对应Texture
//	ScreenQuadRender->BindTexture(PostProcess_FB->TexAttached);
//
//
//
//	/*----------------------------------------------------
//		Part Render Configuration
//	----------------------------------------------------*/
//
//	// 开启颜色混合
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 使用src颜色的Alpha进行混合, src颜色即当前frag颜色, dest颜色即缓冲中的颜色
//
//	float deltaTime = 0;
//	float lastFrame = static_cast<float>(glfwGetTime());
//
//
//	/*----------------------------------------------------
//		Part Render Loop
//	----------------------------------------------------*/
//
//
//    // 绘制循环
//    while (!glfwWindowShouldClose(window))
//    {
//		/*----------------------------------------------------
//		Loop 帧间隔deltaTime刷新
//		----------------------------------------------------*/
//
//
//		float currentFrame = static_cast<float>(glfwGetTime());
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//
//
//		/*----------------------------------------------------
//		Loop 输入处理及相机位置更新
//		计算通用View及Projection矩阵
//		----------------------------------------------------*/
//
//
//		processInput(deltaTime, window);
//
//		// Projection矩阵
//		glm::mat4 projectionMatrix = glm::perspective(glm::radians(CurCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); // 45度FOV, 视口长宽比, 近平面0.1, 远屏幕100
//
//		// View矩阵
//		glm::mat4 viewMatrix = CurCamera->LookAt();
//
//
//
//		/*----------------------------------------------------
//		Loop 启用FBO并重置buffer状态
//		----------------------------------------------------*/
//		
//
//	
//		glBindFramebuffer(GL_FRAMEBUFFER, MSAA_FB->FBO);
//
//
//		glEnable(GL_DEPTH_TEST); // 开启深度测试
//
//		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 设置背景色
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色及深度缓冲
//
//
//
//		/*----------------------------------------------------
//		Loop SkyBox
//		----------------------------------------------------*/
//
//
//		glDepthFunc(GL_LEQUAL);
//		glDepthMask(GL_FALSE);
//
//
//		SkyboxShader->Use();
//
//		// 天空盒特殊View矩阵
//		glm::mat4 viewMatrixSkybox = glm::mat4(glm::mat3(viewMatrix));
//		SkyboxShader->SetMat4("view", viewMatrixSkybox);
//		SkyboxShader->SetMat4("projection", projectionMatrix);
//
//		SkyboxRender->Draw(false);
//
//		glDepthMask(GL_TRUE);
//		glDepthFunc(GL_LESS);
//
//
//
//		/*----------------------------------------------------
//		Loop 地板
//		----------------------------------------------------*/
//
//
//		// 关闭面剔除
//		glDisable(GL_CULL_FACE);
//
//		// 绘制Plan
//		SingleTexShader->Use();
//
//		glm::mat4 modelMatrixFloor = glm::mat4(1.0f); // Plan Model矩阵
//
//		Plan->Draw(SingleTexShader, modelMatrixFloor, viewMatrix, projectionMatrix);
//
//		// 开启面剔除
//		glEnable(GL_CULL_FACE);
//
//
//		/*----------------------------------------------------
//		Loop 光源
//		----------------------------------------------------*/
//
//		
//		SingleColorShader->Use();
//
//		// 光源Model矩阵
//		glm::mat4 modelMatrixLight = glm::mat4(1.0f);
//		modelMatrixLight = glm::translate(modelMatrixLight, LightPos);
//		modelMatrixLight = glm::scale(modelMatrixLight, glm::vec3(0.2f));
//
//		LightObj->Draw(SingleColorShader, modelMatrixLight, viewMatrix, projectionMatrix);
//
//
//		/*----------------------------------------------------
//		Loop Obj
//		----------------------------------------------------*/
//		
//		// nanosuit Model矩阵
//		glm::mat4 modelMatrixObj = glm::mat4(1.0f);
//		modelMatrixObj = glm::translate(modelMatrixObj, cubePositions[0]);
//		modelMatrixObj = glm::scale(modelMatrixObj, glm::vec3(0.05f));
//		
//		// 设置模型shader的动态参数
//		ModelPhongShader->Use();
//		ModelPhongShader->SetVec3("ViewPos", CurCamera->Pos);
//		ModelPhongShader->SetVec3("spotLight.position", CurCamera->Pos);
//		ModelPhongShader->SetVec3("spotLight.direction", CurCamera->Front);
//		//ModelPhongShader->SetFloat("time", glfwGetTime()); // 用于法线爆破的GShader
//
//		// 赋予天空盒纹理
//		ModelPhongShader->SetInt("skybox", 3);
//		glActiveTexture(GL_TEXTURE0 + 3); // 模型的漫反射, 高光, 镜面纹理分别占据了1-3号纹理位置, 因此需要将天空盒设置为4号纹理
//		glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTex);
//
//		// 绘制模型
//		Obj->Draw(ModelPhongShader, modelMatrixObj, viewMatrix, projectionMatrix);
//
//
//		// nanosuit Model法线显示
//		//ModelNormalShader->Use();
//		//Obj->Draw(ModelNormalShader, modelMatrixObj, viewMatrix, projectionMatrix);
//
//
//		// Cube Model矩阵
//		modelMatrixObj = glm::mat4(1.0f);
//		modelMatrixObj = glm::translate(modelMatrixObj, cubePositions[1]);
//		modelMatrixObj = glm::scale(modelMatrixObj, glm::vec3(3.0f));
//		modelMatrixObj = glm::scale(modelMatrixObj, glm::vec3(3.0f));
//
//		SingleTexShader->Use();
//		Cube->Draw(SingleTexShader, modelMatrixObj, viewMatrix, projectionMatrix);
//
//
//		/*----------------------------------------------------
//		Loop Alpha Blending
//		----------------------------------------------------*/
//
//
//
//		// 根据相机距离进行排序
//		std::map<float, glm::vec3> sorted;
//		for (unsigned int i = 0; i < Windows_Pos.size(); i++)
//		{
//			float distance = glm::length(CurCamera->Pos - Windows_Pos[i]);
//			sorted[distance] = Windows_Pos[i];
//		}
//
//		SingleTexShader->Use();
//
//		// 根据排序的顺序由大到小渲染(由远及近)
//		glm::mat4 modelMatrixWindow;
//		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
//		{
//			modelMatrixWindow = glm::mat4(1.0f);
//			modelMatrixWindow = glm::translate(modelMatrixWindow, it->second);
//
//			Window->Draw(SingleTexShader, modelMatrixWindow, viewMatrix, projectionMatrix);
//		}
//
//
//
//		/*----------------------------------------------------
//		Loop FBO处理及渲染至屏幕 & 后处理
//		----------------------------------------------------*/
//
//
//		// MSAA纹理复制
//		glBindFramebuffer(GL_READ_FRAMEBUFFER, MSAA_FB->FBO);
//		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, PostProcess_FB->FBO);
//		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//
//		// 切换至默认Buffer
//		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
//
//		// 清除默认Buffer的缓冲并设置背景色
//		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		// 禁用深度缓冲
//		glDisable(GL_DEPTH_TEST);
//
//		// 渲染屏幕纹理
//		RTShader->Use();
//
//		ScreenQuadRender->Draw(false);
//
//
//
//
//		/*----------------------------------------------------
//		Loop End
//		----------------------------------------------------*/
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    // 退出程序
//    glfwTerminate();
//    return 0;
//}
//
//
//// 初始化GLFW
//void InitGLFW() 
//{
//    glfwInit();
//
//    // 设置OpenGL支持版本
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//
//    // 使用OpenGL核心模式
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//}
//
//
//// 创建窗口对象
//GLFWwindow* CreateWindow()
//{
//    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SoftRendererGL", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return nullptr;
//    }
//    glfwMakeContextCurrent(window);
//
//    // 设置窗口大小变化时的回调
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//    return window;
//}
//
//// 窗口改变回调
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    glViewport(0, 0, width, height);
//}
//
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(float deltaTime, GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		CurCamera->ProcessKeyboard(EMoveDir::FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		CurCamera->ProcessKeyboard(EMoveDir::BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		CurCamera->ProcessKeyboard(EMoveDir::LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		CurCamera->ProcessKeyboard(EMoveDir::RIGHT, deltaTime);
//}
//
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//bool firstMouse = true;
//float lastX = (float)SCR_WIDTH / 2.0;
//float lastY = (float)SCR_HEIGHT / 2.0;
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//	float xpos = static_cast<float>(xposIn);
//	float ypos = static_cast<float>(yposIn);
//
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//	lastX = xpos;
//	lastY = ypos;
//
//	CurCamera->ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//	CurCamera->ProcessMouseScroll(static_cast<float>(yoffset));
//}
//
//
//
//
//
