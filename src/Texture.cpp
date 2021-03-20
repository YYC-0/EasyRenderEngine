#include "../include/Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include <iostream>

bool Texture::load(string imgPath)
{
	glGenTextures(1, &id);
	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(imgPath.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		type = TextureType::TEXTURE_2D;
		return true;
	}
	else
	{
		cout << "Texture \"" << imgPath << "\" failed to load!" << endl;
		return false;
	}
}

void Texture::set(unsigned int id_, TextureType textureType)
{
	id = id_;
	type = textureType;
}
