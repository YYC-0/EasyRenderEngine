#include "../include/Material.h"
#include <iostream>

Material::Material()
{
	init();
}

void Material::setTexture(Texture texture, MaterialMapType type)
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
	Texture t;
	t.load(path);
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

void PBRMaterial::loadTexture(std::string path, MaterialMapType type)
{
	switch (type)
	{
	case MaterialMapType::Albedo:
		albedoMap.load(path);
		useAlbedoMap = true;
		break;
	case MaterialMapType::Normal:
		normalMap.load(path);
		useNormalMap = true;
		break;
	case MaterialMapType::Metallic:
		metallicMap.load(path);
		useMetallicMap = true;
		break;
	case MaterialMapType::Roughness:
		roughnessMap.load(path);
		useRoughnessMap = true;
		break;
	case MaterialMapType::Ao:
		aoMap.load(path);
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

void PBRMaterial::setIrradianceMap(Texture T)
{
	irradianceMap = T;
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
