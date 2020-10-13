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
        init("TEST", 800, 600);

        // create camera
        camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
        // create meshes
        lightDir = vec3(-1.0,-1.0, 1.0);
        cube = make_shared<Cube>(2.0, 1.0, 1.0); // size
        // ---cube material
        Material cubeMaterial;
        cubeMaterial.loadTexture("./resources/container.png", TextureType::Diffuse);
        cubeMaterial.loadTexture("./resources/container_specular.png", TextureType::Specular);
        cubeMaterial.shininess = 64;
        cube->setMaterial(cubeMaterial);

        plane = make_shared<Cube>(5.0, 0.02, 5.0);
        plane->setMaterial(
            Material(vec3(1.0, 0.5, 0.31),  // ambient
                vec3(1.0, 0.5, 0.31),       // diffuse
                vec3(0.5, 0.5, 0.5),        // specular
                32.0)                       // shininess
        );
        plane->setMaterial(cubeMaterial);
        model = make_shared<Model>();
        //model->loadObj("./models/spider/spider.obj");
        //model->setScale({ 0.01,0.01,0.01 });
        model->loadObj("./models/suzanne/suzanne.obj");
        model->setPosition({ 0, 1, 0 });
        model->setMaterial(
            Material(vec3(1.0, 0.5, 0.31),  // ambient
                vec3(1.0, 0.5, 0.31),       // diffuse
                vec3(0.5, 0.5, 0.5),        // specular
                32.0)                       // shininess
        );
        // create light
        directionalLight = make_shared<DirectionalLight>(lightDir);
        pointLight = make_shared<PointLight>(vec3(-3, 5, 0));
        // create shader    
        shader = make_shared<Shader>("./shaders/materials.vert", "./shaders/materials.frag");

        // setting
        setClearColor(vec3(0.1, 0.1, 0.1));
        setCamera(camera);

    }

    virtual void addResources()
    {
        addObject("cube", cube);
        addObject("plain", plane);
        addObject("model", model);
        addLight("directionalLight", directionalLight);
        addLight("pointLight", pointLight);
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

        vec3 pos(0.0, sin(glfwGetTime()), -1.0);
        cube->setPosition(pos);

        cube->draw(shader);
        plane->draw(shader);
        model->draw(shader);
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
};

int main()
{
    myRenderer r;
    r.run();

    return 0;
}
