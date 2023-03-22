#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 MatColor;
uniform vec3 LightColor;

void main()
{
    FragColor = vec4(LightColor * MatColor, 1.0);
} 
