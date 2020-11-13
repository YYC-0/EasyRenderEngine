#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../include/Mesh.h"
#include "../include/Utility.h"

using namespace glm;

Object::Object() :
	drawVertexNum(0),
	faceNum(0),
	position(vec3(0, 0, 0)),
	scale(vec3(1.0, 1.0, 1.0)),
	modelMatrix(mat4(1.0f)),
	transInvModelMatrix(mat4(1.0f))
{
	rotateAngle.resize(3, 0);
}

Object::~Object()
{
	glDeleteBuffers(1, &VBO);
	for (int i = 0; i < VAOs.size(); ++i)
		glDeleteVertexArrays(1, &VAOs[i]);
	for (int i = 0; i < EBOs.size(); ++i)
		glDeleteBuffers(1, &EBOs[i]);
}

void Object::setMaterial(Material m) // 待修改
{
	if (materials.empty())
		materials.push_back(m);
	else
		materials[0] = m;
}

void Object::draw(shared_ptr<Shader> shader)
{

}

void Object::setTransform(const vec3 & pos_, const vec3 & scale_, const vector<float>& rotation_)
{
	position = pos_;
	scale = scale_;
	rotateAngle = rotation_;
	updateModelMatrix();
}

void Object::setPosition(const vec3& pos)
{
	position = pos;
	updateModelMatrix();
}

void Object::setScale(const vec3& scale_)
{
	scale = scale_;
	updateModelMatrix();
}

void Object::setRotation(const vector<float>& rotation_)
{
	rotateAngle = rotation_;
	updateModelMatrix();
}

void Object::setRotateX(float degree)
{
	rotateAngle[0] = degree;
	updateModelMatrix();
}

void Object::setRotateY(float degree)
{
	rotateAngle[1] = degree;
	updateModelMatrix();
}

void Object::setRotateZ(float degree)
{
	rotateAngle[2] = degree;
	updateModelMatrix();
}

vector<float> Object::transformToInterleavedData()
{
	vector<float> interleavedData;
	for (int i = 0; i < vertices.size(); ++i)
	{
		// position
		interleavedData.push_back(vertices[i].x);
		interleavedData.push_back(vertices[i].y);
		interleavedData.push_back(vertices[i].z);
		// normal
		interleavedData.push_back(normals[i].x);
		interleavedData.push_back(normals[i].y);
		interleavedData.push_back(normals[i].z);
		// texture coord
		interleavedData.push_back(texCoords[i].x);
		interleavedData.push_back(texCoords[i].y);
		// tangent bitangent
		interleavedData.push_back(tangents[i].x);
		interleavedData.push_back(tangents[i].y);
		interleavedData.push_back(tangents[i].z);
		interleavedData.push_back(bitangents[i].x);
		interleavedData.push_back(bitangents[i].y);
		interleavedData.push_back(bitangents[i].z);
	}
	return interleavedData;
}

void Object::updateModelMatrix()
{
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::rotate(modelMatrix, radians(rotateAngle[0]), vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, radians(rotateAngle[1]), vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, radians(rotateAngle[2]), vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, scale);
	transInvModelMatrix = glm::transpose(glm::inverse(modelMatrix));
}

pair<vec3, vec3> Object::computeTB(const vec3 & pos1, const vec3 & pos2, const vec3 & pos3, 
	const vec2 & uv1, const vec2 & uv2, const vec2 & uv3)
{
	vec3 tangent, bitangent;
	vec3 edge1 = pos2 - pos1;
	vec3 edge2 = pos3 - pos1;
	vec2 deltaUV1 = uv2 - uv1;
	vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = glm::normalize(tangent);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent = glm::normalize(bitangent);

	return { tangent, bitangent };
}

void Object::bind()
{
	vector<float> interleavedData = transformToInterleavedData();

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);
	
	for (int i = 0; i < indices.size(); ++i)
	{
		unsigned int vao, ebo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned int), indices[i].data(), GL_STATIC_DRAW);

		int stride = interleavedData.size() / vertices.size() * sizeof(float);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void *)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void *)(11 * sizeof(float)));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);

		VAOs.push_back(vao);
		EBOs.push_back(ebo);
	}
}

// Cube ---------------------------------------------------------------------
Cube::Cube(float length_, float width_, float height_, vec3 pos) :
	length(length_), width(width_), height(height_)
{	
	position = pos;
	scale = vec3(length_, width_, height_);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, scale);

	create();
}


