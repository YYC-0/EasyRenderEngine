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

		shader->setAttrI("textures.diffuse", 1);
		shader->setAttrI("textures.normal", 2);
		shader->setAttrI("textures.specular", 3);

		shader->use();


		// texture
		if (materials[i].useDiffuseMap)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, materials[i].diffuseMap.getID());
		}
		if (materials[i].useNormalMap)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, materials[i].normalMap.getID());
		}
		if (materials[i].useSpecularMap)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, materials[i].specularMap.getID());
		}

		glBindVertexArray(VAOs[i]);
		glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Object::setPosition(glm::vec3 pos)
{
	position = pos;
	updateModelMatrix();
}

void Object::setScale(glm::vec3 scale_)
{
	scale = scale_;
	updateModelMatrix();
}

vector<float> Object::transformToInterleavedData()
{
	vector<float> interleavedData;
	for (int i = 0; i < vertices.size(); ++i)
	{
		interleavedData.push_back(vertices[i].x);
		interleavedData.push_back(vertices[i].y);
		interleavedData.push_back(vertices[i].z);
		interleavedData.push_back(normals[i].x);
		interleavedData.push_back(normals[i].y);
		interleavedData.push_back(normals[i].z);
		interleavedData.push_back(texCoords[i].x);
		interleavedData.push_back(texCoords[i].y);
	}
	return interleavedData;
}

void Object::updateModelMatrix()
{
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, scale);
	transInvModelMatrix = glm::transpose(glm::inverse(modelMatrix));
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

		shader->setAttrI("textures.diffuse", 1);
		shader->setAttrI("textures.normal", 2);
		shader->setAttrI("textures.specular", 3);

		shader->use();


		// texture
		if (materials[i].useDiffuseMap)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, materials[i].diffuseMap.getID());
		}
		if (materials[i].useNormalMap)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, materials[i].normalMap.getID());
		}
		if (materials[i].useSpecularMap)
		{
			glActiveTexture(GL_TEXTURE3);
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
			for (int k = 0; k < 3; ++k)
			{
				indices_.push_back(f[k].posIdx);
				normals[f[k].posIdx] = normals_[f[k].nIdx];
				texCoords[f[k].posIdx] = texCoords_[f[k].texIdx];
			}
			faceNum++;
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

		shader->use();

		shader->setAttrI("textures.diffuse", 1);
		shader->setAttrI("textures.normal", 2);
		shader->setAttrI("textures.specular", 3);
		// texture
		if (mtl.useDiffuseMap)
		{
			glActiveTexture(GL_TEXTURE1);
			shader->setAttrI("Texture.diffuse", 1);
			glBindTexture(GL_TEXTURE_2D, mtl.diffuseMap.getID());
		}
		if (mtl.useNormalMap)
		{
			glActiveTexture(GL_TEXTURE2);
			shader->setAttrI("Texture.normal", 2);
			glBindTexture(GL_TEXTURE_2D, mtl.normalMap.getID());
		}
		if (mtl.useSpecularMap)
		{
			glActiveTexture(GL_TEXTURE3);
			shader->setAttrI("Texture.specular", 3);
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
