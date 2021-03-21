#include "../include/Material.h"
#include <iostream>

Material::Material(vec3 ambient_, vec3 diffuse_, vec3 specular_, float shininess_) :
	ambient(ambient_), 
	diffuse(diffuse_), 
	specular(specular_), 
	shininess(shininess_),
	useDiffuseMap(false), 
	useNormalMap(false),
	useSpecularMap(false), 
	usePBR(false),
	diffuseMap(nullptr),
	normalMap(nullptr),
	specularMap(nullptr)
{
}

Material::Material() : 
	diffuseMap(nullptr),
	normalMap(nullptr),
	specularMap(nullptr)
{
	init();
}	


void Material::setTexture(shared_ptr<Texture> texture, MaterialMapType type)
{
	switch (type)
	{
	case MaterialMapType::Diffuse:
		diffuseMap = texture;
		useDiffuseMap = true;
		break;
	case MaterialMapType::Normal:
		normalMap = texture;
		useNormalMap = true;
		break;
	case MaterialMapType::Specular:
		specularMap = texture;
		useSpecularMap = true;
		break;
	default:
		break;
	}
}

void Material::loadTexture(std::string path, MaterialMapType type)
{
	shared_ptr<Texture> t = make_shared<Texture>();
	t->load(path);
	setTexture(t, type);
}

void Material::init()
{
	ambient = vec3(1.0);
	diffuse = vec3(1.0);
	specular = vec3(0.5);
	shininess = 16.0;
	useDiffuseMap = false;
	useNormalMap = false;
	useSpecularMap = false;
	usePBR = false;
}

PBRMaterial::PBRMaterial() :
	albedoMap(nullptr),
	normalMap(nullptr),
	metallicMap(nullptr),
	roughnessMap(nullptr),
	aoMap(nullptr),
	irradianceMap(nullptr),
	prefilterMap(nullptr),
	brdfLUT(nullptr)
{
	init();
}

PBRMaterial::PBRMaterial(vec3 albedo_, float metallic_, float roughness_, float ao_) :
	albedo(albedo_), 
	metallic(metallic_), 
	roughness(roughness_), 
	ao(ao_),
	useAlbedoMap(false), 
	useNormalMap(false), 
	useMetallicMap(false),
	useRoughnessMap(false), 
	useAoMap(false),
	albedoMap(nullptr),
	normalMap(nullptr),
	metallicMap(nullptr),
	roughnessMap(nullptr),
	aoMap(nullptr),
	irradianceMap(nullptr),
	prefilterMap(nullptr),
	brdfLUT(nullptr)
{
	usePBR = true;
}

void PBRMaterial::loadTexture(std::string path, MaterialMapType type)
{
	switch (type)
	{
	case MaterialMapType::Albedo:
		if (!albedoMap)
			albedoMap = make_shared<Texture>();
		albedoMap->load(path);
		useAlbedoMap = true;
		break;
	case MaterialMapType::Normal:
		if (!normalMap)
			normalMap = make_shared<Texture>();
		normalMap->load(path);
		useNormalMap = true;
		break;
	case MaterialMapType::Metallic:
		if (!metallicMap)
			metallicMap = make_shared<Texture>();
		metallicMap->load(path);
		useMetallicMap = true;
		break;
	case MaterialMapType::Roughness:
		if (!roughnessMap)
			roughnessMap = make_shared<Texture>();
		roughnessMap->load(path);
		useRoughnessMap = true;
		break;
	case MaterialMapType::Ao:
		if (!aoMap)
			aoMap = make_shared<Texture>();
		aoMap->load(path);
		useAoMap = true;
		break;
	default:
		break;
	}
}

// input a file folder path
// load file albedo.png, ao.png, metallic.png, normal.png, roughness.png
void PBRMaterial::loadTextures(std::string fileFolderPath)
{
	loadTexture(fileFolderPath + "/albedo.png", MaterialMapType::Albedo);
	loadTexture(fileFolderPath + "/ao.png", MaterialMapType::Ao);
	loadTexture(fileFolderPath + "/metallic.png", MaterialMapType::Metallic);
	loadTexture(fileFolderPath + "/normal.png", MaterialMapType::Normal);
	loadTexture(fileFolderPath + "/roughness.png", MaterialMapType::Roughness);
}

void PBRMaterial::init()
{
	albedo = vec3(1.0);	// RBG
	metallic = 0.0f;	// [0, 1]
	roughness = 1.0f;	// [0, 1]
	ao = 1.0f;

	usePBR = true;
	useAlbedoMap = false;
	useNormalMap = false;
	useMetallicMap = false;
	useRoughnessMap = false;
	useAoMap = false;
}
