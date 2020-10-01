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

	void init(string windowName, int windowWidth, int windowHeight, shared_ptr<Camera> camera_);
	void run();

private:
	shared_ptr<Window> window;
	GLFWwindow *glfwWindow;
	shared_ptr<Camera> camera;

	// timing
	float deltaTime;
	float lastFrame;

	void renderLoop();
	void processInput();

	// will be removed (move to main())
	shared_ptr<Shader> lightingShader;
	shared_ptr<Shader> lightCubeShader;
	Mesh lightCube;
	Cube cube;
	Light pointLight;
};
