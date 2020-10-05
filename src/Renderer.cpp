#include "../include/Renderer.h"
#include <iostream>

Renderer::Renderer() :
    window(nullptr),
    camera(nullptr),
    glfwWindow(nullptr),
    deltaTime(0.0f),
    lastFrame(0.0f),
    lightCube(0.1,0.1,0.1, vec3(1.2, 1.0, 2.0))
{
}


Renderer::~Renderer()
{
}

void Renderer::init(string windowName, int windowWidth, int windowHeight, shared_ptr<Camera> camera_)
{
    window = make_shared<Window>(windowName, windowWidth, windowHeight);
	window->setCamera(camera_);
    glfwWindow = window->getGLFWWindow();
	camera = camera_;

    addResources();
    shader->compile();
}

void Renderer::run()
{
    // �ö��Ժ��޸ģ�����main()�У�
    // ------------------------------------
    //shader = make_shared<Shader>("./shaders/materials.vs", "./shaders/materials.fs");
    lightCubeShader = make_shared<Shader>("./shaders/light_cube.vs", "./shaders/light_cube.fs");
    // create cube
    lightCube.init();
    // create light
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


    //-------------------------------------------------

    // render loop
    // -----------
    while (!glfwWindowShouldClose(glfwWindow))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput();

        // set shader camera
        glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
        glm::mat4 view = camera->getViewMatrix();
        shader->setAttrVec3("viewPos", camera->position);
        shader->setAttrMat4("projection", projection);
        shader->setAttrMat4("view", view);
        lightCubeShader->setAttrMat4("projection", projection);
        lightCubeShader->setAttrMat4("view", view);

        // set shader light
        for (auto light : lights)
        {
            if (light.second->getType() == LightType::Point)
                shader->setAttrI("light.type", 0);
            else if (light.second->getType() == LightType::Directional)
                shader->setAttrI("light.type", 1);
            light.second->setShaderAttr(shader);
        }
        lightCubeShader->setAttrMat4("model", lightCube.getTransMat());

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderLoop();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

}

void Renderer::addResources()
{
}

void Renderer::addMesh(string meshName, shared_ptr<Mesh> mesh)
{
    auto iter = meshes.find(meshName);
    // mesh name already exists
    if (iter != meshes.end())
    {
        cout << " Add Mesh Failed!\tMesh name \"" << meshName << "\" already exists!" << endl;
        return;
    }

    mesh->bind();
    meshes[meshName] = mesh;
}

void Renderer::addLight(string lightName, shared_ptr<Light> light)
{
    auto iter = lights.find(lightName);
    // light name already exists
    if (iter != lights.end())
    {
        cout << " Add Light Failed!\tLight name \"" << lightName << "\" already exists!" << endl;
        return;
    }
    lights[lightName] = light;
}

void Renderer::addShader(shared_ptr<Shader> shader_)
{
    shader = shader_;
}

void Renderer::renderLoop()
{
    //// light properties
    //glm::vec3 lightColor;
    //lightColor.x = sin(glfwGetTime() * 2.0f);
    //lightColor.y = sin(glfwGetTime() * 0.7f);
    //lightColor.z = sin(glfwGetTime() * 1.3f);
    //pointLight.setColor(lightColor);

    //mat4 trans = glm::mat4(1.0f);
    //trans = glm::translate(trans, vec3(sin(glfwGetTime()), 0.0, 0.0));
    //cube.setPosition(vec3(sin(glfwGetTime()), 0.0, 0.0));

    //cube.draw(shader);
    //lightCube.draw(lightCubeShader);
}

void Renderer::processInput()
{
    if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(glfwWindow, true);

    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
        camera->processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
        camera->processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
        camera->processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
        camera->processKeyboard(RIGHT, deltaTime);
}