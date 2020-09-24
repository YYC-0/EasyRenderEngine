#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
using namespace std;

class Mesh
{
public:
	Mesh();
	~Mesh();

	void createCube();
	void draw();
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

	vector<float> transformToInterleavedData();
	void bind();
};
