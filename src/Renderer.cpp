#include "../include/Renderer.h"

Renderer::Renderer() :
    window(nullptr),
    camera(nullptr),
    glfwWindow(nullptr),
    deltaTime(0.0f),
    lastFrame(0.0f),
    cube(2.0) // 之后删掉
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
    // 该段以后修改（放至main()中）
    // ------------------------------------
    lightingShader = make_shared<Shader>("./shaders/materials.vs", "./shaders/materials.fs");
    lightCubeShader = make_shared<Shader>("./shaders/light_cube.vs", "./shaders/light_cube.fs");
    // create cube

    cube.init();
    cube.setMaterial(
        Material(vec3(1.0, 0.5, 0.31),  // ambient
            vec3(1.0, 0.5, 0.31),       // diffuse
            vec3(0.5, 0.5, 0.5),        // specular
            32.0)                       // shininess
    );
    // create light
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    pointLight = Light(lightPos);

    glm::mat4 modelMatLight = glm::mat4(1.0f);
    modelMatLight = glm::translate(modelMatLight, lightPos);
    modelMatLight = glm::scale(modelMatLight, glm::vec3(0.2f)); // a smaller cube

    lightCube.createCube();
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
        lightingShader->setAttrVec3("viewPos", camera->position);
        lightingShader->setAttrMat4("projection", projection);
        lightingShader->setAttrMat4("view", view);
        lightCubeShader->setAttrMat4("projection", projection);
        lightCubeShader->setAttrMat4("view", view);
        lightingShader->setAttrMat4("model", cube.getTransMat()); // model matrix

        // set shader light
        lightingShader->setAttrVec3("light.position", pointLight.getPos());
        lightingShader->setAttrVec3("light.ambient", pointLight.getAmbient());
        lightingShader->setAttrVec3("light.diffuse", pointLight.getDiffuse());
        lightingShader->setAttrVec3("light.specular", pointLight.getSpecular());
        lightCubeShader->setAttrMat4("model", modelMatLight);

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
    // light properties
    glm::vec3 lightColor;
    lightColor.x = sin(glfwGetTime() * 2.0f);
    lightColor.y = sin(glfwGetTime() * 0.7f);
    lightColor.z = sin(glfwGetTime() * 1.3f);
    pointLight.setColor(lightColor);

    mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, vec3(sin(glfwGetTime()), 0.0, 0.0));
    cube.setPosition(vec3(sin(glfwGetTime()), 0.0, 0.0));

    cube.draw(lightingShader);
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
