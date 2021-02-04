#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
using namespace std;

class CubeMap
{
public:
	CubeMap();
	CubeMap(const vector<string>& facesPath);
	//~CubeMap();

	void load(const vector<string>& facesPath);
	void loadHdr(const string &path);
	void drawAsSkybox(const glm::mat4& view, const glm::mat4& projection);
	void setHDR(bool b) { isHDR = b; }

	Cube box;
private:
	void init();

	bool isHDR;
	GLuint cubeMapID;
	unsigned int hdrTexture;
	shared_ptr<Shader> skyboxShader;
	shared_ptr<Shader> equirectangularToCubemapShader;
};
