#pragma once
#include <glm/glm.hpp>
#include "Color.h"
using namespace glm;

class Light
{
public:
	Light();
	Light(vec3 pos, vec3 color = vec3(1.0f));
	void setColor(vec3 color);
	void setPosition(vec3 pos);
	vec3 getColor() { return lightColor; }
	vec3 getPos() { return position; }
	vec3 getAmbient() { return ambientStrength; }
	vec3 getDiffuse() { return diffuseStrength; }
	vec3 getSpecular() { return specularStrength; }

private:
	vec3 position;
	vec3 lightColor;

	vec3 ambientStrength;
	vec3 diffuseStrength;
	vec3 specularStrength;

	void updateLightStrength();
};
