#include "../include/Skybox.h"
#include "../include/stb_image.h"
#include <iostream>

Skybox::Skybox()
{
    init();
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
Skybox::Skybox(const vector<string>& facesPath)
{
    init();
    load(facesPath);
}

void Skybox::load(const vector<string>& facesPath)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < facesPath.size(); i++)
    {
        unsigned char *data = stbi_load(facesPath[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << facesPath[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void Skybox::draw(const glm::mat4 & view, const glm::mat4 & projection)
{
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
    skyboxShader->setAttrMat4("view", view);
    skyboxShader->setAttrMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    box.draw(skyboxShader);
    glDepthFunc(GL_LESS);
}

void Skybox::init()
{
    skyboxShader = make_shared<Shader>("Shaders/skybox.vert", "Shaders/skybox.frag");
    skyboxShader->setAttrI("skybox", 0);
    box.bind();

}
