#include <iostream>
#include "../include/Renderer.h"
#include "../include/Utility.h"

Renderer::Renderer() :
    window(nullptr),
    camera(nullptr),
    glfwWindow(nullptr),
    deltaTime(0.0f),
    lastFrame(0.0f),
    clearColor(vec3(0, 0, 0)),
    dirShadowWidth(2048),
    dirShadowHeight(2048),
    pointShadowWidth(512),
    pointShadowHeight(512),
    lightNearPlane(1.0f),
    lightFarPlane(100.0f),
    dirLightNumMax(5),
    pointLightNumMax(10),
    RSMBufferSize(1024),
    gui(nullptr),
    skybox(nullptr),
    pbrShader(nullptr),
    phongShader(nullptr),
    pbrMode(false)
{
    depthMapFBOs.resize(dirLightNumMax, 0);
}


Renderer::~Renderer()
{
    for (int i = 0; i < depthMapFBOs.size(); ++i)
        if (depthMapFBOs[i] != 0)
            glDeleteFramebuffers(1, &depthMapFBOs[i]);

    glDeleteFramebuffers(1, &cubeDepthMapFBO);
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
    glDisable(GL_MULTISAMPLE); // default disable multisample anti-aliasing
    //glEnable(GL_FRAMEBUFFER_SRGB);

    shadowMap = make_shared<Texture>();
    cubeShadowMap = make_shared<Texture>();
    renderTexture = make_shared<Texture>();

    // loading shader
    depthMapShader = make_shared<Shader>("./shaders/shadow_mapping.vert", "./shaders/shadow_mapping.frag");
    cubeDepthMapShader = make_shared<Shader>("./shaders/point_shadows_depth.vert", "./shaders/point_shadows_depth.frag", "./shaders/point_shadows_depth.geom");
    phongShader = Shader::phong();
    addShader("phong", phongShader);
    screenShader = make_shared<Shader>("./shaders/texture_to_screen.vert", "./shaders/texture_to_screen.frag");
    screenShader->setTexture("screenTexture", 0, renderTexture);

    pair<unsigned int, unsigned int> bufferIDs = createFrameBuffer(windowWidth, windowHeight);
    framebuffer = bufferIDs.first;
    renderTexture->set(bufferIDs.second, TextureType::TEXTURE_2D);
    bufferIDs = createFrameBuffer(windowWidth, windowHeight);
    postProcessFB = bufferIDs.first;
    postProcessRenderTexture = bufferIDs.second;
    screenQuad.bind();

    initShadowMap();
    initCubeShadowMap();
    phongShader->setTexture("shadowMap", 0, shadowMap);
    phongShader->setTexture("cubeDepthMap", 1, cubeShadowMap);
    initialRSMBuffers();
    phongShader->setTexture("RSM_Depth", 5, RSM_depth);
    phongShader->setTexture("RSM_Position", 6, RSM_position);
    phongShader->setTexture("RSM_Normal", 7, RSM_normal);
    phongShader->setTexture("RSM_Flux", 8, RSM_flux);

    // print gl versions
    //const GLubyte *renderer = glGetString(GL_RENDERER);
    //const GLubyte *vendor = glGetString(GL_VENDOR);
    //const GLubyte *version = glGetString(GL_VERSION);
    //const GLubyte *glslVersion =
    //    glGetString(GL_SHADING_LANGUAGE_VERSION);

    //GLint major, minor;
    //glGetIntegerv(GL_MAJOR_VERSION, &major);
    //glGetIntegerv(GL_MINOR_VERSION, &minor);

    //printf("GL Vendor            : %s\n", vendor);
    //printf("GL Renderer          : %s\n", renderer);
    //printf("GL Version (string)  : %s\n", version);
    //printf("GL Version (integer) : %d.%d\n", major, minor);
    //printf("GLSL Version         : %s\n", glslVersion);
}

