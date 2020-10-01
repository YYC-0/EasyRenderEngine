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
	mat4 getTransMat() { return transformMat; }
	void setPosition(glm::vec3 pos);

protected:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	vec3 position; // mesh middle position
	vec3 scale;
	mat4 rotation;
	mat4 transformMat;

	int vertexNum;
	int faceNum;
	vector<unsigned int> indices;
	vector<glm::vec3> vertices; // vertex positions
	vector<glm::vec3> normals;
	Material material;

	vector<float> transformToInterleavedData();
	void bind();
};

class Cube : public Mesh
{
public:
	Cube(float length_ = 1.0f, float width_ = 1.0f, float height_ = 1.0f, vec3 pos = { 0,0,0 });
	void init(); // bind data, should be called in your constructor

private:
	float length;
	float width;
	float height;

	void create();
};