#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "Color.h"
#include "Shader.h"
using namespace glm;

enum class LightType
{
	Directional,
	Point
};

// 光源类的抽象基类
class Light
{
public:
	Light();
	virtual ~Light() {}

	void setColor(vec3 color);
	vec3 getColor() { return lightColor; }
	vec3 getAmbient() { return ambientStrength; }
	vec3 getDiffuse() { return diffuseStrength; }
	vec3 getSpecular() { return specularStrength; }
	LightType getType() { return type; }
	virtual void setShaderAttr(std::shared_ptr<Shader> shader, int lightNum) = 0; // 设置shader所需的变量

protected:
	vec3 lightColor;

	vec3 ambientStrength;
	vec3 diffuseStrength;
	vec3 specularStrength;

	void updateLightStrength(); 

	LightType type;
};

// 点光源
class PointLight : public Light
{
public:
	PointLight();
	PointLight(vec3 pos, vec3 color = vec3(1.0f));

	void setPosition(vec3 pos);
	vec3 getPos() { return position; }
	void setAttenuation(float constant_, float linear_, float quadratic_);
	virtual void setShaderAttr(std::shared_ptr<Shader> shader, int lightNum) override;

	// attenuation coefficient
	float constant;
	float linear;
	float quadratic;
private:
	vec3 position;

};

// 定向光
class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(vec3 dir, vec3 color = vec3(1.0f));

	void setDir(vec3 dir);
	vec3 getDir() { return direction; }
	virtual void setShaderAttr(std::shared_ptr<Shader> shader, int lightNum) override;

private:
	vec3 direction;
};
