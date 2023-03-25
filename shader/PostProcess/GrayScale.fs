#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D RT; // 纹理采样器

void main()
{
    FragColor = texture(RT, texCoord);

    float avarage = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;

    FragColor = vec4(avarage, avarage, avarage, 1.0);
} 
