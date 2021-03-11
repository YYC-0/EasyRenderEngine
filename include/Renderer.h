#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Shader.h"
#include "Window.h"
#include "Mesh.h"
#include "Light.h"
#include "CubeMap.h"
#include "Gui.h"

using namespace std;

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	void init(string windowName, int windowWidth, int windowHeight);
	void run();
	virtual void addResources();

	virtual void renderLoop();
	virtual void processInput();

	void draw(shared_ptr<Object> object, shared_ptr<Shader> shader = nullptr);

	void addObject(string meshName, shared_ptr<Object> mesh);
	void addLight(string lightName, shared_ptr<Light> light);
	void addShader(string shaderName, shared_ptr<Shader> shader_);
	void addSkybox(shared_ptr<CubeMap> skybox_);
	void addGui(shared_ptr<Gui> gui_);
	void addEnvironmentMap(shared_ptr<CubeMap> envMap_);

	void setClearColor(vec3 color);
	void setCamera(shared_ptr<Camera> camera_);
	void setMSAA(bool b);
	void setPBRMode(bool b);

private:
	shared_ptr<Window> window;
	GLFWwindow *glfwWindow;

	// timing
	float deltaTime;
	float lastFrame;

	// Resources
	shared_ptr<Camera> camera;
	map<string, shared_ptr<Object>> renderObjects;
	map<string, shared_ptr<Light>> lights;
	map<string, shared_ptr<DirectionalLight>> dirLights;
	map<string, shared_ptr<PointLight>> pointLights;
	map<string, shared_ptr<Shader>> shaders;

	// shaders
	shared_ptr<Shader> pbrShader;
	shared_ptr<Shader> phongShader;
	shared_ptr<Shader> screenShader;

	vec3 clearColor;

	// shadow
	void renderShadowMap();
	void initShadowMap();	// directional light
	void initCubeShadowMap();	// point light
	int pointLightNumMax;
	int dirLightNumMax;
	shared_ptr<Shader> depthMapShader;
	shared_ptr<Shader> cubeDepthMapShader;
	vector<GLuint> depthMapFBOs;
	GLuint cubeDepthMapFBO;
	GLuint depthMaps;	 // directional light shadow array texture
	GLuint cubeDepthMap; // point light shadow array texture
	GLuint dirShadowWidth;
	GLuint dirShadowHeight;
	GLuint pointShadowWidth;
	GLuint pointShadowHeight;
	GLfloat lightNearPlane;
	GLfloat lightFarPlane;

	// Skybox
	shared_ptr<CubeMap> skybox;
	void drawSkybox();

	// Environment map
	shared_ptr<CubeMap> envMap;

	// GUI
	shared_ptr<Gui> gui;

	// Mode
	bool pbrMode;

	// Post process
	Rectangle screenQuad;
	unsigned int framebuffer;
	unsigned int renderTexture;
	unsigned int postProcessFB;
	unsigned int postProcessRenderTexture;
	pair<unsigned int, unsigned int> createFrameBuffer(int width, int height);
	unsigned int postProcessing();
};