void Cube::draw(shared_ptr<Shader> shader)
{
	for (int i = 0; i < indices.size(); ++i)
	{
		shader->setAttrMat4("model", modelMatrix);
		shader->setAttrMat4("transInvModel", transInvModelMatrix);

		shader->setAttrVec3("material.ambient", materials[i].ambient);
		shader->setAttrVec3("material.diffuse", materials[i].diffuse);
		shader->setAttrVec3("material.specular", materials[i].specular);
		shader->setAttrF("shininess", materials[i].shininess);
		shader->setAttrB("useDiffuseMap", materials[i].useDiffuseMap);
		shader->setAttrB("useSpecularMap", materials[i].useSpecularMap);
		shader->setAttrB("useNormalMap", materials[i].useNormalMap);

		shader->setAttrI("textures.diffuse", 2);
		shader->setAttrI("textures.normal", 3);
		shader->setAttrI("textures.specular", 4);

		shader->use();


		// texture
		if (materials[i].useDiffuseMap)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, materials[i].diffuseMap.getID());
		}
		if (materials[i].useNormalMap)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, materials[i].normalMap.getID());
		}
		if (materials[i].useSpecularMap)
		{
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, materials[i].specularMap.getID());
		}

		glBindVertexArray(VAOs[i]);
		glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Cube::create()
{
	//    v5----- v4
	//   /|      /|
	//  v1------v0|
	//  | |     | |
	//  | |v6---|-|v7
	//  |/      |/
	//  v2------v3

	vector<vec3> v{
		 vec3(0.5f, 0.5f, 0.5f),
		 vec3(-0.5f, 0.5f, 0.5f),
		 vec3(-0.5f, -0.5f, 0.5f),
		 vec3(0.5f, -0.5f, 0.5f),
		 vec3(0.5f, 0.5f, -0.5f),
		 vec3(-0.5f, 0.5f, -0.5f),
		 vec3(-0.5f, -0.5f, -0.5f),
		 vec3(0.5f, -0.5f, -0.5f)
	};

	// vertex positions
	vertices = vector<vec3>{
	   v[0],v[1],v[2], v[3], // forward
	   v[4],v[0],v[3], v[7], // right
	   v[5],v[4],v[7], v[6], // back
	   v[1],v[5],v[6], v[2], // left
	   v[4],v[5],v[1], v[0], // up
	   v[3],v[2],v[6], v[7], // down
	};

	// 每个三角形顶点的索引
	indices.push_back(vector<unsigned int>{
		0,1,2, 0,2,3,
		4,5,6, 4,6,7,
		8,9,10, 8,10,11,
		12,13,14, 12, 14,15,
		16,17,18, 16,18,19,
		20,21,22, 20,22,23
	});

	normals = vector<vec3>{
		vec3{0, 0, 1}, vec3{0, 0, 1}, vec3{0, 0, 1}, vec3{0, 0, 1},
		vec3{1, 0, 0}, vec3{1, 0, 0}, vec3{1, 0, 0}, vec3{1, 0, 0},
		vec3{0, 0, -1},  vec3{0, 0, -1}, vec3{0, 0, -1}, vec3{0, 0, -1},
		vec3{-1, 0, 0}, vec3{-1, 0, 0}, vec3{-1, 0, 0}, vec3{-1, 0, 0},
		vec3{0, 1, 0}, vec3{0, 1, 0}, vec3{0, 1, 0}, vec3{0, 1, 0},
		vec3{0, -1, 0},  vec3{0, -1, 0}, vec3{0, -1, 0}, vec3{0, -1, 0},
	};

	texCoords = vector<vec2>{
		vec2{1,1}, vec2{0,1}, vec2{0,0},vec2{1,0},
		vec2{1,1}, vec2{0,1}, vec2{0,0},vec2{1,0},
		vec2{1,1}, vec2{0,1}, vec2{0,0},vec2{1,0},
		vec2{1,1}, vec2{0,1}, vec2{0,0},vec2{1,0},
		vec2{1,1}, vec2{0,1}, vec2{0,0},vec2{1,0},
		vec2{1,1}, vec2{0,1}, vec2{0,0},vec2{1,0}
	};

	faceNum = indices.size() / 3;

	// tangent bitangent
	for (int i = 0; i < indices[0].size(); i += 3)
	{
		pair<vec3, vec3> tan = computeTB(vertices[indices[0][i]], vertices[indices[0][i + 1]], vertices[indices[0][i + 2]],
			texCoords[indices[0][i]], texCoords[indices[0][i + 1]], texCoords[indices[0][i + 2]]);
		for (int j = 0; j < 2; ++j)
		{
			tangents.push_back(tan.first);
			bitangents.push_back(tan.second);
		}
	}
}

