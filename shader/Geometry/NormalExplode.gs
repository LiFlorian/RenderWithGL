#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vNormal[];
in vec3 vFragPos[];
in vec2 vTexCoord[];

out vec3 gNormal;
out vec3 gFragPos;
out vec2 gTexCoord; 

uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 1.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 

    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);

    return normalize(cross(a, b));
}

void main() {    
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    gNormal = vNormal[0];
    gFragPos = vFragPos[0];
    gTexCoord = vTexCoord[0];
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, normal);
    gNormal = vNormal[1];
    gFragPos = vFragPos[1];
    gTexCoord = vTexCoord[1];
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normal);
    gNormal = vNormal[2];
    gFragPos = vFragPos[2];
    gTexCoord = vTexCoord[2];
    EmitVertex();

    EndPrimitive();
}