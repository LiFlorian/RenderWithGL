#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// PBR材质属性
uniform sampler2D albedoTex;
uniform sampler2D metallicTex;
uniform sampler2D roughnessTex;
uniform sampler2D aoTex;
uniform sampler2D normalTex;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 ViewPos;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{   
    // 切线空间法线, 映射至[-1, 1]
    vec3 tangentNormal = texture(normalTex, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ----------------------------------------------------------------------------
void main()
{		
    vec3 albedo = pow(texture(albedoTex, TexCoords).rgb, vec3(2.2)); // albedo需要映射至线性空间
    float ao = texture(aoTex, TexCoords).r;
    float metallic = texture(metallicTex, TexCoords).r;
    float roughness = texture(roughnessTex, TexCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(ViewPos - WorldPos);

    // 菲涅尔参数, 非金属默认0.04, 金属度插值得出
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // 反射方程
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // Li(p, wi), 光源i的辐射率
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness); // 法线分布函数
        float G   = GeometrySmith(N, V, L, roughness); // 几何函数
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0); // 反射函数, 不同方向, 所以返回值是vec3
        
        // 反射量计算
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // 0.0001防止分母为0
        vec3 specular = numerator / denominator;
        
        // 镜面反射系数KS = 菲涅尔计算结果F
        vec3 kD = vec3(1.0) - F;
        kD *= 1.0 - metallic; // 金属没有漫反射, 所以这里乘以(1 - metallic)

        // n*wi
        float NdotL = max(dot(N, L), 0.0);        

        // 对应光线的完整辐射率
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   
    
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR映射
    color = color / (color + vec3(1.0));
    // Gamma校正
    color = pow(color, vec3(1.0 / 2.2)); 

    FragColor = vec4(color, 1.0);
}