// Model ---------------------------------------------------------------------
Model::Model(vec3 position_, vec3 scale_) :
	Object()
{
	position = position_;
	scale = scale_;
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, scale);
}

Model::Model(const string &path)
{
	loadObj(path);
}

void Model::loadObj(const string &path)
{
	ifstream in;
	in.open(path, std::ifstream::in);
	if (in.fail())
	{
		cout << "Load file " << path << " fail" << endl;
		return;
	}

	vector<vec3> vertices_;
	vector<vec2> texCoords_;
	vector<vec3> normals_;
	vertices_.push_back(vec3()); // start from index 1
	texCoords_.push_back(vec2());
	normals_.push_back(vec3());
	vector<vector<Face>> facesGroupsIdx_;
	vector<Face> faces;
	string line;
	while (!in.eof())
	{
		getline(in, line);
		if (line.empty())
			continue;
		vector<string> tokens;
		Utility::split(line, tokens, " ");
		if (tokens[0] == "v") // vertices
		{
			vec3 v;
			for (int i = 1; i <= 3; ++i)
				v[i-1] = atof(tokens[i].c_str());
			vertices_.push_back(v);
		}
		else if (tokens[0] == "vt") // texture coord
		{
			vec2 texCoord;
			for (int i = 1; i <= 2; ++i)
				texCoord[i-1] = atof(tokens[i].c_str());
			texCoords_.push_back(texCoord);
		}
		else if (tokens[0] == "vn") // vertex normal
		{
			vec3 n;
			for (int i = 1; i <= 3; ++i)
				n[i-1] = atof(tokens[i].c_str());
			normals_.push_back(n);
		}
		else if (tokens[0] == "f") // face
		{
			Face f;
			for (int i = 1; i <=3 ; ++i)
			{
				VertexIndex vi;
				vector<string> indices;
				Utility::replace(tokens[i], "//", "/0/");
				Utility::split(tokens[i], indices, "/");
				assert(indices.size() == 3);
				vi.posIdx = atoi(indices[0].c_str());
				vi.texIdx = atoi(indices[1].c_str());
				vi.nIdx = atoi(indices[2].c_str());
				f.push_back(vi);
			}
			faces.push_back(f);
			// 四边形
			if (tokens.size() == 5)
			{
				Face f1;
				for (int i = 1; i <= 4; ++i)
				{
					if (i == 2)
						continue;
					VertexIndex vi;
					vector<string> indices;
					Utility::replace(tokens[i], "//", "/0/");
					Utility::split(tokens[i], indices, "/");
					assert(indices.size() == 3);
					vi.posIdx = atoi(indices[0].c_str());
					vi.texIdx = atoi(indices[1].c_str());
					vi.nIdx = atoi(indices[2].c_str());
					f1.push_back(vi);
				}
				faces.push_back(f1);
			}
		}
		else if (tokens[0] == "usemtl") // material
		{
			materialName.push_back(tokens[1]);
			if (!faces.empty())
			{
				facesGroupsIdx_.push_back(faces);
				faces.clear();
			}
		}
		else if (tokens[0] == "mtllib")
		{
			size_t pos = path.find_last_of("/");
			if (pos != string::npos)
			{
				string mtllibPath = path.substr(0, pos + 1) + tokens[1];
				loadMaterialLib(mtllibPath);
			}
		}
	}
	if (!faces.empty())
		facesGroupsIdx_.push_back(faces);

	// 转换格式
	vertices = vertices_;
	normals.resize(vertices.size());
	texCoords.resize(vertices.size()); 
	for (int i = 0; i < facesGroupsIdx_.size(); ++i)
	{
		vector<unsigned int> indices_;
		for (int j = 0; j < facesGroupsIdx_[i].size(); ++j)
		{
			Face f = facesGroupsIdx_[i][j];
			int idx[3];
			for (int k = 0; k < 3; ++k)
			{
				idx[k] = f[k].posIdx;
				indices_.push_back(f[k].posIdx);
				normals[f[k].posIdx] = normals_[f[k].nIdx];
				texCoords[f[k].posIdx] = texCoords_[f[k].texIdx];
			}
			faceNum++;

			pair<vec3, vec3> tan = computeTB(vertices[idx[0]], vertices[idx[1]], vertices[idx[2]],
				texCoords[idx[0]], texCoords[idx[1]], texCoords[idx[2]]);
			tangents.push_back(tan.first);
			bitangents.push_back(tan.second);
		}
		indices.push_back(indices_);
	}
}
 
