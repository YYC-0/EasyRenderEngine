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

struct VertexIndex
{
	int posIdx; // position index
	int texIdx; // texture index
	int nIdx;	// normal index
};
typedef vector<VertexIndex> Face;
class Object
{
public:
	Object();
	~Object();

	void setMaterial(Material m);
	virtual void draw(shared_ptr<Shader> shader);

	void setPosition(glm::vec3 pos);
	void setScale(glm::vec3 scale_);

	unsigned int getVAO() { return VAO; }
	mat4 getTransMat() { return transformMat; }
	glm::vec3 getPosition() { return position; }
	glm::vec3 getScale() { return scale; }

	void bind();

protected:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	vec3 position; // mesh middle position
	vec3 scale;
	mat4 rotation; // ��δ����
	mat4 transformMat;

	int vertexNum;
	int faceNum;
	vector<unsigned int> indices;
	vector<vector<Face>> facesGroupsIdx; // faces groups indices
	vector<glm::vec3> vertices; // vertex positions
	vector<glm::vec3> normals;
	vector<glm::vec2> texCoords;
	Material material;

	vector<float> transformToInterleavedData();
};

class Cube : public Object
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

class Model : public Object
{
public:
	Model(vec3 position_ = { 0,0,0 }, vec3 scale_ = { 1.0,1.0,1.0 });

	void loadObj(string path);	// load obj model

	virtual void draw(shared_ptr<Shader> shader);

private:
	
};