#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D depthMap; // 纹理采样器
uniform float near_plane;
uniform float far_plane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC, [0, 1]->[-1, 1]
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float depthValue = texture(depthMap, texCoord).r;

    FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0);
} 
