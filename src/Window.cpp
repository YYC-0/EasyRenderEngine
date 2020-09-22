#include "../include/Window.h"
#include <iostream>

Window::Window()
{
    camera = nullptr;
    windowWidth = 800;
    windowHeight = 600;
    float lastX = windowWidth / 2.0f;
    float lastY = windowHeight / 2.0f;
    firstMouse = true;

    glInit();
}

Window::Window(std::string windowName, int width, int height)
{
    this->windowName = windowName;
    windowWidth = width;
    windowHeight = height;
    float lastX = windowWidth / 2.0f;
    float lastY = windowHeight / 2.0f;
    firstMouse = true;

    glInit();
}

Window::~Window()
{
    glfwTerminate();
}

void Window::framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->windowWidth = width;
    win->windowHeight = height;

    glViewport(0, 0, width, height);
}

void Window::mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (win->camera)
    {
        if (win->firstMouse)
        {
            win->lastX = xpos;
            win->lastY = ypos;
            win->firstMouse = false;
        }

        float xoffset = xpos - win->lastX;
        float yoffset = win->lastY - ypos; // reversed since y-coordinates go from bottom to top

        win->lastX = xpos;
        win->lastY = ypos;

        win->camera->processMouseMovement(xoffset, yoffset);
    }
}

void Window::scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win->camera)
    {
        win->camera->processMouseScroll(yoffset);
    }
}

bool Window::glInit()
{    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), NULL, NULL);
    if (glfwWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwSetWindowUserPointer(glfwWindow, this);

    glfwMakeContextCurrent(glfwWindow);
    glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(glfwWindow, mouse_callback);
    glfwSetScrollCallback(glfwWindow, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
}
