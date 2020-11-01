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
        // initalize window
        init("TEST", 1024, 768);

        // create camera
        camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
        // create light
        lightDir = vec3(1.0f, -1.0f, 1.0f);
        directionalLight = make_shared<DirectionalLight>(lightDir);
        pointLight = make_shared<PointLight>(vec3(-3, 5, 0));
        // create shader    
        shader = make_shared<Shader>("./shaders/materials.vert", "./shaders/materials.frag");

        // create meshes
        cube = make_shared<Cube>(1.0, 1.0, 1.0); // size
        plane = make_shared<Cube>(10.0, 0.02, 10.0);
        // ---cube material
        Material cubeMaterial, planeMaterial;
        cubeMaterial.loadTexture("./resources/container.png", TextureType::Diffuse);
        cubeMaterial.loadTexture("./resources/container_specular.png", TextureType::Specular);
        cube->setMaterial(cubeMaterial);
        planeMaterial.loadTexture("./resources/grass.jpg", TextureType::Diffuse);
        plane->setMaterial(planeMaterial);

        model = make_shared<Model>();
        //model->loadObj("./models/nanosuit/nanosuit.obj");
        //model->setScale({ 0.1,0.1,0.1 });
        model->loadObj("./models/suzanne/suzanne.obj");
        model->setPosition({ 0, 0, 0 });

        // sponza
        sponza = make_shared<Model>("./models/Sponza-master/sponza.obj");
        sponza->setScale({ 0.01, 0.01, 0.01 });

        // setting
        setClearColor(vec3(1, 1, 1));
        setCamera(camera);

    }

    virtual void addResources()
    {
        addObject("cube", cube);
        addObject("plain", plane);
        //addObject("model", model);
        addObject("sponza", sponza);
        addLight("directionalLight", directionalLight);
        //addLight("pointLight", pointLight);
        addShader(shader);
    }

    virtual void renderLoop()
    {
        // light properties
        glm::vec3 lightColor(1.0);
        //lightColor.x = sin(glfwGetTime() * 2.0f);
        //lightColor.y = sin(glfwGetTime() * 0.7f);
        //lightColor.z = sin(glfwGetTime() * 1.3f);
        pointLight->setColor(lightColor);

        vec3 pos(0.0, sin(glfwGetTime()) + 1, -1.0);
        cube->setPosition(pos);

        //model->draw(shader);
        draw(sponza, shader);
        //draw(model, shader);
        //draw(cube, shader);
        //draw(plane, shader);
    }

private:
    vec3 lightDir;
    shared_ptr<Camera> camera;
    shared_ptr<Cube> cube;
    shared_ptr<Cube> plane;
    shared_ptr<DirectionalLight> directionalLight;
    shared_ptr<PointLight> pointLight;
    shared_ptr<Shader> shader;

    shared_ptr<Model> model;
    shared_ptr<Model> sponza;
};

int main()
{
    myRenderer r;
    r.run();

    return 0;
}
