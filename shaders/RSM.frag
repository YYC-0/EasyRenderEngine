#version 460 core
#extension GL_EXT_texture_array : enable
const int SAMPLE_NUM = 800;
const float PI = 3.141592653589793;
const float PI2 = 6.283185307179586;

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

in vec4 RSM_FragPoslightSpace;
uniform sampler2D RSM_Depth;
uniform sampler2D RSM_Position;
uniform sampler2D RSM_Normal;
uniform sampler2D RSM_Flux;

vec3 computeLight(Light light, vec3 objDiffuse, vec3 objSpecular);
float dirShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, int dirLightNum);
float pointShadowCalculation(vec3 fragPos, vec3 lightPos, int pointLightNum);
vec3 RSM();

int dirLightNum = 0;
int pointLightNum = 0;
vec3 normal;

float random(float x){
    return fract(sin(x)*100000.0);
}

void main()
{
    normal = normalize(Normal);
    if(useNormalMap)
    {
        normal = texture(mtl.normalT, TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    }

    vec3 objDiffuse;
    vec3 objSpecular;
    if(useDiffuseMap)
        objDiffuse = texture(mtl.diffuseT, TexCoords).rgb;
    else
        objDiffuse = mtl.diffuse;
    if(useSpecularMap)
        objSpecular = texture(mtl.specularT, TexCoords).rgb;
    else
        objSpecular = mtl.specular;

    vec3 directIllumination;
    vec3 indirectIllumination = RSM();
    for(int i=0; i<lightNum; ++i)
    {

        directIllumination += computeLight(lights[i], objDiffuse, objSpecular);
    }

    vec3 result = directIllumination + 0.1 * indirectIllumination;

    FragColor = vec4(result, 1.0);
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
    //return vec3(shadow, shadow, shadow);
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
    for(int x=-2; x<=2; ++x)
        for(int y=-2; y<=2; ++y)
        {    
            //float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y)*texelSize).r;
            float pcfDepth = texture2DArray(shadowMap, vec3(projCoords.xy + vec2(x,y)*texelSize, dirLightNum)).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    shadow /= 25.0;
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

vec3 RSM()
{
    vec3 indirectIllumination = vec3(0.0);
    vec2 texelSize = 1.0 / textureSize(RSM_Position, 0).xy;

    vec3 coord = RSM_FragPoslightSpace.xyz / RSM_FragPoslightSpace.w;
    coord = coord * 0.5 + 0.5;

    float rMax = textureSize(RSM_Position, 0).x / 4.0;
    for(float i=1; i<=SAMPLE_NUM; i+=1.0)
    {
        float r1 = random(i);
        float r2 = random(i+0.5);
        vec2 c = coord.xy + rMax * vec2(r1*sin(PI2*r2), r1*cos(PI2*r2)) * texelSize;
        vec3 flux = texture(RSM_Flux, c).rgb;
        vec3 xp = texture(RSM_Position, c).rgb;
        vec3 np = texture(RSM_Normal, c).rgb;
        vec3 e = flux * (max(0, dot(np,FragPos-xp)) * max(0, dot(normal,xp-FragPos)) / pow(distance(xp,FragPos),2.0));
        indirectIllumination += r1*r1 * e;
    }
    return indirectIllumination;
}