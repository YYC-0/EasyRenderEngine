#include "../include/Window.h"
#include "../include/Utility.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"
#include <iostream>

Window::Window()
{
    camera = nullptr;
    windowWidth = 800;
    windowHeight = 600;
    float lastX = windowWidth / 2.0f;
    float lastY = windowHeight / 2.0f;
    firstMouse = true;
    enableCursor = true;

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
    enableCursor = true;

    glInit();
}

Window::~Window()
{
    glfwTerminate();
}

void Window::setCursor(bool b)
{
    enableCursor = b;
    if (b)
    {
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
    }
    else
    {
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Window::captureImg(string path)
{
    string suffix = Utility::getSuffix(path);

    unsigned char* data = new unsigned char[3 * windowWidth * windowHeight];
    glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_flip_vertically_on_write(true);

    if (suffix == "png")
        stbi_write_png(path.c_str(), windowWidth, windowHeight, 3, data, 0);
    else if (suffix == "jpg")
        stbi_write_jpg(path.c_str(), windowWidth, windowHeight, 3, data, 100);
    else if (suffix == "bmp")
        stbi_write_bmp(path.c_str(), windowWidth, windowHeight, 3, data);
    else if (suffix == "tga")
        stbi_write_tga(path.c_str(), windowWidth, windowHeight, 3, data);
    else if (suffix == "jpg")
        stbi_write_jpg(path.c_str(), windowWidth, windowHeight, 3, data, 100);
    else
    {
        cout << "Cannot save \"" << suffix << "\" format" << endl;
        delete[] data;
        return;
    }

    cout << "Image was saved at capture.jpg" << endl;
    delete[] data;
}

void Window::framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->windowWidth = width;
    win->windowHeight = height;

    glViewport(0, 0, width, height);
}

void Window::CursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win->enableCursor)
        return;

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

void Window::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win->camera)
    {
        win->camera->processMouseScroll(yoffset);
    }
}

void Window::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
            win->setCursor(false);
        else if (action == GLFW_RELEASE)
            win->setCursor(true);
    }
}

void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Window *win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    switch (key)
    {
    case GLFW_KEY_SPACE:
        if(action == GLFW_PRESS)
            win->captureImg("capture.jpg");
    case GLFW_KEY_ESCAPE:
        if(action == GLFW_PRESS)
            glfwSetWindowShouldClose(win->glfwWindow, true);
    default:
        break;
    }
}

bool Window::glInit()
{    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

    // set callback function
    glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(glfwWindow, CursorPosCallback);
    glfwSetScrollCallback(glfwWindow, scroll_callback);
    glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
    glfwSetKeyCallback(glfwWindow, keyCallback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSwapInterval(0); // disable vertical sync
}
