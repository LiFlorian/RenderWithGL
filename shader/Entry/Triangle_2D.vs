#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor; // 颜色变量
layout (location = 2) in vec2 aTexCoord; // 纹理坐标

out vec3 vertexColor; // 为片段着色器指定一个颜色输出
out vec2 texCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);

	vertexColor = aColor;
    texCoord = aTexCoord;
}
