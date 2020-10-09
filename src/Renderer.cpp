#include "../include/Renderer.h"
#include <iostream>

Renderer::Renderer() :
    window(nullptr),
    camera(nullptr),
    glfwWindow(nullptr),
    deltaTime(0.0f),
    lastFrame(0.0f),
    clearColor(vec3(0, 0, 0)),
    lightCube(0.1,0.1,0.1, vec3(1.2, 1.0, 2.0))
{
}


Renderer::~Renderer()
{
}

void Renderer::init(string windowName, int windowWidth, int windowHeight)
{
    window = make_shared<Window>(windowName, windowWidth, windowHeight);
	window->setCamera(camera);
    glfwWindow = window->getGLFWWindow();

    addResources();
    shader->compile();
}

void Renderer::run()
{
    // 该段以后修改（放至main()中）
    // ------------------------------------
    //shader = make_shared<Shader>("./shaders/materials.vs", "./shaders/materials.fs");
    lightCubeShader = make_shared<Shader>("./shaders/light_cube.vert", "./shaders/light_cube.frag");
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
        shader->setAttrI("lightNum", lights.size());
        int n = 0;
        for (auto light : lights)
        {
            light.second->setShaderAttr(shader, n++);
        }
        lightCubeShader->setAttrMat4("model", lightCube.getTransMat());
        
        // render
        // ------
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderLoop();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();

        cout << "FPS: " << 1.0 / deltaTime << '\r';
    }

}

void Renderer::addResources()
{
}

void Renderer::addObject(string meshName, shared_ptr<Object> mesh)
{
    auto iter = meshes.find(meshName);
    // mesh name already exists
    if (iter != meshes.end())
    {
        cout << "Add Object Failed! Object name \"" << meshName << "\" already exists!" << endl;
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
        cout << "Add Light Failed! Light name \"" << lightName << "\" already exists!" << endl;
        return;
    }
    lights[lightName] = light;
}

void Renderer::addShader(shared_ptr<Shader> shader_)
{
    shader = shader_;
}

void Renderer::setClearColor(vec3 color)
{
    clearColor = color;
}

void Renderer::setCamera(shared_ptr<Camera> camera_)
{
    camera = camera_;
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
