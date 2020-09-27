#include "../include/Renderer.h"

Renderer::Renderer() :
    window(nullptr),
    camera(nullptr),
    glfwWindow(nullptr),
    deltaTime(0.0f),
    lastFrame(0.0f)
{
}


Renderer::~Renderer()
{
}

void Renderer::init(string windowName, int windowWidth, int windowHeight, shared_ptr<Camera> camera_)
{
    window = make_shared<Window>("test", windowWidth, windowHeight);
	window->setCamera(camera_);
    glfwWindow = window->getGLFWWindow();
	camera = camera_;
}

void Renderer::run()
{
    // build and compile our shader zprogram
    // ------------------------------------
    lightingShader = make_shared<Shader>("./shaders/materials.vs", "./shaders/materials.fs");
    lightCubeShader = make_shared<Shader>("./shaders/light_cube.vs", "./shaders/light_cube.fs");

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    cube.createCube();
    cube.setMaterial(
        Material(vec3(1.0, 0.5, 0.31),  // ambient
            vec3(1.0, 0.5, 0.31),       // diffuse
            vec3(0.5, 0.5, 0.5),        // specular
            32.0)                       // shininess
    );
    lightingShader->setAttrVec3("light.position", lightPos);

    glm::mat4 model = glm::mat4(1.0f);
    lightingShader->setAttrMat4("model", model);    
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    lightCubeShader->setAttrMat4("model", model);

    lightCube.createCube();

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

        glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
        glm::mat4 view = camera->getViewMatrix();
        lightingShader->setAttrVec3("viewPos", camera->position);
        lightingShader->setAttrMat4("projection", projection);
        lightingShader->setAttrMat4("view", view);
        lightCubeShader->setAttrMat4("projection", projection);
        lightCubeShader->setAttrMat4("view", view);

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

void Renderer::renderLoop()
{
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    // be sure to activate shader when setting uniforms/drawing objects
    //lightingShader->use();
    //lightingShader->setVec3("light.position", lightPos);
    //lightingShader->setVec3("viewPos", camera->position);

    // light properties
    glm::vec3 lightColor;
    lightColor.x = sin(glfwGetTime() * 2.0f);
    lightColor.y = sin(glfwGetTime() * 0.7f);
    lightColor.z = sin(glfwGetTime() * 1.3f);
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
    lightingShader->setAttrVec3("light.ambient", ambientColor);
    lightingShader->setAttrVec3("light.diffuse", diffuseColor);
    lightingShader->setAttrVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    // material properties
    //lightingShader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    //lightingShader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    //lightingShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
    //lightingShader->setFloat("material.shininess", 32.0f);

    // view/projection transformations
    //glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
    //glm::mat4 view = camera->getViewMatrix();
    //lightingShader->setMat4("projection", projection);
    //lightingShader->setMat4("view", view);

    // world transformation
    //glm::mat4 model = glm::mat4(1.0f);
    //lightingShader->setMat4("model", model);

    cube.draw(lightingShader);

    // also draw the lamp object
    //lightCubeShader->use();
    //lightCubeShader->setMat4("projection", projection);
    //lightCubeShader->setMat4("view", view);
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, lightPos);
    //model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    //lightCubeShader->setMat4("model", model);

    lightCube.draw(lightCubeShader);
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
