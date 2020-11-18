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

class Skybox
{
public:
	Skybox();
	Skybox(const vector<string>& facesPath);
	//~Skybox();

	void load(const vector<string>& facesPath);
	void draw(const glm::mat4& view, const glm::mat4& projection);

	Cube box;
private:
	void init();

	GLuint textureID;
	shared_ptr<Shader> skyboxShader;
};
