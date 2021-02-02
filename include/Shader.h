#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include "Camera.h"
#include "Material.h"
using namespace std;

class Shader
{
public:
	static std::shared_ptr<Shader> phong();
	static std::shared_ptr<Shader> pbr();
	static std::shared_ptr<Shader> light();

	// 从文件路径中获取顶点/片段着色器 并编译
	Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath = "");

	void compile();
	void use();
	void setAttributes();

	void setAttrB(const std::string &name, bool value);
	void setAttrI(const std::string &name, int value);
	void setAttrF(const std::string &name, float value);
	void setAttrMat3(const std::string &name, const glm::mat3 &mat);
	void setAttrMat4(const std::string &name, const glm::mat4 &mat);
	void setAttrVec2(const std::string &name, const glm::vec2 &value);
	void setAttrVec3(const std::string &name, const glm::vec3 &value);
	void setAttrVec4(const std::string &name, const glm::vec4 &value);
	void setCamera(const Camera& camera);
	void setMeterial(shared_ptr<Material> mtl);
	void setPBRMeterial(shared_ptr<PBRMaterial> mtl);

private:
	unsigned int ID; // program ID

	std::string vertexPath;
	std::string fragmentPath;
	std::string geometryPath;
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMat3(const std::string &name, glm::mat3 mat) const;
	void setMat4(const std::string &name, glm::mat4 mat) const;
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	void setVec4(const std::string &name, const glm::vec4 &value) const;

	std::map<std::string, bool> attributesBool;
	std::map<std::string, int> attributesInt;
	std::map<std::string, float> attributesFloat;
	std::map<std::string, glm::vec2> attributesVec2;
	std::map<std::string, glm::vec3> attributesVec3;
	std::map<std::string, glm::vec4> attributesVec4;
	std::map<std::string, glm::mat4> attributesMat4;
	std::map<std::string, glm::mat3> attributesMat3;
};
