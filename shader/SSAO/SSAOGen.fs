#version 330 core

out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform mat4 projection;

uniform vec3 samples[64];

uniform int kernelSize;
uniform int noiseSize;

uniform float Screen_Width;
uniform float Screen_Height;


float radius = 0.5;
float bias = 0.025;

void main()
{
    vec2 noiseScale = vec2(Screen_Width / noiseSize, Screen_Height / noiseSize);

    // get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz); // Texcoords是[0, 1]范围, 但texNoise是以noiseSize为单位Repeat到整个Screen的, 这里进行对应纹理坐标的映射

    // 施密特正交化求出TBN矩阵, randomVec与normal非正交, 这里通过randomVec计算出与normal正交的向量, 用作切线
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // 变换至标准裁剪空间
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
        
        // smoothstep是S型曲线, 在[0, 1]之间光滑插值, 这里是为了去掉远处距离像素对遮蔽因子的影响, 即范围检测
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}