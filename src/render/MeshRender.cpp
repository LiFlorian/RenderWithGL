#include "MeshRender.h"
#include "../render/RenderUtil.h"

MeshRender::MeshRender(float VertexList[], unsigned int VertexSize)
{
	for (int i = 0; i < VertexSize; i++)
	{
		Vertex vertex;

		vertex.Position.x = VertexList[i++];
		vertex.Position.y = VertexList[i++];
		vertex.Position.z = VertexList[i++];

		vertex.Normal.x = VertexList[i++];
		vertex.Normal.y = VertexList[i++];
		vertex.Normal.z = VertexList[i++];

		vertex.TexCoord.x = VertexList[i++];
		vertex.TexCoord.y = VertexList[i];

		vertices.push_back(vertex);
	}

	SetupMesh();
}

MeshRender::MeshRender(vector<Vertex> InVertices)
{
	vertices = InVertices;

	SetupMesh();
}

MeshRender::MeshRender(vector<Vertex> InVertices, vector<unsigned int> InIndices)
{
	vertices = InVertices;
	indices = InIndices;

	SetupMesh();
}

MeshRender::MeshRender(vector<Vertex> InVertices, vector<unsigned int> InIndices, vector<Texture> InTextures)
{
	vertices = InVertices;
	indices = InIndices;
	textures = InTextures;

	SetupMesh();
}

void MeshRender::SetupMesh()
{
	// 绑定VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 绑定VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// 如果有, 则绑定EBO
	if (!indices.empty()) {
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	// 顶点位置
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// 顶点法线
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// 顶点纹理坐标
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

	glBindVertexArray(0);
}

void MeshRender::Draw(Shader* shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	if (!textures.empty())
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元

			string number;
			string name = textures[i].type;

			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseNr++);
			}
			else if (name == "texture_specular")
			{
				number = std::to_string(specularNr++);
			}

			shader->SetInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].ID);
		}

		glActiveTexture(GL_TEXTURE0);
	}

	// Model矩阵
	int modelLoc = glGetUniformLocation(shader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// View矩阵
	int viewLoc = glGetUniformLocation(shader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Projection矩阵
	int projectionLoc = glGetUniformLocation(shader->ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// 绘制网格
	glBindVertexArray(VAO);

	// 根据有无EBO选择不同的绘制方式
	if (!indices.empty())
	{
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}

	glBindVertexArray(0);
}

void MeshRender::SetCustomTexture(char* path, string shaderTarget)
{
	
}

