#include "Light.h"

Light::Light()
{
	lightColor = vec3(1.0f, 1.0f, 1.0f);
	updateLightStrength();
}

void Light::setColor(vec3 color)
{
	lightColor = color;
	updateLightStrength();
}


void Light::updateLightStrength()
{
	diffuseStrength = lightColor * vec3(0.5f);
	ambientStrength = diffuseStrength * vec3(0.2f);
	specularStrength = vec3(1.0f, 1.0f, 1.0f);
}

PointLight::PointLight()
{
	position = vec3(0.0f, 0.0f, 0.0f);
	updateLightStrength();
	type = LightType::Point;
}

PointLight::PointLight(vec3 pos, vec3 color)
{
	lightColor = color;
	position = pos;
	updateLightStrength();
	type = LightType::Point;
}

void PointLight::setPosition(vec3 pos)
{
	position = pos;
}

void PointLight::setShaderAttr(std::shared_ptr<Shader> shader, int lightNum)
{
	std::string pre = "lights[" + std::to_string(lightNum) + "].";
	shader->setAttrI(pre + "type", 0);
	shader->setAttrVec3(pre + "position", position);
	shader->setAttrVec3(pre + "ambient", ambientStrength);
	shader->setAttrVec3(pre + "diffuse", diffuseStrength);
	shader->setAttrVec3(pre + "specular", specularStrength);
}

DirectionalLight::DirectionalLight()
{
	direction = vec3(0.0, -1.0, 0.0);
	updateLightStrength();
	type = LightType::Directional;
}

DirectionalLight::DirectionalLight(vec3 dir, vec3 color)
{
	lightColor = color;
	direction = dir;
	updateLightStrength();
	type = LightType::Directional;
}

void DirectionalLight::setDir(vec3 dir)
{
	direction = dir;
}

void DirectionalLight::setShaderAttr(std::shared_ptr<Shader> shader, int lightNum)
{
	std::string pre = "lights[" + std::to_string(lightNum) + "].";
	shader->setAttrI(pre + "type", 1);
	shader->setAttrVec3(pre + "direction", direction);
	shader->setAttrVec3(pre + "ambient", ambientStrength);
	shader->setAttrVec3(pre + "diffuse", diffuseStrength);
	shader->setAttrVec3(pre + "specular", specularStrength);
}
