#include "../include/Material.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

bool Texture::load(string imgPath, TextureType type_)
{
	type = type_;
	path = imgPath;

	glGenTextures(1, &id);

	int width, height, nrComponents;
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

Material::Material() :
	ambient(vec3(1.0)),
	diffuse(vec3(1.0)),
	specular(vec3(0.5)),
	shininess(32.0),
	useDiffuseMap(false), 
	useNormalMap(false), 
	useSpecularMap(false)
{
}

void Material::loadTexture(std::string path, TextureType type)
{
	switch (type)
	{
	case TextureType::Diffuse:
		diffuseMap.load(path, TextureType::Diffuse);
		useDiffuseMap = true;
		break;
	case TextureType::Normal:
		normalMap.load(path, TextureType::Normal);
		useNormalMap = true;
		break;
	case TextureType::Specular:
		specularMap.load(path, TextureType::Specular);
		useSpecularMap = true;
		break;
	default:
		break;
	}
}

void Material::init()
{
	ambient = vec3(1.0);
	diffuse = vec3(1.0);
	specular = vec3(0.5);
	shininess = 32.0;
	useDiffuseMap = false;
	useNormalMap = false;
	useSpecularMap = false;
}
