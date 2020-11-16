#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include "Camera.h"
using namespace std;

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
	void captureImg(string path);	// capture current frame

	// Event Callback Functions
	// whenever the window size changed(by OS or user resize) this callback function executes
	static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
	// whenever the mouse moves, this callback is called
	static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);
	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
	// mouse button click event
	static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	// key event
	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

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
