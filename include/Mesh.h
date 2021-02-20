#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include <map>
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
	virtual ~Object() = 0;

	void setMaterial(shared_ptr<Material> mtl);
	virtual void draw(shared_ptr<Shader> shader);

	void setTransform(const vec3& pos_, const vec3& scale_, const vector<float>& rotation_);
	void setPosition(const vec3& pos_);
	void setScale(const vec3& scale_);
	void setRotation(const vector<float>& rotation_);
	void setRotateX(float degree);
	void setRotateY(float degree);
	void setRotateZ(float degree);

	mat4 getTransMat() { return modelMatrix; }
	glm::vec3 getPosition() { return position; }
	glm::vec3 getScale() { return scale; }
	vector<float> getRotation() { return rotateAngle; }

	void bind();

protected:
	unsigned int VBO;
	vector<unsigned int> VAOs;
	vector<unsigned int> EBOs;

	vec3 position; // mesh middle position
	vec3 scale;
	vector<float> rotateAngle; // rotate angles of three axis
	mat4 modelMatrix;
	mat4 transInvModelMatrix; // transpose(inverse(modelMatrix))

	int drawVertexNum;
	int faceNum;
	vector<vector<unsigned int>> indices;
	vector<vector<Face>> facesGroupsIdx; // faces groups indices
	vector<glm::vec3> vertices; // vertex positions
	vector<glm::vec3> normals;
	vector<glm::vec2> texCoords;
	vector<glm::vec3> tangents;		// used for noraml map
	vector<glm::vec3> bitangents;	// used for normal map

	vector<shared_ptr<Material>> materials;

	vector<float> transformToInterleavedData();

	void updateModelMatrix(); // update model matrix according current position and scale
	pair<vec3, vec3> computeTB(const vec3& pos1, const vec3& pos2, const vec3& pos3,
		const vec2& uv1, const vec2& uv2, const vec2& uv3);	// compute tangent and bitangent
};

class Cube : public Object
{
public:
	Cube(float length_ = 1.0f, float width_ = 1.0f, float height_ = 1.0f, vec3 pos = { 0,0,0 });
	~Cube() {}
	//virtual void draw(shared_ptr<Shader> shader) override;

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
	Model(const string &path);
	~Model() {}

	void loadObj(const string &path);	// load obj model

	virtual void draw(shared_ptr<Shader> shader) override;

private:
	vector<string> materialName;
	map<string, shared_ptr<Material>> modelMaterials;
	void loadMaterialLib(string path);
	
};

class Sphere : public Object
{
public:
	Sphere(float r, int detailLevel_ = 1, vec3 pos = { 0,0,0 });
	~Sphere();

private:
	float radius;
	float detailLevel;
	int index;
	map<pair<int, int>, int> middlePointIndexCache;

	void create(int detail);
	vec2 computeUV(const vec3 &pos);
	int addVertex(vec3 p);
	int addMiddlePoint(int p1, int p2);
};

class Rectangle : public Object
{
public:
	Rectangle(float width_ = 1.0f, float height_ = 1.0f, vec3 pos = { 0,0,0 });
	~Rectangle() {}

private:
	float width;
	float height;

	void create();
};