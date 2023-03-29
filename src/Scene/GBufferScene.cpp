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
//#include "../render/GBuffer.h"
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
//	/*----------------------------------------------------
//		Part G-Buffer
//	----------------------------------------------------*/
//
//
//	GBuffer* GBufferInst = new GBuffer(SCR_WIDTH, SCR_HEIGHT);
//
//	// 绘制GBuffer所需Shader
//	Shader* GBufferRenderShader = new Shader("shader/DR/GBufferRender.vs", "shader/DR/GBufferRender.fs");
//
//	// 使用GBuffer绘制场景所需Shader
//	Shader* GBufferQuadShader = new Shader("shader/DR/GBufferQuad.vs", "shader/DR/GBufferQuad.fs");
//
//
//	// 铺屏四边形, 最后LightPass绘制用
//	vector<int> ScreenQuadAttri{ 3, 2 };
//	SimpleRender* ScreenQuadRender = new SimpleRender(ScreenQuadAttri, GBufferQuadVertices, sizeof(GBufferQuadVertices), GBufferQuadIndices, sizeof(GBufferQuadIndices));
//
//
//
//	// 屏幕纹理渲染缓冲数据, 用于观察G-Buffer的中间生成纹理, Debug
//	vector<int> DebugQuadAttri{ 2, 2 };
//	SimpleRender* DebugQuadRender = new SimpleRender(DebugQuadAttri, quadVertices, sizeof(quadVertices));
//
//	Shader* RTShader = new Shader("shader/PostProcess/Primitive.vs", "shader/PostProcess/Primitive.fs");
//	RTShader->Use();
//	RTShader->SetInt("RT", 0);
//
//
//
//	/*----------------------------------------------------
//		Part 光源
//	----------------------------------------------------*/
//
//	// 随机光源列表的位置及颜色
//	const GLuint NR_LIGHTS = 32;
//	std::vector<glm::vec3> lightPositions;
//	std::vector<glm::vec3> lightColors;
//	srand(13);
//	for (GLuint i = 0; i < NR_LIGHTS; i++)
//	{
//		// Calculate slightly random offsets
//		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
//		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
//
//		// Also calculate random color
//		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
//		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
//		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
//		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
//	}
//
//	// 光源Shader及静态参数
//	Shader* SingleColorShader = new Shader("shader/SingleColor.vs", "shader/SingleColor.fs");
//	SingleColorShader->Use();
//
//	// 光源Obj
//	MeshRender* LightRender = new MeshRender(Cube_NormalTexVert, sizeof(Cube_NormalTexVert) / sizeof(float));
//
//
//
//	/*----------------------------------------------------
//		Part Obj
//	----------------------------------------------------*/
//
//	// 模型实例的位置列表
//	std::vector<glm::vec3> objectPositions;
//	objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
//	objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
//	objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
//	objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
//	objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
//	objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
//	objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
//	objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
//	objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));
//
//	// 模型Obj
//	ModelRender* NanosuitRender = new ModelRender((char*)"res/model/nanosuit/nanosuit.obj");
//
//
//
//	/*----------------------------------------------------
//		Part Render Loop
//	----------------------------------------------------*/
//
//	float deltaTime = 0;
//	float lastFrame = static_cast<float>(glfwGetTime());
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
//		// View矩阵
//		glm::mat4 viewMatrix = CurCamera->LookAt();
//		// Model矩阵
//		glm::mat4 modelMatrix = glm::mat4(1.0f);
//
//
//		/*------------------------------------------------------------------------------------------------------------------
//			Loop GBuffer Pass
//		--------------------------------------------------------------------------------------------------------------------*/
//
//
//		glBindFramebuffer(GL_FRAMEBUFFER, GBufferInst->ID);
//
//
//		glEnable(GL_DEPTH_TEST); // 开启深度测试
//
//		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 设置背景色
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色及深度缓冲
//		
//		GBufferRenderShader->Use();
//		
//		for (GLuint i = 0; i < objectPositions.size(); i++)
//		{
//			modelMatrix = glm::mat4(1.0f);
//			modelMatrix = glm::translate(modelMatrix, objectPositions[i]);
//			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
//
//			NanosuitRender->Draw(GBufferRenderShader, modelMatrix, viewMatrix, projectionMatrix);
//		}
//
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//		/*--------------------------------------------------------------------------------------------------------------
//			Loop Light Pass
//		--------------------------------------------------------------------------------------------------------------*/
//
//
//		GBufferQuadShader->Use(); // 激活屏幕绘制Shader
//		GBufferQuadShader->SetVec3("ViewPos", CurCamera->Pos);
//
//		const GLfloat constant = 1.0;
//		const GLfloat linear = 0.7;
//		const GLfloat quadratic = 1.8;
//		for (GLuint i = 0; i < lightPositions.size(); i++)
//		{
//			glUniform3fv(glGetUniformLocation(GBufferQuadShader->ID, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &lightPositions[i][0]);
//			glUniform3fv(glGetUniformLocation(GBufferQuadShader->ID, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &lightColors[i][0]);
//			// Update attenuation parameters and calculate radius
//			glUniform1f(glGetUniformLocation(GBufferQuadShader->ID, ("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
//			glUniform1f(glGetUniformLocation(GBufferQuadShader->ID, ("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);
//		}
//
//		GBufferQuadShader->SetInt("gPosition", 0);
//		GBufferQuadShader->SetInt("gNormal", 1);
//		GBufferQuadShader->SetInt("gColorSpec", 2);
//		GBufferInst->ApplyBuffers(); // 激活GBuffer对应的Texture
//
//		ScreenQuadRender->Draw(false); // 绘制铺屏四边形
//
//
//		// Debug代码, 直接显示GBuffer的纹理附件
//		//RTShader->Use();
//		//glActiveTexture(GL_TEXTURE0);
//		//glBindTexture(GL_TEXTURE_2D, GBufferInst->gColorSpec);
//
//		//DebugQuadRender->Draw(false);
//
//
//
//		/*------------------------------------------------------------------------------------------------------------------
//			Loop Light Obj Forward Draw Pass
//		--------------------------------------------------------------------------------------------------------------------*/
//
//
//		// 复制GBuffer深度缓冲至当前帧缓冲
//		glBindFramebuffer(GL_READ_FRAMEBUFFER, GBufferInst->ID);
//		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
//		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		
//		
//		SingleColorShader->Use();
//
//		for (GLuint i = 0; i < lightPositions.size(); i++)
//		{
//			SingleColorShader->SetVec3("InColor", lightColors[i]);
//
//			modelMatrix = glm::mat4(1.0f);
//			modelMatrix = glm::translate(modelMatrix, lightPositions[i]);
//			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
//
//			LightRender->Draw(SingleColorShader, modelMatrix, viewMatrix, projectionMatrix);
//		}
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
