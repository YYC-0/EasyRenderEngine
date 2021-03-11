#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"

#include <iostream>
#include <memory>
#include <cmath>
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

        // create meshes
        //float interval = 2.5;
        //for (int i = 0; i <= 7; ++i)
        //{
        //    for (int j = 0; j <= 7; ++j) 
        //    {
        //        shared_ptr<Sphere> sphere = make_shared<Sphere>(1.0, 4, vec3(i * interval, 0.0, j * interval));
        //        shared_ptr<PBRMaterial> pbrMtl = make_shared<PBRMaterial>(
        //                vec3(1.0, 0.0, 0.0), // albedo
        //                i/7.0,              // mtallic
        //                std::max(0.05, j/7.0),  // roughness
        //                1.0);     // ao

        //        sphere->setMaterial(pbrMtl);
        //        spheres.push_back(sphere);
        //    }
        //}

        spheres.resize(8);
        for(int i=0; i<spheres.size(); ++i)
            spheres[i] = make_shared<Sphere>(1.0, 5, vec3(cos(i* M_PI/4.0) * 4, 0.0, sin(i*M_PI/4.0) * 4));
        //sphere0 = make_shared<Sphere>(1.0, 5, vec3(0.0, 3.0, 0.0));

        suzanne = make_shared<Model>("./resources/suzanne.obj");
        suzanne->setScale(vec3(1.5));
        suzanne->setRotateY(180);

        // create material
        shared_ptr<PBRMaterial> wood = make_shared<PBRMaterial>();
        wood->loadTextures("./resources/pbr/textures/pbr/bamboo-wood-semigloss/");
        shared_ptr<PBRMaterial> gold = make_shared<PBRMaterial>();
        gold->loadTextures("./resources/pbr/textures/pbr/gold/");
        shared_ptr<PBRMaterial> ornateCelticGold = make_shared<PBRMaterial>();
        ornateCelticGold->loadTextures("./resources/pbr/textures/pbr/ornate-celtic-gold/");
        shared_ptr<PBRMaterial> paintPeeling = make_shared<PBRMaterial>();
        paintPeeling->loadTextures("./resources/pbr/textures/pbr/paint-peeling/");
        shared_ptr<PBRMaterial> slipperystonework = make_shared<PBRMaterial>();
        slipperystonework->loadTextures("./resources/pbr/textures/pbr/slipperystonework/");
        shared_ptr<PBRMaterial> titaniumScuffed = make_shared<PBRMaterial>();
        titaniumScuffed->loadTextures("./resources/pbr/textures/pbr/Titanium-Scuffed/");
        shared_ptr<PBRMaterial> wornpaintedcement = make_shared<PBRMaterial>();
        wornpaintedcement->loadTextures("./resources/pbr/textures/pbr/wornpaintedcement/");
        shared_ptr<PBRMaterial> wrinkledPaper = make_shared<PBRMaterial>();
        wrinkledPaper->loadTextures("./resources/pbr/textures/pbr/wrinkled-paper/");

        shared_ptr<PBRMaterial> suzanneMtl = make_shared<PBRMaterial>(vec3(1.0), 1.0, 0.1); // albedo metallic roughness
        suzanne->setMaterial(suzanneMtl);

        spheres[0]->setMaterial(wood);
        spheres[1]->setMaterial(gold);
        spheres[2]->setMaterial(ornateCelticGold);
        spheres[3]->setMaterial(paintPeeling);
        spheres[4]->setMaterial(slipperystonework);
        spheres[5]->setMaterial(titaniumScuffed);
        spheres[6]->setMaterial(wornpaintedcement);
        spheres[7]->setMaterial(wrinkledPaper);

        // Gui
        gui = make_shared<Gui>();
        gui->add("direction light", dirLight);
        gui->add("suzanne", suzanne);
        gui->add("suzanne mtl", suzanneMtl);
        gui->add("wood mtl", wood);
        gui->add("ornateCelticGold mtl", ornateCelticGold);

        // setting
        //setClearColor(vec3(0, 0, 0));
        setClearColor(vec3(1.0, 1.0, 1.0));
        setCamera(camera);
        setMSAA(true);

        setPBRMode(true); // !!!!
    }

    virtual void addResources() override
    {
        for (int i = 0; i < spheres.size(); ++i)
            addObject("sphere" + to_string(i+1), spheres[i]);

        addObject("suzanne", suzanne);

        //addLight("pointLight", pointLight1);
        addLight("direction light", dirLight);
        addLight("direction light2", dirLight2);

        addEnvironmentMap(cubeMap);

        addSkybox(cubeMap);

        addGui(gui);
    }

    virtual void renderLoop()
    {
        //for (auto sphere : spheres)
        //    draw(sphere);
        //draw(suzanne);
    }

private:
    int windowWidth;
    int windowHeight;
    shared_ptr<Camera> camera;
    vector<shared_ptr<Sphere>> spheres;
    shared_ptr<Model> suzanne;

    shared_ptr<PointLight> pointLight1;
    shared_ptr<DirectionalLight> dirLight;
    shared_ptr<DirectionalLight> dirLight2;


    shared_ptr<CubeMap> cubeMap;

    shared_ptr<Gui> gui;
};

int main()
{
    myRenderer r;
    r.run();

    return 0;
}
