#include "../include/Shader.h"
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

Shader::Shader(string vertexPath_, string fragmentPath_)
{
	vertexPath = vertexPath_;
	fragmentPath = fragmentPath_;
}

void Shader::setAttrB(const std::string & name, bool value)
{
	attributesBool[name] = value;
}

void Shader::setAttrI(const std::string & name, int value)
{
	attributesInt[name] = value;
}

void Shader::setAttrF(const std::string & name, float value)
{
	attributesFloat[name] = value;
}

void Shader::setAttrMat4(const std::string & name, const glm::mat4 & mat)
{
	attributesMat4[name] = mat;
}

void Shader::setAttrVec2(const std::string & name, const glm::vec2 & value)
{
	attributesVec2[name] = value;
}

void Shader::setAttrVec3(const std::string & name, const glm::vec3 & value)
{
	attributesVec3[name] = value;
}

void Shader::setAttrVec4(const std::string & name, const glm::vec4 & value)
{
	attributesVec4[name] = value;
}

void Shader::setCamera(const Camera &camera)
{
	glm::mat4 projection = camera.getProjectionMatrix();
	glm::mat4 view = camera.getViewMatrix();
	setAttrVec3("viewPos", camera.position);
	setAttrMat4("projection", projection);
	setAttrMat4("view", view);
}

void Shader::compile()
{
	// 1. 从文件路径中获取顶点/片段着色器
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;

	// 保证ifstream对象可以抛出异常：
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath.c_str());
		fShaderFile.open(fragmentPath.c_str());
		// 读取文件的缓冲内容到数据流中
		stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// 关闭文件处理器
		vShaderFile.close();
		fShaderFile.close();
		// 转换数据流到string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure a)
	{
		cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
	}

	// 2. 编译着色器
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];
	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();
	// 顶点着色器
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// 打印编译错误
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// 片段着色器
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// 着色器程序
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// 打印连接错误（如果有的话）
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// 删除着色器
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(ID);

	for (auto attrB : attributesBool)
		setBool(attrB.first, attrB.second);
	for (auto attrI : attributesInt)
		setInt(attrI.first, attrI.second);
	for (auto attrF : attributesFloat)
		setFloat(attrF.first, attrF.second);
	for (auto attrVec2 : attributesVec2)
		setVec2(attrVec2.first, attrVec2.second);
	for (auto attrVec3 : attributesVec3)
		setVec3(attrVec3.first, attrVec3.second);
	for (auto attrVec4 : attributesVec4)
		setVec4(attrVec4.first, attrVec4.second);
	for (auto attrMat4 : attributesMat4)
		setMat4(attrMat4.first, attrMat4.second);
}

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setVec2(const std::string & name, const glm::vec2 & value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string & name, const glm::vec4 & value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
