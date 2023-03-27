#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D RT; // 纹理采样器

void main()
{
    FragColor = vec4(vec3(texture(RT, texCoord).r), 1.0);
} 
