#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

// 平行光
struct ParaLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform ParaLight paraLight;

// 点光源
struct PointLight{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // 衰减
    float constant; // 常数项
    float linear; // 一阶项
    float quadratic; // 二阶项
};
uniform PointLight pointLight;

// 投射光
struct SpotLight{
    vec3  position;
    vec3  direction;

    vec3 diffuse;
    vec3 specular;

    float innerCutOff; // 投射角(半程)
    float outerCutOff; // 外轮廓角, 平滑边缘用
};
uniform SpotLight spotLight;

// 物体材质
struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};
uniform Material material;

uniform vec3 ViewPos;

// 函数声明
vec3 calculate_paraLight(vec3 diffuseColor, vec3 specularColor);
vec3 calculate_pointLight(vec3 diffuseColor, vec3 specularColor);
vec3 calculate_spotLight(vec3 diffuseColor, vec3 specularColor);

void main()
{
    vec3 diffuseColor = vec3(texture(material.diffuseMap, TexCoord));
    vec3 specularColor = vec3(texture(material.specularMap, TexCoord));

    vec3 result = calculate_pointLight(diffuseColor, specularColor) + calculate_spotLight(diffuseColor, specularColor);
    FragColor = vec4(result, 1.0);
} 

// 平行光
vec3 calculate_paraLight(vec3 diffuseColor, vec3 specularColor)
{
    // 环境光
    vec3 ambient = paraLight.ambient * diffuseColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-paraLight.direction);

    // 漫反射光
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = paraLight.diffuse * (diff * diffuseColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ViewPos - FragPos);

    // 镜面高光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = paraLight.specular * (spec * specularColor);

    return ambient + diffuse + specular;
}

// 点光源
vec3 calculate_pointLight(vec3 diffuseColor, vec3 specularColor)
{
    // 环境光
    vec3 ambient = pointLight.ambient * diffuseColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);

    // 漫反射光
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * (diff * diffuseColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ViewPos - FragPos);

    // 镜面高光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pointLight.specular * (spec * specularColor);

    // 点光源距离衰减因子
    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * distance * distance);

    return (ambient + diffuse + specular) * attenuation;
}

// 投射光
vec3 calculate_spotLight(vec3 diffuseColor, vec3 specularColor)
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(spotLight.position - FragPos);

    float theta = dot(lightDir, normalize(-spotLight.direction));
    float epsilon = spotLight.innerCutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

    // 漫反射光
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * (diff * diffuseColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ViewPos - FragPos);

    // 镜面高光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spotLight.specular * (spec * specularColor);

    return (diffuse + specular) * intensity;
}
