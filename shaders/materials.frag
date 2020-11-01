#version 330 core
out vec4 FragColor;

uniform float shininess;
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
};

uniform bool useDiffuseMap;
uniform bool useSpecularMap;
uniform bool useNormalMap;

struct Texture {
    sampler2D diffuse;
    sampler2D normal;
    sampler2D specular;
};

struct Light {
    int type;
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;
  
uniform vec3 viewPos;
uniform Material material;
uniform Texture textures;
uniform Light lights[16];
uniform int lightNum;
uniform sampler2D shadowMap;


vec3 computeLight(Light light);
float shadowCompute(vec4 fragPosLightSpace, vec3 lightDir);

void main()
{
    vec3 result;
    for(int i=0; i<lightNum; ++i)
    {
        result += computeLight(lights[i]);
    }
    
    FragColor = vec4(result, 1.0);
} 

vec3 computeLight(Light light)
{
    vec3 lightDir;
    if(light.type == 0) // point light
        lightDir = normalize(light.position - FragPos);
    else if(light.type == 1) // directional light
        lightDir = normalize(-light.direction);

    // ambient
    vec3 ambient;
    if(useDiffuseMap)
        ambient = light.ambient * texture(textures.diffuse, TexCoords).rgb;
    else
        ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 diffuse;
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    if(useDiffuseMap)
        diffuse = light.diffuse * diff * texture(textures.diffuse, TexCoords).rgb;
    else
        diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 specular;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    if(useSpecularMap)
        specular = light.specular * spec * texture(textures.specular, TexCoords).rgb;
    else
        specular = light.specular * (spec * material.specular);  
    
    // shadow
    float shadow = shadowCompute(FragPosLightSpace, lightDir);
    
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);
    
    return result;
    //return vec3(shadow, 0, 0);
}

float shadowCompute(vec4 fragPosLightSpace, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.0;
    //float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);
    float shadow = 0;
    float currentDepth = projCoords.z;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x=-1; x<=1; ++x)
        for(int y=-1; y<=1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y)*texelSize).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    shadow /= 9.0;
    return shadow;
}