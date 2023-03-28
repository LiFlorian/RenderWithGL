#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D depthMap; // 纹理采样器

void main()
{
    FragColor = vec4(vec3(texture(depthMap, texCoord).r), 1.0);
} 
