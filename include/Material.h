#pragma once
#include <glm/glm.hpp>
#include <string>
#include <memory>
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
	Material(vec3 ambient_, vec3 diffuse_, vec3 specular_, float shininess_);

	void setTexture(shared_ptr<Texture> texture, MaterialMapType type);
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
	shared_ptr<Texture> diffuseMap;
	shared_ptr<Texture> normalMap;
	shared_ptr<Texture> specularMap;

	bool usePBR;
};

class PBRMaterial : public Material
{
public:
	PBRMaterial();
	PBRMaterial(vec3 albedo_, float metallic_, float roughness_, float ao_ = 1.0f);

	virtual void loadTexture(std::string path, MaterialMapType type);
	void loadTextures(std::string fileFolderPath);
	void setIrradianceMap(shared_ptr<Texture> T) { irradianceMap = T; }
	void setPrefilterMap(shared_ptr<Texture> T) { prefilterMap = T; }
	void setBrdfLUT(shared_ptr<Texture> T) { brdfLUT = T; }
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

	shared_ptr<Texture> albedoMap;
	shared_ptr<Texture> normalMap;
	shared_ptr<Texture> metallicMap;
	shared_ptr<Texture> roughnessMap;
	shared_ptr<Texture> aoMap;
	shared_ptr<Texture> irradianceMap;
	shared_ptr<Texture> prefilterMap;
	shared_ptr<Texture> brdfLUT;
};