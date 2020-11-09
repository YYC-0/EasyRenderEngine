#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace[5];

uniform mat4 model;
uniform mat4 transInvModel;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix[5];
uniform int dirLightNum;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = vec3(transInvModel * vec4(aNormal, 1.0));
    TexCoords = aTexCoords;
    for(int i=0; i<5; ++i)
        FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
