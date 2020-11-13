#include "../include/Gui.h"
#include <iostream>

Gui::Gui()
{
	window = nullptr;
}

Gui::~Gui()
{
}

void Gui::addObject(string name, shared_ptr<Object> obj)
{
    auto iter = objects.find(name);
    // object name already exists
    if (iter != objects.end())
    {
        cout << "Add Object GUI Failed! Object name \"" << name << "\" already exists!" << endl;
        return;
    }

    objects[name] = obj;
    names.push_back(name);
    cNames = new char*[names.size()];
    for (int i = 0; i < names.size(); ++i)
    {
        cNames[i] = new char[names[i].size() + 1];
        for (int j = 0; j < names[i].size(); ++j)
            cNames[i][j] = names[i][j];
        cNames[i][names[i].size()] = '\0';
    }
}

void Gui::setUpContext(GLFWwindow *window_)
{
    window = window_;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(NULL);
}

void Gui::show()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Controller");

    static int item = 0;
    ImGui::Combo("Object", &item, cNames, names.size(), names.size());

    objectGui(names[item]);

    ImGui::End();

    ImGui::Render();
}

void Gui::objectGui(string objectName)
{
    shared_ptr<Object> obj = objects[objectName];
    vec3 pos = obj->getPosition();
    vec3 scale = obj->getScale();
    vector<float> rotation = obj->getRotation();

    ImGui::Text("Position:");
    ImGui::PushItemWidth(70);
    ImGui::DragFloat("X ##pos", &pos.x, 0.1, 0, 0, "%.2f", 1); ImGui::SameLine();
    ImGui::DragFloat("Y ##pos", &pos.y, 0.1, 0, 0, "%.2f", 1); ImGui::SameLine();
    ImGui::DragFloat("Z ##pos", &pos.z, 0.1, 0, 0, "%.2f", 1);
    ImGui::PopItemWidth();

    ImGui::Text("Scale:");
    ImGui::PushItemWidth(70);
    ImGui::DragFloat("X ##scale", &scale.x, 0.1, 0, 0, "%.2f", 1); ImGui::SameLine();
    ImGui::DragFloat("Y ##scale", &scale.y, 0.1, 0, 0, "%.2f", 1); ImGui::SameLine();
    ImGui::DragFloat("Z ##scale", &scale.z, 0.1, 0, 0, "%.2f", 1);
    ImGui::PopItemWidth();

    ImGui::Text("Rotation:");
    ImGui::PushItemWidth(70);
    ImGui::DragFloat("X ##rotation", &rotation[0], 1, 0, 0, "%.2f", 1); ImGui::SameLine();
    ImGui::DragFloat("Y ##rotation", &rotation[1], 1, 0, 0, "%.2f", 1); ImGui::SameLine();
    ImGui::DragFloat("Z ##rotation", &rotation[2], 1, 0, 0, "%.2f", 1);
    ImGui::PopItemWidth();

    obj->setTransform(pos, scale, rotation);
}

void Gui::helpMarker(std::string desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
