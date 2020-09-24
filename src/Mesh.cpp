#include "../include/Mesh.h"
using namespace glm;

Mesh::Mesh() :
	VAO(0),
	VBO(0),
	EBO(0),
	vertexNum(0),
	faceNum(0)
{
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::createCube()
{        
	//    v5----- v4
	//   /|      /|
	//  v1------v0|
	//  | |     | |
	//  | |v6---|-|v7
	//  |/      |/
	//  v2------v3

	vec3 v[] = {
		 vec3(0.5f, 0.5f, 0.5f),
		 vec3(-0.5f, 0.5f, 0.5f),
		 vec3(-0.5f, -0.5f, 0.5f),
		 vec3(0.5f, -0.5f, 0.5f),
		 vec3(0.5f, 0.5f, -0.5f),
		 vec3(-0.5f, 0.5f, -0.5f),
		 vec3(-0.5f, -0.5f, -0.5f),
		 vec3(0.5f, -0.5f, -0.5f)
	};

	 positions = vector<vec3>{
		v[0],v[1],v[2], v[3], // forward
		v[4],v[0],v[3], v[7], // right
		v[5],v[4],v[7], v[6], // back
		v[1],v[5],v[6], v[2], // left
		v[4],v[5],v[1], v[0], // up
		v[3],v[2],v[6], v[7], // down
	};

	 // 每个三角形顶点的索引
	 indices = vector<unsigned int>{
		 0,1,2, 0,2,3,
		 4,5,6, 4,6,7,
		 8,9,10, 8,10,11,
		 12,13,14, 12, 14,15,
		 16,17,18, 16,18,19,
		 20,21,22, 20,22,23
	 };

	 normals = vector<vec3>{
		 vec3{0, 0, 1}, vec3{0, 0, 1}, vec3{0, 0, 1}, vec3{0, 0, 1},
		 vec3{1, 0, 0}, vec3{1, 0, 0}, vec3{1, 0, 0}, vec3{1, 0, 0},
		 vec3{0, 0, -1},  vec3{0, 0, -1}, vec3{0, 0, -1}, vec3{0, 0, -1},
		 vec3{-1, 0, 0}, vec3{-1, 0, 0}, vec3{-1, 0, 0}, vec3{-1, 0, 0},
		 vec3{0, 1, 0}, vec3{0, 1, 0}, vec3{0, 1, 0}, vec3{0, 1, 0},
		 vec3{0, -1, 0},  vec3{0, -1, 0}, vec3{0, -1, 0}, vec3{0, -1, 0},
	 };
	 vertexNum = positions.size();
	 faceNum = indices.size() / 3;

	 bind();
}

void Mesh::draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

vector<float> Mesh::transformToInterleavedData()
{
	vector<float> interleavedData;
	for (int i = 0; i < vertexNum; ++i)
	{
		interleavedData.push_back(positions[i].x);
		interleavedData.push_back(positions[i].y);
		interleavedData.push_back(positions[i].z);
		interleavedData.push_back(normals[i].x);
		interleavedData.push_back(normals[i].y);
		interleavedData.push_back(normals[i].z);
	}
	return interleavedData;
}

void Mesh::bind()
{
	vector<float> interleavedData = transformToInterleavedData();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	int stride = interleavedData.size() / vertexNum * sizeof(float);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}
