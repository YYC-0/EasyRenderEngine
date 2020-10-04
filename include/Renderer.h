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
	virtual void addResources();

protected:
	virtual void renderLoop();
	void processInput();

	void addMesh(string meshName, shared_ptr<Mesh> mesh);
	void addLight(string lightName, shared_ptr<Light> light);
	void addShader(shared_ptr<Shader> shader_); // ´ýÐÞ¸Ä

private:
	shared_ptr<Window> window;
	GLFWwindow *glfwWindow;

	// timing
	float deltaTime;
	float lastFrame;

	// Resources
	shared_ptr<Camera> camera;
	map<string, shared_ptr<Mesh>> meshes;
	map<string, shared_ptr<Light>> lights;


	// will be removed (move to main())
	shared_ptr<Shader> shader;
	shared_ptr<Shader> lightCubeShader;
	//Mesh lightCube;
	Cube lightCube;
	Light pointLight;
};
