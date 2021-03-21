#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
using namespace std;

enum TextureType
{
	TEXTURE_1D = GL_TEXTURE_1D,
	TEXTURE_2D = GL_TEXTURE_2D,
	TEXTURE_3D = GL_TEXTURE_3D,
	TEXTURE_1D_ARRAY = GL_TEXTURE_1D_ARRAY,
	TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
	TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
	TEXTURE_CUBE_MAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
};


class Texture
{
public:
	Texture() = default;
	Texture(unsigned int id_, TextureType textureType) :
		id(id_), type(textureType) {}
	~Texture();

	bool load(string imgPath);
	void set(unsigned int id_, TextureType textureType);
	unsigned int getID() const { return id; }
	TextureType getType() const { return type; }
	void generate(TextureType t);

private:
	unsigned int id;
	TextureType type;
};