void Renderer::run()
{
    addResources();
    // Setup Dear ImGui context
    if (gui)
        gui->setUpContext(glfwWindow);

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
        processInput();

        // start the dear ImGui frame
        if (gui)
            gui->show();

        // render shadow map
        glEnable(GL_DEPTH_TEST);
        renderShadowMap();
        renderRSMBuffers();

        // set shader uniforms
        for (auto &shader_ : shaders)
        {
            shared_ptr<Shader>& shader = shader_.second;
            // set shader camera
            shader->setCamera(*camera);

            // set shader light
            shader->setAttrI("lightNum", lights.size());
            int n = 0;
            for (auto &light : lights)
            {
                light.second->setShaderAttr(shader, n++);
            }
        }
        
        // render scene
        // render to texture
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, window->getWidth(), window->getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0); 
        glEnable(GL_DEPTH_TEST);
        for (auto &obj : renderObjects)
            draw(obj.second);
        if (skybox) // draw skybox
            skybox->drawAsSkybox(mat4(mat3(camera->getViewMatrix())), camera->getProjectionMatrix());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // post processing
        postProcessing();
        // render texture to screen
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        screenQuad.draw(screenShader);

        if(gui)
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();

        // user render loop
        userEvents();
    }

}

void Renderer::draw(shared_ptr<Object> object, shared_ptr<Shader> shader)
{
    if(shader)
        object->draw(shader);
    else
    {
        if (pbrMode)
            object->draw(pbrShader);
        else
            object->draw(phongShader);
    }
}

void Renderer::addResources()
{
}

void Renderer::addObject(string meshName, shared_ptr<Object> mesh)
{
    auto iter = renderObjects.find(meshName);
    // mesh name already exists
    if (iter != renderObjects.end())
    {
        cout << "Add Object Failed! Object name \"" << meshName << "\" already exists!" << endl;
        return;
    }

    mesh->bind();
    renderObjects[meshName] = mesh;
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
    if (light->getType() == LightType::Directional)
        dirLights[lightName] = dynamic_pointer_cast<DirectionalLight>(light);
    else if (light->getType() == LightType::Point)
        pointLights[lightName] = dynamic_pointer_cast<PointLight>(light);
}

void Renderer::addShader(string shaderName, shared_ptr<Shader> shader_)
{
    auto iter = shaders.find(shaderName);
    // shader name already exists
    if (iter != shaders.end())
    {
        cout << "Add Shader Failed! Shader name \"" << shaderName << "\" already exists!" << endl;
        return;
    }

    shaders[shaderName] = shader_;
}

void Renderer::addSkybox(shared_ptr<CubeMap> skybox_)
{
    skybox = skybox_;
}

void Renderer::addGui(shared_ptr<Gui> gui_)
{
    gui = gui_;
}

void Renderer::addEnvironmentMap(shared_ptr<CubeMap> envMap_)
{
    envMap_->preComputeMaps();
    envMap = envMap_;

    if (!pbrShader)
        pbrShader = Shader::pbr();

    pbrShader->setTexture("irradianceMap", 7, envMap_->getIrradianceMap());
    pbrShader->setTexture("prefilterMap", 8, envMap_->getPrefilterMapID());
    pbrShader->setTexture("brdfLUT", 9, envMap_->getBrdfLUTTextureID());

}

// Add postprocessing shader by sequence
void Renderer::addPostprocessingShader(shared_ptr<Shader> shader)
{
    postProcessingShaders.push_back(shader);
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

void Renderer::setMSAA(bool b)
{
    if (b)
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);
}

void Renderer::setPBRMode(bool b)
{
    pbrMode = b;
    if (pbrMode)
    {
        auto iter = shaders.find("pbrShader");
        if (iter == shaders.end())
        {
            pbrShader = Shader::pbr();
            addShader("pbrShader", pbrShader);
        }
    }
    else
    {
        auto iter = shaders.find("pbrShader");
        if (iter != shaders.end())
            shaders.erase(iter);
    }
}

