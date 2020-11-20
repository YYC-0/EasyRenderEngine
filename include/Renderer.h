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
#include "Skybox.h"
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

	void draw(shared_ptr<Object> object, shared_ptr<Shader> shader);

	void addObject(string meshName, shared_ptr<Object> mesh);
	void addLight(string lightName, shared_ptr<Light> light);
	void addShader(string shaderName, shared_ptr<Shader> shader_);
	void addSkybox(shared_ptr<Skybox> skybox_);
	void addGui(shared_ptr<Gui> gui_);

	void setClearColor(vec3 color);
	void setCamera(shared_ptr<Camera> camera_);


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
	shared_ptr<Skybox> skybox;

	// GUI
	shared_ptr<Gui> gui;

};
