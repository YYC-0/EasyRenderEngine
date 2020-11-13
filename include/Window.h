#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include "Camera.h"

class Window
{
public:
	Window();
	Window(std::string windowName, int width, int height);
	~Window();

	GLFWwindow* getGLFWWindow() { return glfwWindow; }
	void setCamera(std::shared_ptr<Camera> camera) { this->camera = camera; }
	int getWidth() { return windowWidth; }
	int getHeight() { return windowHeight; }
	bool getCursorState() { return enableCursor; }
	void setCursor(bool b); // set if cursor is enable

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
	bool enableCursor;

	// for mouse and scroll event
	std::shared_ptr<Camera> camera;
	float lastX;
	float lastY;
	bool firstMouse;
};
