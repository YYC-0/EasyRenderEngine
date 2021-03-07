#include "../include/Material.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

bool Texture::load(string imgPath, TextureType type_)
{
	glGenTextures(1, &id);
	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(imgPath.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		return true;
	}
	else
	{
		cout << "Texture \"" << imgPath << "\" failed to load!" << endl;
		return false;
	}
}

Material::Material()
{
	init();
}

void Material::setTexture(Texture texture, TextureType type)
{
	switch (type)
	{
	case TextureType::Diffuse:
		diffuseMap = texture;
		useDiffuseMap = true;
		break;
	case TextureType::Normal:
		normalMap = texture;
		useNormalMap = true;
		break;
	case TextureType::Specular:
		specularMap = texture;
		useSpecularMap = true;
		break;
	default:
		break;
	}
}

void Material::loadTexture(std::string path, TextureType type)
{
	Texture t;
	t.load(path, type);
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

void PBRMaterial::loadTexture(std::string path, TextureType type)
{
	switch (type)
	{
	case TextureType::Albedo:
		albedoMap.load(path, TextureType::Albedo);
		useAlbedoMap = true;
		break;
	case TextureType::Normal:
		normalMap.load(path, TextureType::Normal);
		useNormalMap = true;
		break;
	case TextureType::Metallic:
		metallicMap.load(path, TextureType::Metallic);
		useMetallicMap = true;
		break;
	case TextureType::Roughness:
		roughnessMap.load(path, TextureType::Roughness);
		useRoughnessMap = true;
		break;
	case TextureType::Ao:
		aoMap.load(path, TextureType::Ao);
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
	loadTexture(fileFolderPath + "/albedo.png", TextureType::Albedo);
	loadTexture(fileFolderPath + "/ao.png", TextureType::Ao);
	loadTexture(fileFolderPath + "/metallic.png", TextureType::Metallic);
	loadTexture(fileFolderPath + "/normal.png", TextureType::Normal);
	loadTexture(fileFolderPath + "/roughness.png", TextureType::Roughness);
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
