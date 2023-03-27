#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTex;
uniform sampler2D shadowMap;

uniform vec3 ViewPos;
uniform vec3 LightPos;

// 函数声明
// vec3 calculate_paraLight(ParaLight lightInst, vec3 diffuseColor, vec3 specularColor);
// vec3 calculate_pointLight(PointLight lightInst, vec3 diffuseColor, vec3 specularColor);
// vec3 calculate_spotLight(SpotLight lightInst, vec3 diffuseColor, vec3 specularColor);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // [-1, 1]线性映射至[0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    return projCoords.z > closestDepth  ? 1.0 : 0.0;
}

void main()
{
    vec3 texColor = texture(diffuseTex, fs_in.TexCoord).rgb;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);

    // Ambient
    vec3 ambient = 0.15 * texColor;

    // Diffuse
    vec3 lightDir = normalize(LightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(ViewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;   

    // 计算阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor;    

    FragColor = vec4(lighting, 1.0f);
} 
