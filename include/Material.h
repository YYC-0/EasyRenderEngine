#pragma once
#include <glm/glm.hpp>
using namespace glm;

class Material
{
public:
	Material() = default;
	Material(vec3 ambient_, vec3 diffuse_, vec3 specular_, float shininess_) :
		ambient(ambient_), diffuse(diffuse_), specular(specular_), shininess(shininess_) {}

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
