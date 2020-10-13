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
  
uniform vec3 viewPos;
uniform Material material;
uniform Texture textures;
uniform Light lights[16];
uniform int lightNum;

vec3 computeLight(Light light);

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
        
    vec3 result = ambient + diffuse + specular;

    return result;
}