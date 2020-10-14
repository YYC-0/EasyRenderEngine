#pragma once
#include <glm/glm.hpp>
#include <string>
using namespace glm;
using namespace std;

enum class TextureType
{
	Diffuse,
	Normal,
	Specular
};

class Texture
{
public:
	Texture() = default;
	bool load(string imgPath, TextureType type_);
	unsigned int getID() { return id; }

private:
	unsigned int id;
	TextureType type;
	string path;
};

class Material
{
public:
	Material();
	Material(vec3 ambient_, vec3 diffuse_, vec3 specular_, float shininess_) :
		ambient(ambient_), diffuse(diffuse_), specular(specular_), shininess(shininess_),
		useDiffuseMap(false), useNormalMap(false), useSpecularMap(false) {}

	void loadTexture(std::string path, TextureType type);
	void init();

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
};
