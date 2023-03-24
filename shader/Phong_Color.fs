#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

struct LightMat{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
};

uniform LightMat lightMat;
uniform Material material;
uniform vec3 LightPos;
uniform vec3 ViewPos;

void main()
{
    // 环境光
    vec3 ambient = lightMat.ambient * material.ambientColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);

    // 漫反射光
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightMat.diffuse * (diff * material.diffuseColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ViewPos - FragPos);

    // 镜面高光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightMat.specular * (spec * material.specularColor);

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
} 