void Model::draw(shared_ptr<Shader> shader)
{
	for (int i = 0; i < indices.size(); ++i)
	{
		Material mtl;
		if(!modelMaterials.empty())
			mtl = modelMaterials[materialName[i]];

		shader->setAttrMat4("model", modelMatrix);
		shader->setAttrMat4("transInvModel", transInvModelMatrix);

		shader->setAttrVec3("material.ambient", mtl.ambient);
		shader->setAttrVec3("material.diffuse", mtl.diffuse);
		shader->setAttrVec3("material.specular", mtl.specular);
		shader->setAttrF("shininess", mtl.shininess);
		shader->setAttrB("useDiffuseMap", mtl.useDiffuseMap);
		shader->setAttrB("useSpecularMap", mtl.useSpecularMap);
		shader->setAttrB("useNormalMap", mtl.useNormalMap);

		shader->setAttrI("textures.diffuse", 2);
		shader->setAttrI("textures.normal", 3);
		shader->setAttrI("textures.specular", 4);

		shader->use();

		// texture
		if (mtl.useDiffuseMap)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, mtl.diffuseMap.getID());
		}
		if (mtl.useNormalMap)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mtl.normalMap.getID());
		}
		if (mtl.useSpecularMap)
		{
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, mtl.specularMap.getID());
		}

		glBindVertexArray(VAOs[i]);
		glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

}

// 读取mtl格式文件
void Model::loadMaterialLib(string path)
{
	ifstream in;
	in.open(path, std::ifstream::in);
	if (in.fail())
	{
		cout << "Load file " << path << " fail" << endl;
		return;
	}			

	string dir; // 当前目录
	size_t pos = path.find_last_of("/");
	if (pos != string::npos)
		dir = path.substr(0, pos+1);

	string line;
	Material mtl;
	string mtlName;
	while (!in.eof())
	{
		getline(in, line);
		if (line.empty())
			continue;
		vector<string> tokens;
		Utility::split(line, tokens, " ");
		if (tokens[0] == "newmtl")
		{
			if (mtlName != "")
				modelMaterials[mtlName] = mtl;
			mtlName = tokens[1];
			mtl.init();
		}
		else if (tokens[0] == "Ka")
		{
			for (int i = 0; i < 3; ++i)
				mtl.ambient[i] = atof(tokens[i + 1].c_str());
		}
		else if (tokens[0] == "Kd")
		{
			for (int i = 0; i < 3; ++i)
				mtl.diffuse[i] = atof(tokens[i + 1].c_str());
		}
		else if (tokens[0] == "Ks")
		{
			for (int i = 0; i < 3; ++i)
				mtl.specular[i] = atof(tokens[i + 1].c_str());
		}
		else if (tokens[0] == "Ns")
		{
			mtl.shininess = atof(tokens[1].c_str());
		}
		else if (tokens[0] == "map_Kd")
		{
			string texPath;
			texPath = dir + tokens[1];
			mtl.loadTexture(texPath, TextureType::Diffuse);
		}
		else if (tokens[0] == "map_Ks")
		{
			string texPath;
			texPath = dir + tokens[1];
			mtl.loadTexture(texPath, TextureType::Specular);
		}
		else if (tokens[0] == "map_Bump")
		{
			string texPath;
			//size_t pos = tokens[1].find_first_of("\\");
			//if (pos == string::npos)
			//	pos = tokens[1].find_first_of("/");
			//if (pos == string::npos)
			//	texPath = dir + tokens[1];
			//else
			//	texPath = dir + tokens[1].substr(pos + 1, tokens[1].size());
			texPath = dir + tokens[1];
			mtl.loadTexture(texPath, TextureType::Normal);
		}
	}
	if (mtlName != "")
		modelMaterials[mtlName] = mtl;

}
