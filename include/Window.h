#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Camera.h"

class Window
{
public:
	Window();
	Window(std::string windowName, int width, int height);
	~Window();

	GLFWwindow* getGLFWWindow() { return glfwWindow; }
	void setCamera(Camera *camera) { this->camera = camera; }

	// whenever the window size changed(by OS or user resize) this callback function executes
	static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

	// whenever the mouse moves, this callback is called
	static void mouse_callback(GLFWwindow *window, double xpos, double ypos);

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

private:
	bool glInit();

	GLFWwindow *glfwWindow;
	int windowWidth;
	int windowHeight;
	std::string windowName;

	// for mouse and scroll event
	Camera *camera;
	float lastX;
	float lastY;
	bool firstMouse;
};
