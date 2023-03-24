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

// 初级光照phong模型以及三种光源
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
//	// 创建场景相机
//	CurCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//
//	glfwSetCursorPosCallback(window, mouse_callback); // 鼠标滑动回调
//	glfwSetScrollCallback(window, scroll_callback); // 鼠标滚轮回调
//
//	// 定义常数
//	glm::vec3 LightPos = glm::vec3(1.2f, 1.0f, 2.0f);
//
//	// 物体绘制所需Shader以及Context
//	Shader* ContextShader = new Shader("shader/Light/Phong_TextureMap.vs", "shader/Light/Phong_TextureMap.fs");
//	RenderContext* Context = new RenderContext(ContextShader, EVertexType::EPos_Normal_Tex, Cube_NormalTexVert, sizeof(Cube_NormalTexVert));
//	ContextShader->Use();
//	// 平行光参数
//	ContextShader->SetVec3("paraLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
//	ContextShader->SetVec3("paraLight.ambient", glm::vec3(0.02f, 0.02f, 0.02f));
//	ContextShader->SetVec3("paraLight.diffuse", glm::vec3(0.05f, 0.05f, 0.05f));
//	ContextShader->SetVec3("paraLight.specular", glm::vec3(0.1f, 0.1f, 0.1f));
//	// 点光源静态参数
//	ContextShader->SetVec3("pointLight.position", LightPos);
//	ContextShader->SetVec3("pointLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
//	ContextShader->SetVec3("pointLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
//	ContextShader->SetVec3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
//	ContextShader->SetFloat("pointLight.constant", 1.0f);
//	ContextShader->SetFloat("pointLight.linear", 0.09f);
//	ContextShader->SetFloat("pointLight.quadratic", 0.032f);
//	// 投射光静态参数
//	ContextShader->SetVec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
//	ContextShader->SetVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
//	ContextShader->SetFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
//	ContextShader->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
//
//	ContextShader->SetFloat("material.shininess", 32.0f);
//	unsigned int DiffuseMap = LoadTexture("res/map/container_diffuseMap.png");
//	ContextShader->SetInt("material.diffuseMap", 0);
//	Context->BindTexture(DiffuseMap);
//	unsigned int SpecularMap = LoadTexture("res/map/container2_specularMap.png");
//	ContextShader->SetInt("material.specularMap", 1);
//	Context->BindTexture(SpecularMap);
//
//	// 光源绘制所需的Shader及Obj
//	Shader* LightShader = new Shader("shader/Light/Light_Base.vs", "shader/Light/Light_Base.fs");
//	Light* OneLight = new Light(LightShader, LightPos);
//	OneLight->BindBufferData(Context->VBO); // 都是正方体, 先用Context的VBO绑定自己的VAO
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
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// 渲染背景
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		float currentFrame = static_cast<float>(glfwGetTime());
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//		processInput(deltaTime, window);
//
//		// Projection矩阵
//		glm::mat4 projection = glm::mat4(1.0f);
//		projection = glm::perspective(glm::radians(CurCamera->Zoom), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f); // 45度FOV, 视口长宽比, 近平面0.1, 远屏幕100
//
//		// View矩阵
//		glm::mat4 view = CurCamera->LookAt();
//
//		// 绘制Context内容
//		ContextShader->Use();
//		ContextShader->SetVec3("ViewPos", CurCamera->Pos);
//		ContextShader->SetVec3("spotLight.position", CurCamera->Pos);
//		ContextShader->SetVec3("spotLight.direction", CurCamera->Front);
//		Context->ActiveBindedTextures();
//
//		for (int i = 1; i < 10; i++)
//		{
//			// Model矩阵
//			glm::mat4 model = glm::mat4(1.0f);
//			model = glm::translate(model, cubePositions[i]);
//			float angle = 20.0f * i;
//			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//
//			Context->SetVertexTransform(model, view, projection);
//			Context->DrawElements(false, EDrawType::ECube);
//		}
//
//		// 绘制光源
//		LightShader->Use();
//		OneLight->Draw(view, projection);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	delete Context;
//
//	// 退出程序
//	glfwTerminate();
//	return 0;
//}



/*----------------------------------------------------
		模板测试绘制模型外轮廓管线流程
----------------------------------------------------*/

//glEnable(GL_STENCIL_TEST);
//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // 设置模板缓冲失败及深度测试失败时什么都不做, 成功时写入模板缓冲
//glStencilMask(0xFF); // 启用模板缓冲写入
//glStencilFunc(GL_ALWAYS, 1, 0xFF); // 掩码设为1, ref设为1, 接下来所有绘制的像素都将在模板缓冲中写入1
//
//// 模型Model矩阵
//glm::mat4 modelMatrix = glm::mat4(1.0f);
//modelMatrix = glm::translate(modelMatrix, cubePositions[0]);
//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
//
//// 设置模型shader的动态参数
//ModelShader->Use();
//ModelShader->SetVec3("ViewPos", CurCamera->Pos);
//ModelShader->SetVec3("spotLight.position", CurCamera->Pos);
//ModelShader->SetVec3("spotLight.direction", CurCamera->Front);
//
//// 绘制模型
//Model->Draw(ModelShader, modelMatrix, viewMatrix, projectionMatrix);
//
//
//glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // 掩码设为1, ref设为1, NOTEQUAL表示禁用之前模板缓冲写入1的区域
//glStencilMask(0x00); // 禁止模板缓冲的写入
//glDisable(GL_DEPTH_TEST);
//
//
//// 绘制外轮廓
//modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1f));
//
//OutlineShader->Use();
//Model->Draw(OutlineShader, modelMatrix, viewMatrix, projectionMatrix);
//
//glStencilMask(0xFF);
//glEnable(GL_DEPTH_TEST);
//glDisable(GL_STENCIL_TEST);
