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
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};
uniform Material material;

uniform vec3 ViewPos;

// 函数声明
vec3 calculate_paraLight(ParaLight lightInst, vec3 diffuseColor, vec3 specularColor);
vec3 calculate_pointLight(PointLight lightInst, vec3 diffuseColor, vec3 specularColor);
vec3 calculate_spotLight(SpotLight lightInst, vec3 diffuseColor, vec3 specularColor);

void main()
{
    vec3 diffuseColor = vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 specularColor = vec3(texture(material.texture_specular1, TexCoord));

    vec3 result = calculate_paraLight(paraLight, diffuseColor, specularColor);
    result += calculate_pointLight(pointLight, diffuseColor, specularColor);
    // result += calculate_spotLight(spotLight, diffuseColor, specularColor);

    FragColor = vec4(result, 1.0);
} 

// 平行光
vec3 calculate_paraLight(ParaLight lightInst, vec3 diffuseColor, vec3 specularColor)
{
    // 环境光
    vec3 ambient = lightInst.ambient * diffuseColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-lightInst.direction);

    // 漫反射光
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightInst.diffuse * (diff * diffuseColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ViewPos - FragPos);

    // 镜面高光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightInst.specular * (spec * specularColor);

    return ambient + diffuse + specular;
}

// 点光源
vec3 calculate_pointLight(PointLight lightInst, vec3 diffuseColor, vec3 specularColor)
{
    // 环境光
    vec3 ambient = lightInst.ambient * diffuseColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightInst.position - FragPos);

    // 漫反射光
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightInst.diffuse * (diff * diffuseColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ViewPos - FragPos);

    // 镜面高光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightInst.specular * (spec * specularColor);

    // 点光源距离衰减因子
    float distance = length(lightInst.position - FragPos);
    float attenuation = 1.0 / (lightInst.constant + lightInst.linear * distance + lightInst.quadratic * distance * distance);

    return (ambient + diffuse + specular) * attenuation;
}

// 投射光
vec3 calculate_spotLight(SpotLight lightInst, vec3 diffuseColor, vec3 specularColor)
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightInst.position - FragPos);

    float theta = dot(lightDir, normalize(-lightInst.direction));
    float epsilon = lightInst.innerCutOff - lightInst.outerCutOff;
    float intensity = clamp((theta - lightInst.outerCutOff) / epsilon, 0.0, 1.0);

    // 漫反射光
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightInst.diffuse * (diff * diffuseColor);

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ViewPos - FragPos);

    // 镜面高光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightInst.specular * (spec * specularColor);

    return (diffuse + specular) * intensity;
}
