#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform float texMix; // 在OpenGL程序代码中设定这个变量

uniform sampler2D texture1; // 纹理采样器
uniform sampler2D texture2; // 纹理采样器

void main()
{
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), texMix);
} 
