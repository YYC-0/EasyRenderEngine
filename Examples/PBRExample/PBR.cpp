#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"

#include <iostream>
#include <memory>
using namespace std;

class myRenderer : public Renderer
{
public:
    myRenderer()
    {
        windowWidth = 1200;
        windowHeight = 800;
        // initalize window, should be called first
        init("PBR", windowWidth, windowHeight);

        // create camera
        float aspect = (float)windowWidth / (float)windowHeight;
        camera = make_shared<Camera>(aspect, glm::vec3(-3.0f, 6.0f, -3.0f), glm::vec3(1.0f, -1.0f, 1.0f));

        // create light
        pointLight1 = make_shared<PointLight>(vec3(0, 10, 0));
       // pointLight1->setAttenuation(1.0f, 0.045f, 0.0075f);
        dirLight = make_shared<DirectionalLight>(vec3(0, -1, 0));
        dirLight2 = make_shared<DirectionalLight>(vec3(-1, -1, -1));

        // load cube map
        cubeMap = make_shared<CubeMap>();
        cubeMap->loadHdr("./resources/pbr/textures/hdr/Brooklyn_Bridge_Planks_2k.hdr", 1024);
        cubeMap->generateIrradianceMap();
        cubeMap->generatePrefilterMap();
        cubeMap->generateBrdfLUTTexture();
        Texture irradianceMap(cubeMap->getIrradianceMapID(), TextureType::Irradiance);
        Texture prefilterMap(cubeMap->getPrefilterMapID(), TextureType::Irradiance);
        Texture brdfLUT(cubeMap->getBrdfLUTTextureID(), TextureType::Irradiance);

        // create shader    
        phong = Shader::phong();
        pbrShader = Shader::pbr();

        // create meshes
        float interval = 2.5;
        for (int i = 0; i <= 7; ++i)
        {
            for (int j = 0; j <= 7; ++j)
            {
                shared_ptr<Sphere> sphere = make_shared<Sphere>(1.0, 4, vec3(i * interval, 0.0, j * interval));
                shared_ptr<PBRMaterial> pbrMtl = make_shared<PBRMaterial>(
                        vec3(1.0, 0.0, 0.0), // albedo
                        i/7.0,              // mtallic
                        std::max(0.05, j/7.0),  // roughness
                        1.0);     // ao
                pbrMtl->setIrradianceMap(irradianceMap);
                pbrMtl->setPrefilterMap(prefilterMap);
                pbrMtl->setBrdfLUT(brdfLUT);

                sphere->setMaterial(pbrMtl);
                spheres.push_back(sphere);
            }
        }

        sphere0 = make_shared<Sphere>(1.0, 5, vec3(0.0, 3.0, 0.0));

        shared_ptr<Material> mtl = make_shared<Material>(
                vec3(1.0, 0.5, 0.31),  // ambient
                vec3(1.0, 0.5, 0.31),       // diffuse
                vec3(0.5, 0.5, 0.5),        // specular
                32.0);                       // shininess);

        shared_ptr<PBRMaterial> pbrMtl = make_shared<PBRMaterial>();
        //pbrMtl->loadTextures("./resources/pbr/textures/pbr/ornate-celtic-gold/");
        pbrMtl->loadTextures("./resources/pbr/textures/pbr/gold/");
        pbrMtl->setIrradianceMap(irradianceMap);
        pbrMtl->setPrefilterMap(prefilterMap);
        pbrMtl->setBrdfLUT(brdfLUT);

        sphere0->setMaterial(pbrMtl);

        // test------
        testLUTShader = make_shared<Shader>("Shaders/brdfLUT.vert", "Shaders/brdfLUT.frag");
        testRect = make_shared<Rectangle>(1, 1, vec3(2, 2, 2));
        //testCube = make_shared<Cube>(2,2,2);
        Texture LUT(cubeMap->getBrdfLUTTextureID(), TextureType::Diffuse);
        shared_ptr<Material> rectMtl = make_shared<Material>();
        rectMtl->setTexture(LUT, TextureType::Diffuse);
        //testCube->setMaterial(rectMtl);
        testRect->setMaterial(rectMtl);
        // ------

        // Gui
        gui = make_shared<Gui>();
        gui->add("direction light", dirLight);

        // setting
        //setClearColor(vec3(0, 0, 0));
        setClearColor(vec3(1.0, 1.0, 1.0));
        setCamera(camera);
        setMSAA(true);
    }

    virtual void addResources() override
    {
        for (int i = 0; i < spheres.size(); ++i)
            addObject("sphere" + to_string(i+1), spheres[i]);
        addObject("sphere0", sphere0);

        addObject("Rect", testRect);
        //addObject("testCube", testCube);

        //addLight("pointLight", pointLight1);
        addLight("direction light", dirLight);
        addLight("direction light2", dirLight2);

        addShader("phong", phong);
        addShader("pbr shader", pbrShader);
        addShader("LUT", testLUTShader);

        addSkybox(cubeMap);

        addGui(gui);
    }

    virtual void renderLoop()
    {
        for (auto sphere : spheres)
            draw(sphere, pbrShader);
        draw(sphere0, pbrShader);

        //draw(testCube, phong);
        draw(testRect, phong);
    }

private:
    int windowWidth;
    int windowHeight;
    shared_ptr<Camera> camera;
    vector<shared_ptr<Sphere>> spheres;
    shared_ptr<Sphere> sphere0;

    shared_ptr<PointLight> pointLight1;
    shared_ptr<DirectionalLight> dirLight;
    shared_ptr<DirectionalLight> dirLight2;

    shared_ptr<Shader> phong;
    shared_ptr<Shader> pbrShader;

    shared_ptr<CubeMap> cubeMap;

    shared_ptr<Gui> gui;

    shared_ptr<Shader> testLUTShader;
    shared_ptr<Rectangle> testRect;
    //shared_ptr<Cube> testCube;
};

int main()
{
    myRenderer r;
    r.run();

    return 0;
}
