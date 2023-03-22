// 第一章绘制简单2D图形的管线流程
// 备份用, 使用时复制到main函数中, 后续抽象重构
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


// 第一章后半绘制场景相机+立方体的流程
//int main()
//{
//	InitGLFW();
//
//	GLFWwindow* window = CreateWindow();
//
//	// 初始化glad
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return -1;
//	}
//
//	curCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//
//	// Viewport聚焦时隐藏光标
//	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//	glfwSetCursorPosCallback(window, mouse_callback); // 鼠标滑动回调
//	glfwSetScrollCallback(window, scroll_callback); // 鼠标滚轮回调
//
//	Shader* ContextShader = new Shader("shader/Entry/3D_Base.vs", "shader/Entry/3D_Base.fs");
//	RenderContext* Context = new RenderContext(EVertexType::EPos_Tex, Cube_TexVert, sizeof(Cube_TexVert));
//	unsigned int Texture1 = LoadTexture("res/textures/container.jpg", GL_RGB);
//	unsigned int Texture2 = LoadTexture("res/textures/awesomeface.png", GL_RGBA, true);
//	CurShader->Use();
//	CurShader->SetInt("texture1", 0);
//	CurShader->SetInt("texture2", 1);
//	CurShader->SetFloat("texMix", 0.2);
//
//	glEnable(GL_DEPTH_TEST); // 开启深度测试
//
//	float deltaTime = 0;
//	float lastFrame = static_cast<float>(glfwGetTime());
//
//	// 绘制循环
//	while (!glfwWindowShouldClose(window))
//	{
//		// 清除深度缓冲
//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//// 渲染背景
//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//glClear(GL_COLOR_BUFFER_BIT);
//
//float currentFrame = static_cast<float>(glfwGetTime());
//deltaTime = currentFrame - lastFrame;
//lastFrame = currentFrame;
//
//processInput(deltaTime, window);
//
//// Projection矩阵
//glm::mat4 projection = glm::mat4(1.0f);
//projection = glm::perspective(glm::radians(CurCamera->Zoom), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f); // 45度FOV, 视口长宽比, 近平面0.1, 远屏幕100
//
//// View矩阵
//glm::mat4 view = CurCamera->LookAt();
//
//// 定义颜色
//glm::vec3 LightColor = glm::vec3(1.0);
//glm::vec3 ObjColor = glm::vec3(1.0f, 0.5f, 0.31f);
//
//// 绘制Context内容
//ContextShader->Use();
//Context->ActiveTexture(Texture1);
//Context->ActiveTexture(Texture2);
//Context->SetVertexTransform(view, projection);
//Context->DrawElements(false, EDrawType::ECube);
//
//glfwSwapBuffers(window);
//glfwPollEvents();
//	}
//
//	delete Context;
//
//	// 退出程序
//	glfwTerminate();
//	return 0;
//}
