#pragma once
#include <glm/glm.hpp>
#include <string>
using namespace glm;
using namespace std;

enum class TextureType
{
	Diffuse,
	Normal,
	Specular,

	// PBR
	Albedo,
	Metallic,
	Roughness,
	Ao,
	Irradiance,

	Else
};

class Texture
{
public:
	Texture() = default;
	Texture(unsigned int id_) :
		id(id_) {}
	bool load(string imgPath, TextureType type_);
	unsigned int getID() const { return id; }

private:
	unsigned int id;
};

class Material
{
public:
	Material();
	Material(vec3 ambient_, vec3 diffuse_, vec3 specular_, float shininess_) :
		ambient(ambient_), diffuse(diffuse_), specular(specular_), shininess(shininess_),
		useDiffuseMap(false), useNormalMap(false), useSpecularMap(false), usePBR(false) {}

	void setTexture(Texture texture, TextureType type);
	virtual void loadTexture(std::string path, TextureType type);
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

	virtual void loadTexture(std::string path, TextureType type);
	void loadTextures(std::string fileFolderPath);
	void setIrradianceMap(Texture T);
	void setPrefilterMap(Texture T) { prefilterMap = T; }
	void setBrdfLUT(Texture T) { brdfLUT = T; }
	virtual void init() override;


	vec3 albedo;		// RBG
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