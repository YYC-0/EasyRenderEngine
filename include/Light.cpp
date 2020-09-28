#include "Light.h"

Light::Light()
{
	lightColor = vec3(1.0f, 1.0f, 1.0f);
	position = vec3(0.0f, 0.0f, 0.0f);
	updateLightStrength();
}

Light::Light(vec3 pos, vec3 color)
{
	lightColor = color;
	position = pos;
	updateLightStrength();
}

void Light::setColor(vec3 color)
{
	lightColor = color;
	updateLightStrength();
}

void Light::setPosition(vec3 pos)
{
	position = pos;
}

void Light::updateLightStrength()
{
	diffuseStrength = lightColor * vec3(0.5f);
	ambientStrength = diffuseStrength * vec3(0.2f);
	specularStrength = vec3(1.0f, 1.0f, 1.0f);
}
