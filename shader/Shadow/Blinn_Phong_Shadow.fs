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

    // 执行透视除法
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    // [-1, 1]线性映射至[0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    // 应用阴影偏移, 防止出现明暗条纹
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // 对深度贴图采样得到光源空间的深度, 使用周围纹素阴影平均以达到抗锯齿效果
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float closestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += projCoords.z - bias > closestDepth ? 1.0 : 0.0;  
        }
    }
    shadow /= 9.0;

    // 超出光锥远平面的范围, 直接将阴影设置为0
    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }

    // 计算包含阴影的颜色值
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor;    

    FragColor = vec4(lighting, 1.0f);
} 
