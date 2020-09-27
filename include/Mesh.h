#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Material.h"
#include "Shader.h"
using namespace std;

class Mesh
{
public:
	Mesh();
	~Mesh();

	void createCube();
	void setMaterial(Material m);
	void draw(shared_ptr<Shader> shader);
	unsigned int getVAO() { return VAO; }

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	int vertexNum;
	int faceNum;
	vector<unsigned int> indices;
	vector<glm::vec3> positions; // vertex positions
	vector<glm::vec3> normals;
	Material material;

	vector<float> transformToInterleavedData();
	void bind();
};
