#include "../include/Shader.h"
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <direct.h>
#include  <stdio.h>  


shared_ptr<Shader> Shader::phong()
{
	shared_ptr<Shader> shader = make_shared<Shader>("./shaders/phong.vert", "./shaders/phong.frag");
	return shader;
}

std::shared_ptr<Shader> Shader::pbr()
{
	shared_ptr<Shader> shader = make_shared<Shader>("./shaders/phong.vert", "./shaders/pbr.frag");
	return shader;
}

std::shared_ptr<Shader> Shader::light()
{
	return make_shared<Shader>("./shaders/light.vert", "./shaders/light.frag");
}

Shader::Shader(string vertexPath_, string fragmentPath_, string geometryPath_)
{
	vertexPath = vertexPath_;
	fragmentPath = fragmentPath_;
	geometryPath = geometryPath_;
	setAttrI("shadowMap", 0);
	setAttrI("cubeDepthMap", 1);
	compile();
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

void Shader::setAttrMat3(const std::string & name, const glm::mat3 & mat)
{
	attributesMat3[name] = mat;
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

// set material attributes 
void Shader::setMeterial(shared_ptr<Material> mtl)
{
	if (mtl->usePBR)
	{
		shared_ptr<PBRMaterial> pbrMtl = dynamic_pointer_cast<PBRMaterial>(mtl);
		setPBRMeterial(pbrMtl);
		return;
	}

	setAttrVec3("mtl.ambient", mtl->ambient);
	setAttrVec3("mtl.diffuse", mtl->diffuse);
	setAttrVec3("mtl.specular", mtl->specular);
	setAttrF("shininess", mtl->shininess);
	setAttrB("useDiffuseMap", mtl->useDiffuseMap);
	setAttrB("useSpecularMap", mtl->useSpecularMap);
	setAttrB("useNormalMap", mtl->useNormalMap);


	// textures
	if (mtl->useDiffuseMap)
	{
		setAttrI("mtl.diffuseT", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mtl->diffuseMap.getID());
	}
	if (mtl->useNormalMap)
	{
		setAttrI("mtl.normalT", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mtl->normalMap.getID());
	}
	if (mtl->useSpecularMap)
	{
		setAttrI("mtl.specularT", 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mtl->specularMap.getID());
	}
}

void Shader::setPBRMeterial(shared_ptr<PBRMaterial> mtl)
{
	vec3 albedo;		// RBG
	float metallic;		// [0, 1]
	float roughness;	// [0, 1]
	float ao;

	setAttrB("useAlbedoMap", mtl->useAlbedoMap);
	setAttrB("useNormalMap", mtl->useNormalMap);
	setAttrB("useMatallicMap", mtl->useMetallicMap);
	setAttrB("useRoughnessMap", mtl->useRoughnessMap);
	setAttrB("useAoMap", mtl->useAoMap);
	setAttrB("useIrradianceMap", mtl->useIrradianceMap);

	// textures
	if (mtl->useAlbedoMap)
	{
		setAttrI("mtl.albedoT", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mtl->albedoMap.getID());
	}
	else
		setAttrVec3("mtl.albedo", mtl->albedo);
	if (mtl->useNormalMap)
	{
		setAttrI("mtl.normalT", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mtl->normalMap.getID());
	}
	if (mtl->useMetallicMap)
	{
		setAttrI("mtl.metallicT", 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mtl->metallicMap.getID());
	}
	else
		setAttrF("mtl.metallic", mtl->metallic);
	if (mtl->useRoughnessMap)
	{
		setAttrI("mtl.roughnessT", 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, mtl->roughnessMap.getID());
	}
	else
		setAttrF("mtl.roughness", mtl->roughness);
	if (mtl->useAoMap)
	{
		setAttrI("mtl.aoT", 6);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, mtl->aoMap.getID());
	}
	else
		setAttrF("mtl.ao", mtl->ao);
	if (mtl->useIrradianceMap)
	{
		setAttrI("mtl.irradianceMap", 7);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mtl->irradianceMap.getID());
	}

}

void Shader::compile()
{
	// 1. 从文件路径中获取顶点/片段着色器
	string vertexCode;
	string fragmentCode;
	string geometryCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	ifstream gShaderFile;

	// 保证ifstream对象可以抛出异常：
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath.c_str());
		fShaderFile.open(fragmentPath.c_str());
		bool success = true;
		if (!vShaderFile.good())
		{
			cout << "Shader file " << vertexPath << " not exist!" << endl;
			success = false;
		}
		if (!fShaderFile.good())
		{
			cout << "Shader file " << fragmentPath << " not exist!" << endl;
			success = false;
		}
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
		// 几何着色器
		if (geometryPath != "")
		{
			gShaderFile.open(geometryPath);
			if (!gShaderFile.good())
			{
				cout << "Shader file " << geometryPath << " not exist!" << endl;
				success = false;
			}
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
		if (!success)
			return;
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

	// 几何着色器
	unsigned int geometry;
	if (geometryPath != "")
	{
		const char * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
	}

	// 着色器程序
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != "")
		glAttachShader(ID, geometry);
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
	if (geometryPath != "")
		glDeleteShader(geometry);
}

void Shader::use()
{
	glUseProgram(ID);
}

// set shader's attributes
void Shader::setAttributes()
{
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
	for (auto attrMat3 : attributesMat3)
		setMat4(attrMat3.first, attrMat3.second);
	for (auto attrMat4 : attributesMat4)
		setMat4(attrMat4.first, attrMat4.second);
}

// clear all attributes
void Shader::clear()
{
	attributesBool.clear();
	attributesInt.clear();
	attributesFloat.clear();
	attributesVec2.clear();
	attributesVec3.clear();
	attributesVec4.clear();
	attributesMat4.clear();
	attributesMat3.clear();
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

void Shader::setMat3(const std::string & name, glm::mat3 mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
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
