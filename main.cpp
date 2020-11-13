#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/Shader.h"
#include "include/Camera.h"
#include "include/Window.h"
#include "include/Renderer.h"

#include <iostream>
#include <memory>
using namespace std;

class myRenderer : public Renderer
{
public:
    myRenderer()
    {
        windowWidth = 1024;
        windowHeight = 768;
        // initalize window
        init("TEST", windowWidth, windowHeight);

        // create camera
        float aspect = (float)windowWidth / (float)windowHeight;
        camera = make_shared<Camera>(aspect, glm::vec3(0.0f, 1.0f, 3.0f));
        // create light
        lightDir = vec3(0.2f, -1.0f, 0.2f);
        directionalLight = make_shared<DirectionalLight>(lightDir);
        directionalLight2 = make_shared<DirectionalLight>(vec3(0.1,-1,0));
        pointLight = make_shared<PointLight>(vec3(0, 3, 0));
        pointLight->setAttenuation(1.0f, 0.045f, 0.0075f);
        pointLight2 = make_shared<PointLight>(vec3(7, 3, 0));
        pointLight2->setAttenuation(1.0f, 0.045f, 0.0075f);
        // create shader    
        shader = make_shared<Shader>("./shaders/materials.vert", "./shaders/materials.frag");

        // create meshes
        cube = make_shared<Cube>(1.0, 1.0, 1.0); // size
        plane = make_shared<Cube>(10.0, 0.02, 10.0);
        // ---cube material
        Material cubeMaterial, planeMaterial;
        cubeMaterial.loadTexture("./resources/brickwall.jpg", TextureType::Diffuse);
        //cubeMaterial.loadTexture("./resources/container_specular.png", TextureType::Specular);
        cubeMaterial.loadTexture("./resources/brickwall_normal.jpg", TextureType::Normal);
        cube->setMaterial(cubeMaterial);
        planeMaterial.loadTexture("./resources/grass.jpg", TextureType::Diffuse);
        plane->setMaterial(planeMaterial);

        model = make_shared<Model>();
        //model->loadObj("./models/nanosuit/nanosuit.obj");
        //model->setScale({ 0.1,0.1,0.1 });
        model->loadObj("./models/suzanne/suzanne.obj");
        model->setPosition({ 0, 0, 0 });

        // sponza
        //sponza = make_shared<Model>("./models/Sponza-master/sponza.obj");
        //sponza->setScale({ 0.01, 0.01, 0.01 });

        // Gui
        gui = make_shared<Gui>();
        gui->addObject("cube", cube);
        gui->addObject("plain", plane);

        // setting
        setClearColor(vec3(1, 1, 1));
        setCamera(camera);

    }

    virtual void addResources()
    {
        addObject("cube", cube);
        addObject("plain", plane);
        //addObject("model", model);
       // addObject("sponza", sponza);
        //addLight("directionalLight", directionalLight);
       // addLight("directionalLight2", directionalLight2);
        addLight("pointLight", pointLight);
        addLight("pointLight2", pointLight2);
        addShader(shader);
        addGui(gui);
    }

    virtual void renderLoop()
    {
        // light properties
        glm::vec3 lightColor(1.0);
        //lightColor.x = sin(glfwGetTime() * 2.0f);
        //lightColor.y = sin(glfwGetTime() * 0.7f);
        //lightColor.z = sin(glfwGetTime() * 1.3f);
        pointLight->setColor(lightColor);
        pointLight->setPosition(lightPos);

        //model->draw(shader);
        //draw(sponza, shader);
        //draw(model, shader);
        draw(cube, shader);
        draw(plane, shader);
    }

private:
    vec3 lightPos;
    int windowWidth;
    int windowHeight;
    vec3 lightDir;
    shared_ptr<Camera> camera;
    shared_ptr<Cube> cube;
    shared_ptr<Cube> plane;
    shared_ptr<DirectionalLight> directionalLight;
    shared_ptr<DirectionalLight> directionalLight2;
    shared_ptr<PointLight> pointLight;
    shared_ptr<PointLight> pointLight2;
    shared_ptr<Shader> shader;

    shared_ptr<Model> model;
    shared_ptr<Model> sponza;

    shared_ptr<Gui> gui;
};

int main()
{
    myRenderer r;
    r.run();

    return 0;
}
