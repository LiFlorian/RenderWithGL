﻿#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 LightPos;
uniform vec3 ViewPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoord = aTexCoord;

    vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    mat3 inverseTBN = transpose(mat3(T, B, N));

    vs_out.TangentLightPos = inverseTBN * LightPos;
    vs_out.TangentViewPos = inverseTBN * ViewPos;
    vs_out.TangentFragPos = inverseTBN * vs_out.FragPos;
}
