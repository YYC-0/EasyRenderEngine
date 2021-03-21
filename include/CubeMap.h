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
	void preComputeMaps(); // generate irradianceMap, prefilterMap, brdfLUTTexture for IBL

	shared_ptr<Texture> getCubeMap() { return cubeMap; }
	shared_ptr<Texture> getIrradianceMap() { return irradianceMap; }
	shared_ptr<Texture> getPrefilterMapID() { return prefilterMap; }
	shared_ptr<Texture> getBrdfLUTTextureID() { return brdfLUTTexture; }

private:
	void init();
	void generateIrradianceMap();
	void generatePrefilterMap();
	void generateBrdfLUTTexture();

	Cube box;

	bool gammaCorrection;
	shared_ptr<Texture> cubeMap;

	shared_ptr<Texture> irradianceMap;
	shared_ptr<Texture> prefilterMap; // used for specular IBL
	shared_ptr<Texture> brdfLUTTexture;

	shared_ptr<Shader> skyboxShader;

	glm::mat4 captureProjection;
	vector<glm::mat4> captureViews;
};
