#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D InTex; // 纹理采样器

void main()
{
    FragColor = texture(InTex, texCoord);
} 
