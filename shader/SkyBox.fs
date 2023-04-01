#version 330 core

out vec4 FragColor;

in vec3 sampleCoord;

uniform samplerCube skyboxTex;

void main()
{
    vec3 sampleColor = texture(skyboxTex, sampleCoord).rgb;

    // gamma校正
    sampleColor = sampleColor / (sampleColor + vec3(1.0));
    sampleColor = pow(sampleColor, vec3(1.0/2.2)); 

    FragColor = vec4(sampleColor, 1.0);
} 
