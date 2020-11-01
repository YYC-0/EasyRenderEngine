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

	void draw(shared_ptr<Object> object, shared_ptr<Shader> shader);

	void addObject(string meshName, shared_ptr<Object> mesh);
	void addLight(string lightName, shared_ptr<Light> light);
	void addShader(shared_ptr<Shader> shader_); // ´ýÐÞ¸Ä

	void setClearColor(vec3 color);
	void setCamera(shared_ptr<Camera> camera_);

	void captureImg(string path);

	shared_ptr<Shader> depthMapShader;

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

	// shadow
	GLuint depthMapFBO;
	GLuint depthMap;
	GLuint shadowWidth;
	GLuint shadowHeight;
	GLfloat lightNearPlane;
	GLfloat lightFarPlane;
	void initShadowMap();

	// render object
	vector<shared_ptr<Object>> renderObjects;
	vector<shared_ptr<Shader>> shaders;

	// will be removed (move to main())
	shared_ptr<Shader> shader;
	shared_ptr<Shader> lightCubeShader;
	//Object lightCube;
	Cube lightCube;
};
