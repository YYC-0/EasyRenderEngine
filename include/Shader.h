#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <map>

class Shader
{
public:
	unsigned int ID; // 程序ID

	// 从文件路径中获取顶点/片段着色器 并编译
	Shader(const GLchar *vertexPath, const GLchar *fragmentPath);

	void use();

	void setAttrB(const std::string &name, bool value);
	void setAttrI(const std::string &name, int value);
	void setAttrF(const std::string &name, float value);
	void setAttrMat4(const std::string &name, const glm::mat4 &mat);
	void setAttrVec2(const std::string &name, const glm::vec2 &value);
	void setAttrVec3(const std::string &name, const glm::vec3 &value);
	void setAttrVec4(const std::string &name, const glm::vec4 &value);

private:
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
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
};
