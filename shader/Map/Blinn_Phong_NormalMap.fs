#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

// 点光源
struct PointLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // 衰减
    float constant; // 常数项
    float linear; // 一阶项
    float quadratic; // 二阶项
};
uniform PointLight pointLight;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

// 函数声明
vec3 calculate_pointLight(PointLight lightInst, vec3 diffuseColor, vec3 normal);

void main()
{
    vec3 diffuseColor = vec3(texture(diffuseTex, fs_in.TexCoord));
    vec3 normal = vec3(texture(normalTex, fs_in.TexCoord)).rgb;
    normal = normalize(normal * 2 - 1);

    vec3 result = calculate_pointLight(pointLight, diffuseColor, normal);
    FragColor = vec4(result, 1.0);
} 

// 点光源
vec3 calculate_pointLight(PointLight lightInst, vec3 diffuseColor, vec3 normal)
{
    // 环境光
    vec3 ambient = lightInst.ambient * diffuseColor;

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    // 漫反射光
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightInst.diffuse * (diff * diffuseColor);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // 镜面高光
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = lightInst.specular * (spec * diffuseColor);

    // 点光源距离衰减因子
    float distance = length(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float attenuation = 1.0 / (lightInst.constant + lightInst.linear * distance + lightInst.quadratic * distance * distance);

    return (ambient + diffuse + specular) * attenuation;
}
