#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gFlux;

struct Material {
    vec3 diffuse; 
    sampler2D diffuseT;
    sampler2D normalT;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

uniform bool useNormalMap;
uniform bool useDiffuseMap;
uniform Material mtl;

void main()
{
    gPosition = FragPos;
    gNormal = normalize(Normal);
    if(useNormalMap)
    {
        gNormal = texture(mtl.normalT, TexCoords).rgb;
        gNormal = normalize(gNormal * 2.0 - 1.0);
        gNormal = normalize(TBN * gNormal);
    }
    
    if(useDiffuseMap)
        gFlux = texture(mtl.diffuseT, TexCoords).rgb;
    else
        gFlux = mtl.diffuse;
}