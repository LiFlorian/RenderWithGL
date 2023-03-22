#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

struct LightMat{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

uniform LightMat lightMat;
uniform Material material;
uniform vec3 LightPos;
uniform vec3 ViewPos;

void main()
{
    vec3 diffuseColor = vec3(texture(material.diffuseMap, TexCoord));
    vec3 specularColor = vec3(texture(material.specularMap, TexCoord));

    // 环境光
    vec3 ambient = lightMat.ambient * diffuseColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);

    // 漫反射光
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightMat.diffuse * (diff * diffuseColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ViewPos - FragPos);

    // 镜面高光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightMat.specular * (spec * specularColor);

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
} 
