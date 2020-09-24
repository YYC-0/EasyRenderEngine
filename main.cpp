#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/Shader.h"
#include "include/Camera.h"
#include "include/Window.h"
#include "include/Renderer.h"

#include <iostream>
#include <memory>
using namespace std;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

class myRenderer : public Renderer
{
public:
    myRenderer()
    {
        camera = make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
        init("test", SCR_WIDTH, SCR_HEIGHT, camera);
    }

    shared_ptr<Camera> camera;
};

int main()
{
    myRenderer r;
    r.run();

    return 0;
}
