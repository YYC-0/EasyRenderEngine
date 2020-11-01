#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"
#include "../include/Renderer.h"
#include "../include/Utility.h"

Renderer::Renderer() :
    window(nullptr),
    camera(nullptr),
    glfwWindow(nullptr),
    deltaTime(0.0f),
    lastFrame(0.0f),
    clearColor(vec3(0, 0, 0)),
    lightCube(0.1, 0.1, 0.1, vec3(1.2, 1.0, 2.0)),
    shadowWidth(2048),
    shadowHeight(2048),
    lightNearPlane(1.0f),
    lightFarPlane(100.0f)
{
}


Renderer::~Renderer()
{
    glDeleteFramebuffers(1, &depthMapFBO);
}

void Renderer::init(string windowName, int windowWidth, int windowHeight)
{
    window = make_shared<Window>(windowName, windowWidth, windowHeight);
	//window->setCamera(camera);
    glfwWindow = window->getGLFWWindow();

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // culling back face

    // loading shader
    depthMapShader = make_shared<Shader>("./shaders/shadow_mapping.vert", "./shaders/shadow_mapping.frag");
    depthMapShader->compile();

    initShadowMap();
}

void Renderer::run()
{
    // 该段以后修改（放至main()中）
    // ------------------------------------
    //shader = make_shared<Shader>("./shaders/materials.vs", "./shaders/materials.fs");
    lightCubeShader = make_shared<Shader>("./shaders/light_cube.vert", "./shaders/light_cube.frag");
    // create cube
    lightCube.bind();
    // create light
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


    //-------------------------------------------------

    addResources();
    shader->compile();
    shader->setAttrI("shadowMap", 0);
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

        // user render loop
        renderLoop();

        // render shadow map
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mat4 lightProjection, lightView;
        mat4 lightSpaceMatrix;
        for (auto light : lights)
        {
            if (light.second->getType() == LightType::Directional)
            {
                shared_ptr<DirectionalLight> dirLight = dynamic_pointer_cast<DirectionalLight>(light.second);
                lightProjection = ortho(-50.0f, 50.0f, -50.0f, 50.0f, lightNearPlane, lightFarPlane);
                lightView = lookAt(-dirLight->getDir()*vec3(30.0f), vec3(0.0f), vec3(0.0, 1.0, 0.0));
                lightSpaceMatrix = lightProjection * lightView;
            }

            glCullFace(GL_FRONT); // use cull front face to avoid peter panning
            depthMapShader->setAttrMat4("lightSpaceMatrix", lightSpaceMatrix);
            glViewport(0, 0, shadowWidth, shadowHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            for (int i = 0; i < renderObjects.size(); ++i)
                renderObjects[i]->draw(depthMapShader);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glCullFace(GL_BACK);
        }

        // set shader camera
        glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
        glm::mat4 view = camera->getViewMatrix();
        shader->setAttrVec3("viewPos", camera->position);
        //shader->setAttrMat4("projection", lightProjection);
        //shader->setAttrMat4("view", lightView);
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
        glViewport(0, 0, window->getWidth(), window->getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // set shadow map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shader->setAttrMat4("lightSpaceMatrix", lightSpaceMatrix);
        // draw objects
        for (int i=0; i<renderObjects.size(); ++i)
        {
            renderObjects[i]->draw(shaders[i]);
        }
        renderObjects.clear();
        shaders.clear();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();

        cout << "FPS: " << 1.0 / deltaTime << '\r';
    }

}

void Renderer::draw(shared_ptr<Object> object, shared_ptr<Shader> shader)
{
    renderObjects.push_back(object);
    shaders.push_back(shader);
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
    window->setCamera(camera);
}

void Renderer::captureImg(string path)
{
    string suffix = Utility::getSuffix(path);

    int width = window->getWidth();
    int height = window->getHeight();
    unsigned char* data = new unsigned char[3 * width * height];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_flip_vertically_on_write(true);

    if (suffix == "png")
        stbi_write_png(path.c_str(), width, height, 3, data, 0);
    else if(suffix == "jpg")
        stbi_write_jpg(path.c_str(), width, height, 3, data, 100);
    else if (suffix == "bmp")
        stbi_write_bmp(path.c_str(), width, height, 3, data);
    else if (suffix == "tga")
        stbi_write_tga(path.c_str(), width, height, 3, data);
    else if (suffix == "jpg")
        stbi_write_jpg(path.c_str(), width, height, 3, data, 100);
    else
    {
        cout << "Cannot save \"" << suffix << "\" format" << endl;
    }

    delete[] data;
}

void Renderer::initShadowMap()
{
    // create depth map FBO
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        this->captureImg("capture.jpg");
        cout << "Image was saved at capture.jpg" << endl;
    }
}
