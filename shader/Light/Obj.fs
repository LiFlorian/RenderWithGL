#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 MatColor;
uniform vec3 LightColor;
uniform vec3 LightPos;

void main()
{
    // 环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);

    // 漫反射光
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightColor;

    vec3 result = (ambient + diffuse) * MatColor;

    FragColor = vec4(result, 1.0);
} 
