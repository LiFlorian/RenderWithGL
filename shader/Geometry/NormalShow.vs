#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 vNormal;

uniform mat4 model;
uniform mat4 view;

void main()
{
    // 因为GS中生成的另一个顶点也需要乘projection矩阵以适应相机变化, 因此这里先将pos变换至view空间, GS中再变换至最终坐标
    gl_Position = view * model * vec4(aPos, 1.0);

    // 相应地, 因为输入法线本来就是最终坐标, 因此这里先乘以model和view的逆矩阵, 同样将其变换至view空间
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vNormal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}
