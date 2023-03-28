#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

uniform sampler2D diffuseTex;
uniform samplerCube depthCubeMap;

uniform vec3 ViewPos;
uniform vec3 LightPos;

uniform float far_plane;

float ShadowCalculation(vec3 fragPos)
{
    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - LightPos;
    // Use the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthCubeMap, fragToLight).r;
    // It is currently in linear range between [0,1]. Let's re-transform it back to original depth value
    closestDepth *= far_plane;
    // Now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // Now test for shadows
    float bias = 0.05; // We use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 texColor = texture(diffuseTex, fs_in.TexCoord).rgb;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);

    // Ambient
    vec3 ambient = 0.3 * texColor;

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

    float shadow = ShadowCalculation(fs_in.FragPos);    

    // 计算包含阴影的颜色值
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor;    

    FragColor = vec4(lighting, 1.0f);


    // depthCubeMap可视化, Debug用
    // vec3 fragToLight = fs_in.FragPos - LightPos; 
    // float closestDepth = texture(depthCubeMap, fragToLight).r;
    // FragColor = vec4(vec3(closestDepth), 1.0);
} 
