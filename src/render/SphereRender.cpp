#include "SphereRender.h"

const float PI = 3.14159265359f;

SphereRender::SphereRender(float InRadius, GLuint InXSegments, GLuint InYSegments)
{
	Radius = InRadius;
	XSegments = InXSegments;
	YSegments = InYSegments;

	SetupMesh();
}

void SphereRender::SetupMesh()
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	
	// 生成顶点数组, theta[0, PI]纬度, phi[0, 2PI]经度
	// 球坐标系, x = radius * sin(theta) * cos(phi)
	// 球坐标系, y = radius * cos(theta)
	// 球坐标系, x = radius * sin(theta) * sin(phi)
	for (unsigned int x = 0; x <= XSegments; ++x)
	{
		for (unsigned int y = 0; y <= YSegments; ++y)
		{
			float xSegment = (float)x * PI / (float)XSegments;
			float ySegment = (float)y * PI / (float)YSegments;
			float xPos = Radius * std::sin(ySegment) * std::cos(xSegment * 2);
			float yPos = Radius * std::cos(ySegment);
			float zPos = Radius * std::sin(ySegment) * std::sin(xSegment * 2);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
			uv.push_back(glm::vec2(xSegment, ySegment));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	// 交错生成indices数组
	bool oddRow = false;
	for (unsigned int y = 0; y < YSegments; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (unsigned int x = 0; x <= XSegments; ++x)
			{
				indices.push_back(y * (XSegments + 1) + x);
				indices.push_back((y + 1) * (XSegments + 1) + x);
			}
		}
		else
		{
			for (int x = XSegments; x >= 0; --x)
			{
				indices.push_back((y + 1) * (XSegments + 1) + x);
				indices.push_back(y * (XSegments + 1) + x);
			}
		}
		oddRow = !oddRow;
	}

	IndexCount = static_cast<GLuint>(indices.size());

	// 组织成顶点着色器可用的顶点列表
	std::vector<float> data;
	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		if (normals.size() > 0)
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}
		if (uv.size() > 0)
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
	}

	// 绑定顶点缓冲
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	unsigned int stride = (3 + 2 + 3) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
}

void SphereRender::Draw(Shader* shader, glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	// Model矩阵
	int modelLoc = glGetUniformLocation(shader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// View矩阵
	int viewLoc = glGetUniformLocation(shader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Projection矩阵
	int projectionLoc = glGetUniformLocation(shader->ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, IndexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}