void Renderer::renderShadowMap()
{
    // render directional light shadow map
    mat4 lightProjection, lightView;
    mat4 lightSpaceMatrix;
    int dirLightNum = 0;
    glCullFace(GL_FRONT); // use cull front face to avoid peter panning
    glViewport(0, 0, dirShadowWidth, dirShadowHeight);
    for (auto &light : dirLights)
    {
        shared_ptr<DirectionalLight> dirLight = light.second;
        lightProjection = ortho(-30.0f, 30.0f, -30.0f, 30.0f, lightNearPlane, lightFarPlane);
        lightView = lookAt(-dirLight->getDir() * vec3(20.0f), vec3(0.0f), vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        depthMapShader->setAttrMat4("lightSpaceMatrix", lightSpaceMatrix);
        for (auto &shader_ : shaders)
        {
            shared_ptr<Shader>& shader = shader_.second;
            shader->setAttrMat4("lightSpaceMatrix[" + std::to_string(dirLightNum) + "]", lightSpaceMatrix);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[dirLightNum]);
        glClear(GL_DEPTH_BUFFER_BIT);
        //for (int i = 0; i < renderObjects.size(); ++i)
        //    renderObjects[i]->draw(depthMapShader);
        for (auto &object : renderObjects)
            object.second->draw(depthMapShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        dirLightNum++;
    }


    // render point light shadow map
    int pointLightNum = 0;
    glViewport(0, 0, pointShadowWidth, pointShadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, cubeDepthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    for (auto &light : pointLights)
    {
        shared_ptr<PointLight> pointLight = light.second;
        GLfloat aspect = (GLfloat)pointShadowWidth / (GLfloat)pointShadowHeight;
        float farPlane = 30.0f;
        lightProjection = perspective(radians(90.0f), aspect, 1.0f, farPlane);
        vector<mat4> shadowTransforms;
        shadowTransforms.push_back(lightProjection * lookAt(pointLight->getPos(), pointLight->getPos() + vec3(1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(lightProjection * lookAt(pointLight->getPos(), pointLight->getPos() + vec3(-1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(lightProjection * lookAt(pointLight->getPos(), pointLight->getPos() + vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(lightProjection * lookAt(pointLight->getPos(), pointLight->getPos() + vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(lightProjection * lookAt(pointLight->getPos(), pointLight->getPos() + vec3(0.0, 0.0, 1.0), vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(lightProjection * lookAt(pointLight->getPos(), pointLight->getPos() + vec3(0.0, 0.0, -1.0), vec3(0.0, -1.0, 0.0)));
        for (int i = 0; i < 6; ++i)
            cubeDepthMapShader->setAttrMat4("shadowMatrices[" + to_string(i) + "]", shadowTransforms[i]);
        cubeDepthMapShader->setAttrF("far_plane", farPlane);
        cubeDepthMapShader->setAttrVec3("lightPos", pointLight->getPos());
        cubeDepthMapShader->setAttrI("lightNum", pointLightNum);
        for (auto &shader_ : shaders)
        {
            shared_ptr<Shader>& shader = shader_.second;
            shader->setAttrF("far_plane", farPlane);
        }

        //for (int i = 0; i < renderObjects.size(); ++i)
        //    renderObjects[i]->draw(cubeDepthMapShader);
        for (auto &object : renderObjects)
            object.second->draw(cubeDepthMapShader);

        pointLightNum++;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);

    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeDepthMap);
}

void Renderer::initShadowMap()
{
    // create depth map FBO
    for (int i = 0; i < dirLightNumMax; ++i)
        glGenFramebuffers(1, &depthMapFBOs[i]);

    // create depth texture
    GLuint depthMaps;
    glGenTextures(1, &depthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMaps);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT,
        dirShadowWidth, dirShadowHeight, dirLightNumMax, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    for (int i = 0; i < dirLightNumMax; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[i]);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_ARRAY, depthMap, 0);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMaps, 0, i);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    shadowMap->set(depthMaps, TextureType::TEXTURE_2D_ARRAY);
}

void Renderer::initCubeShadowMap()
{
    // using cube map
    glGenFramebuffers(1, &cubeDepthMapFBO);

    GLuint cubeDepthMap;
    glGenTextures(1, &cubeDepthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeDepthMap);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT,
        pointShadowWidth, pointShadowHeight, 6*pointLightNumMax, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    //for (GLuint i = 0; i < 6 * pointLightNumMax; ++i)
    //    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
    //        shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, cubeDepthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeDepthMap, 0);
    //glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeDepthMap, 0, 1);
    //glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubeDepthMap, 0, i);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Point light shadow framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    cubeShadowMap->set(cubeDepthMap, TextureType::TEXTURE_CUBE_MAP_ARRAY);
}

void Renderer::drawSkybox()
{

}

// Create a framebuffer
// return framebuffer and color attachment texture
pair<unsigned int, unsigned int> Renderer::createFrameBuffer(int width, int height)
{
    unsigned int fb;
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    // create a color attachment texture
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    // create a renderbuffer object for depth and stencil attachment
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return {fb, texColorBuffer};
}

// Post processing
void Renderer::postProcessing()
{
    glDisable(GL_DEPTH_TEST);

    for (auto &shader : postProcessingShaders)
    {
        // copy renderTexture to postProcessRenderTexture
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postProcessFB);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, window->getWidth(), window->getHeight());

        glBindTexture(GL_TEXTURE_2D, postProcessRenderTexture);
        glClear(GL_COLOR_BUFFER_BIT);
        screenQuad.draw(shader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Renderer::initialRSMBuffers()
{
    glGenFramebuffers(1, &RSMBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, RSMBuffer);
    // bind four textures. depth, world space coordinates, normal, flux
    GLuint depth, pos, normal, flux;
    // depth
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, RSMBufferSize, RSMBufferSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
    // position
    glGenTextures(1, &pos);
    glBindTexture(GL_TEXTURE_2D, pos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RSMBufferSize, RSMBufferSize, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pos, 0);
    // normal
    glGenTextures(1, &normal);
    glBindTexture(GL_TEXTURE_2D, normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RSMBufferSize, RSMBufferSize, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);
    // flux
    glGenTextures(1, &flux);
    glBindTexture(GL_TEXTURE_2D, flux);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RSMBufferSize, RSMBufferSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, flux, 0);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    RSM_depth = make_shared<Texture>(depth, TextureType::TEXTURE_2D);
    RSM_position = make_shared<Texture>(pos, TextureType::TEXTURE_2D);
    RSM_normal = make_shared<Texture>(normal, TextureType::TEXTURE_2D);
    RSM_flux = make_shared<Texture>(flux, TextureType::TEXTURE_2D);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    RSMBufferShader = make_shared<Shader>("./shaders/RSM_buffer.vert", "./shaders/RSM_buffer.frag");
}

void Renderer::renderRSMBuffers()
{
    shared_ptr<DirectionalLight> dirLight = dirLights.begin()->second;
    mat4 lightProjection = ortho(-30.0f, 30.0f, -30.0f, 30.0f, lightNearPlane, lightFarPlane);
    mat4 lightView = lookAt(-dirLight->getDir() * vec3(25.0f), vec3(0.0f), vec3(0.0, 1.0, 0.0));
    mat4 lightSpaceMatrix = lightProjection * lightView;

    RSMBufferShader->setAttrMat4("lightSpaceMatrix", lightSpaceMatrix);
    for (auto &shader_ : shaders)
    {
        shared_ptr<Shader> &shader = shader_.second;
        shader->setAttrMat4("RSM_lightSpaceMatrix", lightSpaceMatrix);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, RSMBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, RSMBufferSize, RSMBufferSize);
    for (auto &object : renderObjects)
        object.second->draw(RSMBufferShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::userEvents()
{
}

void Renderer::processInput()
{
    if (window->getCursorState() == false)
    {
        if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
            camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
        if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
            camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
        if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
            camera->processKeyboard(CameraMovement::LEFT, deltaTime);
        if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
            camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
    }
}
