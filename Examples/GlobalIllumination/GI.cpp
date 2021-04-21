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
        init("Sponza", windowWidth, windowHeight);

        // create camera
        float aspect = (float)windowWidth / (float)windowHeight;
        camera = make_shared<Camera>(aspect, glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(-1, -1, -1));
        // create light
        lightDir = vec3(-0.5, -0.4, -0.75);
        directionalLight = make_shared<DirectionalLight>(lightDir);

        // create meshes
        buddha = make_shared<Model>("./resources/models/Happy Buddha/happy_vrip.obj");
        buddha->setScale(vec3(22, 22, 22));
        buddha->setPosition(vec3(0, -3.5, 0));
        shared_ptr<Material> suzanneMtl = make_shared<Material>();
        buddha->setMaterial(suzanneMtl);

        cube1 = make_shared<Cube>(5.0, 0.1, 5.0); // size
        cube2 = make_shared<Cube>(0.1, 5.0, 5.0);
        cube3 = make_shared<Cube>(5.0, 5.0, 0.1);
        cube1->setPosition(vec3(0, -2.5, 0));
        cube2->setPosition(vec3(-2.5, 0, 0));
        cube3->setPosition(vec3(0, 0, -2.5));
        shared_ptr<Material> cube1Mtl = make_shared<Material>(vec3(1.0), vec3(0, 0, 1), vec3(0.1), 16);
        shared_ptr<Material> cube2Mtl = make_shared<Material>(vec3(1.0), vec3(1, 0, 0), vec3(0.1), 16);
        shared_ptr<Material> cube3Mtl = make_shared<Material>(vec3(1.0), vec3(0, 1, 0), vec3(0.1), 16);
        cube1->setMaterial(cube1Mtl);
        cube2->setMaterial(cube2Mtl);
        cube3->setMaterial(cube3Mtl);

        test1 = make_shared<Rectangle>(1.0, 1.0, vec3(-2, 1, 0));
        test1->setRotateX(-90);
        test2 = make_shared<Rectangle>(1.0, 1.0, vec3(0, 1, 0));
        test2->setRotateX(-90);
        test3 = make_shared<Rectangle>(1.0, 1.0, vec3(2, 1, 0));
        test3->setRotateX(-90);
        test4 = make_shared<Rectangle>(1.0, 1.0, vec3(4, 1, 0));
        test4->setRotateX(-90);
        // material
        shared_ptr<Material> RSM_positionMtl = make_shared<Material>();
        RSM_positionMtl->setTexture(RSM_position, MaterialMapType::Diffuse);
        test1->setMaterial(RSM_positionMtl);

        shared_ptr<Material> RSM_normalMtl = make_shared<Material>();
        RSM_normalMtl->setTexture(RSM_normal, MaterialMapType::Diffuse);
        test2->setMaterial(RSM_normalMtl);

        shared_ptr<Material> RSM_fluxMtl = make_shared<Material>();
        RSM_fluxMtl->setTexture(RSM_flux, MaterialMapType::Diffuse);
        test3->setMaterial(RSM_fluxMtl);

        shared_ptr<Material> RSM_depthMtl = make_shared<Material>();
        RSM_depthMtl->setTexture(RSM_depth, MaterialMapType::Diffuse);
        test4->setMaterial(RSM_depthMtl);


        // sponza
        //sponza = make_shared<Model>("./resources/models/Sponza-master/sponza.obj");
        //sponza->setScale({ 0.01, 0.01, 0.01 });

        // Gui
        gui = make_shared<Gui>();
        //gui->add("cube", cube);
        //gui->add("pointlight", pointLight);
        //gui->add("pointlight2", pointLight2);
        //gui->add("pointlight3", pointLight3);
        gui->add("dirlight", directionalLight);
        gui->add("plan1", cube1);
        gui->add("plan2", cube2);
        gui->add("plan3", cube3);
        gui->add("suzanne", buddha);

        // setting
        setClearColor(vec3(0, 0, 0));
        setCamera(camera);
        setMSAA(true);

    }

    virtual void addResources() override
    {
        addObject("cube1", cube1);
        addObject("cube2", cube2);
        addObject("cube3", cube3);
        addObject("suzanne", buddha);
        //addObject("plane", plane);
       // addObject("sponza", sponza);
        //addObject("test1", test1);
        //addObject("test2", test2);
        //addObject("test3", test3);
        //addObject("test4", test4);

        addLight("directionalLight", directionalLight);
        //addLight("pointLight", pointLight);
        //addLight("pointLight2", pointLight2);
       // addLight("pointLight3", pointLight3);

        //addSkybox(skybox);

        addGui(gui);
    }

    virtual void renderLoop()
    {
    }

private:
    int windowWidth;
    int windowHeight;
    vec3 lightDir;
    shared_ptr<Model> buddha;
    shared_ptr<Camera> camera;
    shared_ptr<Cube> cube1;
    shared_ptr<Cube> cube2;
    shared_ptr<Cube> cube3;
    shared_ptr<Model> sponza;
    shared_ptr<Rectangle> test1;
    shared_ptr<Rectangle> test2;
    shared_ptr<Rectangle> test3;
    shared_ptr<Rectangle> test4;

    shared_ptr<DirectionalLight> directionalLight;
    shared_ptr<PointLight> pointLight;
    shared_ptr<PointLight> pointLight2;
    shared_ptr<PointLight> pointLight3;


    shared_ptr<Gui> gui;
};

int main()
{
    myRenderer r;
    r.run();

    return 0;
}
