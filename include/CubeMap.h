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
	void loadHdr(const string &path, int resolution = 512);
	void drawAsSkybox(const glm::mat4& view, const glm::mat4& projection);
	void setGammaCorrection(bool b) { gammaCorrection = b; }
	void generateIrradianceMap();

	unsigned int getCubeMapID() { return cubeMapID; }
	unsigned int getIrradianceMapID() { return irradianceMapID; }

	Cube box;
private:
	void init();

	bool gammaCorrection;
	unsigned int cubeMapID;
	unsigned int irradianceMapID;
	unsigned int hdrTexture;
	shared_ptr<Shader> skyboxShader;

	glm::mat4 captureProjection;
	vector<glm::mat4> captureViews;
};
