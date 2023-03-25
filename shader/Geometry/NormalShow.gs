#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 vNormal[];

uniform mat4 projection;

const float MAGNITUDE = 0.01;

void GenerateLine(int index)
{   
    // 输出原本顶点
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    
    // 输出在顶点位置法线扩张一定距离的另一个顶点, 且组成一个line
    gl_Position = projection * (gl_in[index].gl_Position + vec4(vNormal[index], 0.0) * MAGNITUDE);
    EmitVertex();

    EndPrimitive();
}

void main() {    
    GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}