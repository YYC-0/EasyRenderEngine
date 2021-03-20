#include "../include/CubeMap.h"
#include "../include/stb_image.h"
#include <iostream>

CubeMap::CubeMap()
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
CubeMap::CubeMap(const vector<string>& facesPath)
{
    init();
    load(facesPath);
}

void CubeMap::load(const vector<string>& facesPath)
{
    unsigned int cubeMapID;
    glGenTextures(1, &cubeMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);

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

    cubeMap.set(cubeMapID, TextureType::TEXTURE_CUBE_MAP);
}

void CubeMap::loadHdr(const string &path, int cubeSideLength)
{
    gammaCorrection = true;

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    float *data = stbi_loadf(path.c_str(), &width, &height, &nrChannels, 0);
    unsigned int hdrTextureID = 0;
    if (data)
    {
        glGenTextures(1, &hdrTextureID);
        glBindTexture(GL_TEXTURE_2D, hdrTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "HDR image failed to load at path: " << path << std::endl;
    }
    Texture hdrTexture(hdrTextureID, TextureType::TEXTURE_2D);

    stbi_image_free(data);

    // setup framebuffer
    unsigned int captureFBO;
    //unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    //glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    //glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeSideLength, cubeSideLength);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // setup cubemap to render to and attach to framebuffer
    unsigned int cubeMapID;
    glGenTextures(1, &cubeMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, cubeSideLength, cubeSideLength, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cubeMap.set(cubeMapID, TextureType::TEXTURE_CUBE_MAP);

    // convert HDR equirectangular environment map to cubemap equivalent
    shared_ptr<Shader> equirectangularToCubemapShader = make_shared<Shader>("Shaders/cube_map.vert", "Shaders/equirectangular_to_cubemap.frag");
    equirectangularToCubemapShader->use();
    equirectangularToCubemapShader->setAttrMat4("projection", captureProjection);
    equirectangularToCubemapShader->setTexture("equirectangularMap", 0, &hdrTexture);

    glViewport(0, 0, cubeSideLength, cubeSideLength);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glDisable(GL_CULL_FACE);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader->setAttrMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        box.draw(equirectangularToCubemapShader);
    }
    glEnable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void CubeMap::drawAsSkybox(const glm::mat4 & view, const glm::mat4 & projection)
{
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
    skyboxShader->setAttrMat4("view", view);
    skyboxShader->setAttrMat4("projection", projection);
    skyboxShader->setAttrB("gammaCorrection", gammaCorrection);
    skyboxShader->setTexture("skybox", 0, &cubeMap);

    box.draw(skyboxShader);

    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
}

// generate irradianceMap, prefilterMap, brdfLUTTexture for IBL
void CubeMap::preComputeMaps()
{
    generateIrradianceMap();
    generatePrefilterMap();
    generateBrdfLUTTexture();
}

// generate irradiance map from the environment map
void CubeMap::generateIrradianceMap()
{
    unsigned int irradianceMapID;
    glGenTextures(1, &irradianceMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    irradianceMap.set(irradianceMapID, TextureType::TEXTURE_CUBE_MAP);

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    shared_ptr<Shader> convolutionShader = make_shared<Shader>("Shaders/cube_map.vert", "Shaders/irradiance_convolution.frag");
    convolutionShader->use();
    convolutionShader->setAttrMat4("projection", captureProjection);
    convolutionShader->setTexture("environmentMap", 0, &cubeMap);

    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glDisable(GL_CULL_FACE);
    for (unsigned int i = 0; i < 6; ++i)
    {
        convolutionShader->setAttrMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMapID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        box.draw(convolutionShader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);

}

// generate prefilter map for specular IBL
void CubeMap::generatePrefilterMap()
{
    unsigned int prefilterMapID;
    glGenTextures(1, &prefilterMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMapID);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    prefilterMap.set(prefilterMapID, TextureType::TEXTURE_CUBE_MAP);

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    shared_ptr<Shader> prefilterShader = make_shared<Shader>("Shaders/cube_map.vert", "Shaders/prefilter.frag");
    prefilterShader->use();
    prefilterShader->setAttrMat4("projection", captureProjection);
    prefilterShader->setTexture("environmentMap", 0, &cubeMap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    glDisable(GL_CULL_FACE); 
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth = 128 * pow(0.5, mip);
        unsigned int mipHeight = 128 * pow(0.5, mip);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader->setAttrF("roughness", roughness);

        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader->setAttrMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMapID, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            box.draw(prefilterShader);
        }
    }
    glEnable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeMap::generateBrdfLUTTexture()
{
    unsigned int brdfLUTTextureID;
    glGenTextures(1, &brdfLUTTextureID);

    glBindTexture(GL_TEXTURE_2D, brdfLUTTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    brdfLUTTexture.set(brdfLUTTextureID, TextureType::TEXTURE_2D);

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTextureID, 0);

    shared_ptr<Shader> brdfLUTShader = make_shared<Shader>("Shaders/brdfLUT.vert", "Shaders/brdfLUT.frag");
    brdfLUTShader->use();
    Rectangle rect;
    rect.bind();

    glViewport(0, 0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rect.draw(brdfLUTShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeMap::init()
{
    gammaCorrection = false;
    skyboxShader = make_shared<Shader>("Shaders/skybox.vert", "Shaders/skybox.frag");
    box.bind();

    // set up projection and view matrices for capturing data onto the 6 cubemap face directions
    captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    captureViews =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
}
