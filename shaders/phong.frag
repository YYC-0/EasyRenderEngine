#version 460 core
#extension GL_EXT_texture_array : enable
out vec4 FragColor;

uniform float shininess;
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;   

    sampler2D diffuseT;
    sampler2D normalT;
    sampler2D specularT; 
};

uniform bool useDiffuseMap;
uniform bool useSpecularMap;
uniform bool useNormalMap;

struct Light {
    int type;
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

in vec4 FragPosLightSpace[5];   // frag position in directional light space
 
uniform vec3 viewPos;
uniform Material mtl;
uniform Light lights[16];
uniform int lightNum;
uniform sampler2DArray shadowMap;
uniform samplerCubeArray cubeDepthMap;
uniform float far_plane;

vec3 computeLight(Light light, vec3 objDiffuse, vec3 objSpecular);
float dirShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, int dirLightNum);
float pointShadowCalculation(vec3 fragPos, vec3 lightPos, int pointLightNum);

int dirLightNum = 0;
int pointLightNum = 0;
vec3 normal;
void main()
{
    normal = normalize(Normal);
    if(useNormalMap)
    {
        normal = texture(mtl.normalT, TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    }
//    FragColor = vec4(normal, 1.0);
    vec3 objDiffuse;
    vec3 objSpecular;
    if(useDiffuseMap)
        objDiffuse = texture(mtl.diffuseT, TexCoords).rgb;
    else
        objDiffuse = mtl.ambient;
    if(useSpecularMap)
        objSpecular = texture(mtl.specularT, TexCoords).rgb;
    else
        objSpecular = mtl.specular;

    vec3 result;
    for(int i=0; i<lightNum; ++i)
    {
        result += computeLight(lights[i], objDiffuse, objSpecular);
    }
    
    FragColor = vec4(result, 1.0);
    
//    vec3 fragToLight = FragPos - lights[0].position;
//    float closestDepth = texture(cubeDepthMap, vec4(fragToLight, pointLightNum)).r;
//    closestDepth *= far_plane;
//    FragColor = vec4(vec3(closestDepth / far_plane), 1.0);

} 

vec3 computeLight(Light light, vec3 objDiffuse, vec3 objSpecular)
{
    vec3 lightDir;
//    if(light.type == 0) // point light
//        lightDir = normalize(light.position - FragPos);
//    else if(light.type == 1) // directional light
//        lightDir = normalize(-light.direction);
    lightDir = normalize(light.type * (-light.direction) + (1-light.type) * (light.position - FragPos));

    // ambient
    vec3 ambient;
    ambient = light.ambient * objDiffuse;
  	
    // diffuse 
    vec3 diffuse;
    float diff = max(dot(normal, lightDir), 0.0);
    diffuse = light.diffuse * diff * objDiffuse;
    
    // specular
    vec3 specular;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    specular = light.specular * spec * objSpecular;  
    
    float attenuation = 1.0;
    // shadow
    float shadow = 0;
    if(light.type == 0) // point light
    {
        // attenuation
        float dis = length(light.position - FragPos);
        attenuation = 1.0 / (light.constant + light.linear*dis + light.quadratic*(dis*dis));
        shadow = pointShadowCalculation(FragPos, light.position, pointLightNum);
        pointLightNum++;
    }
    else if(light.type == 1) // directional light
    {
        shadow = dirShadowCalculation(FragPosLightSpace[dirLightNum], lightDir, dirLightNum);
        dirLightNum++;
    }

    vec3 result = attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
    
    return result;
    //return vec3(shadow, 0, 0);
}

float dirShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, int dirLightNum)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.0;
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0;
    float currentDepth = projCoords.z;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;
    for(int x=-1; x<=1; ++x)
        for(int y=-1; y<=1; ++y)
        {    
            //float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y)*texelSize).r;
            float pcfDepth = texture2DArray(shadowMap, vec3(projCoords.xy + vec2(x,y)*texelSize, dirLightNum)).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    shadow /= 9.0;
    return shadow;
}
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);
float pointShadowCalculation(vec3 fragPos, vec3 lightPos, int pointLightNum)
{
    vec3 fragToLight = fragPos - lightPos;
    //float closestDepth = texture(cubeDepthMap, fragToLight).r;
    float shadow = 0.0;
    float diskRadius = 0.05;
    int samples = 20;
    for(int i=0; i<samples; ++i)
    {
        vec3 samplePos = fragToLight + sampleOffsetDirections[i]*diskRadius;
        float closestDepth = texture(cubeDepthMap, vec4(samplePos, pointLightNum)).r;
        closestDepth *= far_plane;
        float currentDepth = length(fragToLight);

        shadow += currentDepth > closestDepth ? 1.0 : 0.0;
    }
    shadow /= float(samples);
    
    return shadow;
}