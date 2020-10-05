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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class myRenderer : public Renderer
{
public:
    myRenderer()
    {
        // create camera
        camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
        // create meshes
        lightDir = vec3(-1.0,-1.0, 1.0);
        cube = make_shared<Cube>(2.0, 1.0, 1.0);
        cube->setMaterial(
            Material(vec3(1.0, 0.5, 0.31),  // ambient
                vec3(1.0, 0.5, 0.31),       // diffuse
                vec3(0.5, 0.5, 0.5),        // specular
                32.0)                       // shininess
        );
        plane = make_shared<Cube>(5.0, 0.02, 5.0);
        plane->setMaterial(
            Material(vec3(1.0, 0.5, 0.31),  // ambient
                vec3(1.0, 0.5, 0.31),       // diffuse
                vec3(0.5, 0.5, 0.5),        // specular
                32.0)                       // shininess
        );
        // create light
        light = make_shared<DirectionalLight>(lightDir);
        // create shader    
        shader = make_shared<Shader>("./shaders/materials.vs", "./shaders/materials.fs");

        // initalize window
        init("test", SCR_WIDTH, SCR_HEIGHT, camera);
    }

    virtual void addResources()
    {
        addMesh("cube", cube);
        addMesh("plain", plane);
        addLight("pointLight", light);
        addShader(shader);
    }

    virtual void renderLoop()
    {
        // light properties
        glm::vec3 lightColor(1.0);
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
        light->setColor(lightColor);

        vec3 pos(0.0, sin(glfwGetTime()), 0.0);
        cube->setPosition(pos);

        cube->draw(shader);
        plane->draw(shader);
    }

private:
    vec3 lightDir;
    shared_ptr<Camera> camera;
    shared_ptr<Cube> cube;
    shared_ptr<Cube> plane;
    shared_ptr<DirectionalLight> light;
    shared_ptr<Shader> shader;
};

int main()
{
    myRenderer r;
    r.run();

    return 0;
}
