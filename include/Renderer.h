#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Window.h"
#include "Mesh.h"
#include "Light.h"

using namespace std;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void init(string windowName, int windowWidth, int windowHeight);
	void run();
	virtual void addResources();

	virtual void renderLoop();
	virtual void processInput();

	void addObject(string meshName, shared_ptr<Object> mesh);
	void addLight(string lightName, shared_ptr<Light> light);
	void addShader(shared_ptr<Shader> shader_); // ´ýÐÞ¸Ä

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
	map<string, shared_ptr<Object>> meshes;
	map<string, shared_ptr<Light>> lights;

	vec3 clearColor;

	// will be removed (move to main())
	shared_ptr<Shader> shader;
	shared_ptr<Shader> lightCubeShader;
	//Object lightCube;
	Cube lightCube;
};
