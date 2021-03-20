#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <map>
#include <memory>
#include "Window.h"
#include "Light.h"
#include "Mesh.h"
#include "Material.h"
using namespace std;

class Renderer;

class Gui
{
public:
	Gui();
	~Gui();
	void add(string name, shared_ptr<Object> obj);
	void add(string name, shared_ptr<Light> light);
	void add(string name, shared_ptr<Material> mtl);

private:
	friend class Renderer;
	GLFWwindow *window;
	map<string, shared_ptr<Object>> objects;
	map<string, shared_ptr<Light>> lights;
	map<string, shared_ptr<Material>> materials;
	vector<string> names;
	char** cNames;

	void setUpContext(GLFWwindow *window_);
	void show();
	void objectGui(string objectName);
	void lightGui(string lightName);
	void mtlGUI(string mtlName);

	void helpMarker(std::string desc);
};
