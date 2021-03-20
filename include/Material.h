#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Texture.h"
using namespace glm;
using namespace std;

enum class MaterialMapType
{
	Diffuse,
	Normal,
	Specular,

	// PBR
	Albedo,
	Metallic,
	Roughness,
	Ao,

	Else
};


class Material
{
public:
	Material();
	Material(vec3 ambient_, vec3 diffuse_, vec3 specular_, float shininess_) :
		ambient(ambient_), diffuse(diffuse_), specular(specular_), shininess(shininess_),
		useDiffuseMap(false), useNormalMap(false), useSpecularMap(false), usePBR(false) {}

	void setTexture(Texture texture, MaterialMapType type);
	virtual void loadTexture(std::string path, MaterialMapType type);
	virtual void init();

	bool useDiffuseMap;
	bool useNormalMap;
	bool useSpecularMap;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

	// textures
	Texture diffuseMap;
	Texture normalMap;
	Texture specularMap;

	bool usePBR;
};

class PBRMaterial : public Material
{
public:
	PBRMaterial() { init(); };
	PBRMaterial(vec3 albedo_, float metallic_, float roughness_, float ao_ = 1.0f) :
		albedo(albedo_), metallic(metallic_), roughness(roughness_), ao(ao_),
		useAlbedoMap(false), useNormalMap(false), useMetallicMap(false), 
		useRoughnessMap(false), useAoMap(false)
	{
		usePBR = true;
	}

	virtual void loadTexture(std::string path, MaterialMapType type);
	void loadTextures(std::string fileFolderPath);
	void setIrradianceMap(Texture T);
	void setPrefilterMap(Texture T) { prefilterMap = T; }
	void setBrdfLUT(Texture T) { brdfLUT = T; }
	virtual void init() override;


	vec3 albedo;		// RGB
	float metallic;		// [0, 1]
	float roughness;	// (0, 1]
	float ao;

	bool useAlbedoMap;
	bool useNormalMap;
	bool useMetallicMap;
	bool useRoughnessMap;
	bool useAoMap;

	Texture albedoMap;
	Texture normalMap;
	Texture metallicMap;
	Texture roughnessMap;
	Texture aoMap;
	Texture irradianceMap;
	Texture prefilterMap;
	Texture brdfLUT;
};