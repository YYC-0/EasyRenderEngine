#version 460 core
#extension GL_EXT_texture_array : enable

#define PI 3.14159265358979

out vec4 FragColor;

uniform float shininess;
struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    sampler2D albedoT;
    sampler2D normalT;
    sampler2D metallicT;
    sampler2D roughnessT;
    sampler2D aoT;
};

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
 
uniform bool useAlbedoMap;
uniform bool useNormalMap;
uniform bool useMatallicMap;
uniform bool useRoughnessMap;
uniform bool useAoMap;

uniform vec3 viewPos;
uniform Material mtl;
uniform Light lights[16];
uniform int lightNum;
uniform sampler2DArray shadowMap;
uniform samplerCubeArray cubeDepthMap;
uniform float far_plane;

int dirLightNum = 0;
int pointLightNum = 0;
vec3 N;
vec3 V;

vec3 computeLight(Light light, vec3 albedo, float metallic, float roughness);

// Normal Distribution Function
// roughness: [0,1]
float NDF_GGX(vec3 N, vec3 H, float roughness);

// Geometry Function
float G_SchlickGGX(float NDotV, float roughness); 
float G_Smith(float NdotV, float NdotL, float roughness);

// Fresnel Function 
vec3 Fresnel_Schlick(float NdotL, vec3 F0);

void main()
{
    // surface normal
    N = normalize(Normal);
    if(useNormalMap)
    {
        N = texture(mtl.normalT, TexCoords).rgb;
        N = normalize(N * 2.0 - 1.0);
        N = normalize(TBN * N);
    }

    // view direction
    V = normalize(viewPos - FragPos);

    //
    vec3 albedo = mtl.albedo;
    float metallic = mtl.metallic;
    float roughness = mtl.roughness;
    float ao = mtl.ao;
    if(useAlbedoMap)
        albedo = pow(texture(mtl.albedoT, TexCoords).rgb, vec3(2.2));
    if(useMatallicMap)
        metallic = texture(mtl.metallicT, TexCoords).r;
    if(useRoughnessMap)
        roughness = texture(mtl.roughnessT, TexCoords).r;
    if(useAoMap)
        ao = texture(mtl.aoT, TexCoords).r;

    // reflectance equation
    vec3 Lo;
    for(int i=0; i<lightNum; ++i)
    {
        Lo += computeLight(lights[i], albedo, metallic, roughness);
    }
    vec3 ambient = vec3(0.03) * mtl.albedo * mtl.ao;
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}

vec3 computeLight(Light light, vec3 albedo, float metallic, float roughness)
{
    // calculate per-light radiance -----------------------
    // light direction
    vec3 L = normalize(light.type * (-light.direction) + (1-light.type) * (light.position - FragPos));
    // halfway vector
    vec3 H = normalize(V + L); 	

    float NdotV = max(dot(N, V), 0.0); 	
    float NdotL = max(dot(N, L), 0.0);
    
    float attenuation = 1.0;
    if(light.type == 0) // point light
    {
        // attenuation
        float dis = length(light.position - FragPos);
        attenuation = 1.0 / (light.constant + light.linear*dis + light.quadratic*(dis*dis));
    }
    vec3 radiance = light.diffuse * attenuation;

    // cook-torrance brdf -------------------------------
    // Fresnel
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = Fresnel_Schlick(max(dot(H, V), 0.0), F0); // H,V replace N,L
    // NDF
    float NDF = NDF_GGX(N, H, roughness);
    // Geometry value 
    float G = G_Smith(NdotV, NdotL, roughness);

    vec3 nominator = NDF * G * F;
    float denom = 4.0 * NdotV * NdotL + 0.001;
    vec3 specular = nominator / denom;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

// Normal Distribution Function
float NDF_GGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness; // Disney
	float a2 = a*a;
	float NDotH2 = pow(max(dot(N, H), 0.0), 2);

	return a2 / (PI * pow(NDotH2 * (a2 - 1.0) + 1.0, 2));
}

// Geometry Function
// k is remapping of a based on whether we're using the geometry function for either direct lighting or IBL lighting:
//	k_direct = (a+1)^2 / 8
//	k_IBL = a^2 / 2;
float G_SchlickGGX(float NDotV, float roughness)
{
    float k = pow((roughness + 1), 2) / 8.0;
	return NDotV / (NDotV * (1-k) + k);
}
float G_Smith(float NdotV, float NdotL, float roughness)
{
	float ggx1  = G_SchlickGGX(NdotV, roughness);
	float ggx2  = G_SchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Fresnel Function
vec3 Fresnel_Schlick(float NdotL, vec3 F0)
{
	return F0 + (1 - F0) * pow(1.0 - NdotL, 5);
}