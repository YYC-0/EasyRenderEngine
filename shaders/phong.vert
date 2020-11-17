#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace[5];
out mat3 TBN;

uniform mat4 model;
uniform mat4 transInvModel;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix[5];

void main()
{
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = vec3(transInvModel * vec4(normal, 1.0));
    TexCoords = texCoords;
    for(int i=0; i<5; ++i)
        FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0);

    // compute TBN matrix
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    TBN = mat3(T, B, N);
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
