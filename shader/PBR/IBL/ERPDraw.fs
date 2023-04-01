#version 330 core

out vec4 FragColor;

in vec3 localPos;

uniform sampler2D equirectangularMap; // 等距柱状投影贴图

const vec2 invAtan = vec2(0.1591, 0.3183); // vec2(1 / pi, 1 / 2pi)

// 固定公式, pi = (u - 0.5) * 2pi, theta = (0.5 - v) * pi
// https://blog.csdn.net/messi_cyc/article/details/79030195
// https://blog.csdn.net/lin453701006/article/details/76919188
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y)); // (theta, phi)
    uv *= invAtan; // (theta / pi, phi / 2pi)
    uv += 0.5; // (theta / pi + 0.5, phi / 2pi + 0.5)
    return uv;
}

void main()
{       
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(equirectangularMap, uv).rgb;

    FragColor = vec4(color, 1.0);
}