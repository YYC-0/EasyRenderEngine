#pragma once
#include <string>

#include "Camera.h"
#include "Shader.h"
#include "Window.h"

using namespace std;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void init(string windowName, int windowWidth, int windowHeight, Camera *camera_);
	void run();

private:
	Window *window;
	GLFWwindow *glfwWindow;
	Camera *camera;

	// timing
	float deltaTime;
	float lastFrame;

	void renderLoop();
	void processInput();

};
