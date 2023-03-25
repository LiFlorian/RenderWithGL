#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D RT; // 纹理采样器

void main()
{
    FragColor = vec4(1.0 - texture(RT, texCoord).rgb, 1.0);
} 
