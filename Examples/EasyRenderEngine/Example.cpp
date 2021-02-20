/*#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/Renderer.h"

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
        camera = make_shared<Camera>(aspect, glm::vec3(0.0f, 1.0f, 3.0f));
        // create light
        lightDir = vec3(0.2f, -1.0f, 0.2f);
        directionalLight = make_shared<DirectionalLight>(lightDir);
        pointLight = make_shared<PointLight>(vec3(11.0, 1.5, -2.7));
        pointLight->setAttenuation(1.0f, 0.045f, 0.0075f);
        pointLight2 = make_shared<PointLight>(vec3(10.4, 7.2, -3.7));
        pointLight2->setAttenuation(1.0f, 0.045f, 0.0075f);
        pointLight3 = make_shared<PointLight>(vec3(-11.1, 2.6, -0.3));
        pointLight3->setAttenuation(1.0f, 0.045f, 0.0075f);

        // create shader    
        shader = Shader::phong();

        // create meshes
        cube = make_shared<Cube>(1.0, 1.0, 1.0); // size
        // ---cube material
        Material cubeMaterial, planeMaterial;
        cubeMaterial.loadTexture("./resources/brickwall.jpg", TextureType::Diffuse);
        cubeMaterial.loadTexture("./resources/brickwall_normal.jpg", TextureType::Normal);
        cube->setMaterial(cubeMaterial);

        // sponza
        sponza = make_shared<Model>("./resources/models/Sponza-master/sponza.obj");
        sponza->setScale({ 0.01, 0.01, 0.01 });

        // skybox
        vector<string> skyboxPath{ "resources/skybox/right.jpg", "resources/skybox/left.jpg",
            "resources/skybox/top.jpg", "resources/skybox/bottom.jpg",
            "resources/skybox/front.jpg", "resources/skybox/back.jpg" };
        skybox = make_shared<Skybox>(skyboxPath);

        // Gui
        gui = make_shared<Gui>();
        gui->add("cube", cube);
        gui->add("pointlight", pointLight);
        gui->add("pointlight2", pointLight2);
        gui->add("pointlight3", pointLight3);
        gui->add("dirlight", directionalLight);

        // setting
        //setClearColor(vec3(0, 0, 0));
        setClearColor(vec3(1.0, 1.0, 1.0));
        setCamera(camera);
        setMSAA(true);

    }

    virtual void addResources() override
    {
        addObject("cube", cube);
        addObject("sponza", sponza);

        addLight("directionalLight", directionalLight);
        addLight("pointLight", pointLight);
        addLight("pointLight2", pointLight2);
        addLight("pointLight3", pointLight3);

        addShader("phong", shader);

        addSkybox(skybox);

        addGui(gui);
    }

    virtual void renderLoop()
    {
        draw(sponza, shader);
        draw(cube, shader);
    }

private:
    int windowWidth;
    int windowHeight;
    vec3 lightDir;
    shared_ptr<Camera> camera;
    shared_ptr<Cube> cube;
    shared_ptr<Model> sponza;

    shared_ptr<DirectionalLight> directionalLight;
    shared_ptr<PointLight> pointLight;
    shared_ptr<PointLight> pointLight2;
    shared_ptr<PointLight> pointLight3;

    shared_ptr<Shader> shader;

    shared_ptr<Skybox> skybox;

    shared_ptr<Gui> gui;
};

int main()
{
    myRenderer r;
    r.run();

    return 0;
}
*/