#version 330 core

in vec2 TexCoords;

out float FragColor;

uniform sampler2D SSAOTex;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(SSAOTex, 0));

    // 4*4核模糊SSAO贴图
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(SSAOTex, TexCoords + offset).r;
        }
    }

    FragColor = result / (4.0 * 4.0);
}