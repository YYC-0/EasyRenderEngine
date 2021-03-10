#include "../include/Gui.h"
#include <iostream>

Gui::Gui()
{
	window = nullptr;
}

Gui::~Gui()
{
}

void Gui::add(string name, shared_ptr<Object> obj)
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

void Gui::add(string name, shared_ptr<Light> light)
{
    auto iter = lights.find(name);
    // object name already exists
    if (iter != lights.end())
    {
        cout << "Add Light GUI Failed! Light name \"" << name << "\" already exists!" << endl;
        return;
    }

    lights[name] = light;
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

void Gui::add(string name, shared_ptr<Material> mtl)
{
    auto iter = lights.find(name);
    // object name already exists
    if (iter != lights.end())
    {
        cout << "Add Light GUI Failed! Material name \"" << name << "\" already exists!" << endl;
        return;
    }

    materials[name] = mtl;
    names.push_back(name);
    cNames = new char *[names.size()];
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

    auto iter = objects.find(names[item]);
    if (iter != objects.end())
        objectGui(names[item]);
    else
    {
        auto iter2 = lights.find(names[item]);
        if (iter2 != lights.end())
            lightGui(names[item]);
        else
        {
            auto iter3 = materials.find(names[item]);
            if (iter3 != materials.end())
                mtlGUI(names[item]);
        }
    }

    // show FPS on the bottom
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 20);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 110);
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

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

void Gui::lightGui(string lightName)
{
    shared_ptr<Light> light = lights[lightName];
    vec3 color = light->getColor();
    float c[3] = { color.r, color.g, color.b };
    ImGui::Text("Color:");
    ImGui::ColorEdit3("Light Color", c);
    light->setColor(vec3(c[0], c[1], c[2]));

    if (light->getType() == LightType::Directional)
    {
        shared_ptr<DirectionalLight> dirL = dynamic_pointer_cast<DirectionalLight>(light);
        vec3 dir = dirL->getDir();
        ImGui::Text("Direction:");
        ImGui::PushItemWidth(70);
        ImGui::DragFloat("X ##lightDir", &dir.x, 0.1, 0, 0, "%.2f", 1); ImGui::SameLine();
        ImGui::DragFloat("Y ##lightDir", &dir.y, 0.1, 0, 0, "%.2f", 1); ImGui::SameLine();
        ImGui::DragFloat("Z ##lightDir", &dir.z, 0.1, 0, 0, "%.2f", 1);
        ImGui::PopItemWidth();

        dirL->setDir(dir);
    }
    else if (light->getType() == LightType::Point)
    {
        shared_ptr<PointLight> pointL = dynamic_pointer_cast<PointLight>(light);
        vec3 pos = pointL->getPos();
        ImGui::Text("Position:");
        ImGui::PushItemWidth(70);
        ImGui::DragFloat("X ##lightPos", &pos.x, 0.1, 0, 0, "%.2f", 1); ImGui::SameLine();
        ImGui::DragFloat("Y ##lightPos", &pos.y, 0.1, 0, 0, "%.2f", 1); ImGui::SameLine();
        ImGui::DragFloat("Z ##lightPos", &pos.z, 0.1, 0, 0, "%.2f", 1);
        ImGui::PopItemWidth();
        pointL->setPosition(pos);

        ImGui::Text("Attenuation Coefficient:");
        ImGui::PushItemWidth(70);
        ImGui::DragFloat("constant", &pointL->constant, 0.01, 0, 0, "%.2f", 1); ImGui::SameLine();
        ImGui::DragFloat("linear", &pointL->linear, 0.001, 0, 0, "%.3f", 1); ImGui::SameLine();
        ImGui::DragFloat("quadratic", &pointL->quadratic, 0.001, 0, 0, "%.3f", 1);
        ImGui::PopItemWidth();
    }
}

void Gui::mtlGUI(string mtlName)
{
    shared_ptr<Material> mtl = materials[mtlName];
    // PBR material
    if (mtl->usePBR)
    {
        shared_ptr<PBRMaterial> pbrMtl = dynamic_pointer_cast<PBRMaterial>(mtl);
        ImVec2 imgSize(50, 50);
        // albedo
        if (pbrMtl->useAlbedoMap)
        {
            // Textures
            ImGui::Text("Albedo:    "); ImGui::SameLine();
            ImGui::Image((void *)(intptr_t)pbrMtl->albedoMap.getID(), imgSize);
        }
        else
        {
            vec3 albedo = pbrMtl->albedo;
            float c[3] = { albedo.r, albedo.g, albedo.b };
            ImGui::Text("Albedo:");
            ImGui::ColorEdit3("", c);
            pbrMtl->albedo = vec3(c[0], c[1], c[2]);
        }
        // metallic
        if (pbrMtl->useMetallicMap)
        {
            ImGui::Text("Metallic:  "); ImGui::SameLine();
            ImGui::Image((void *)(intptr_t)pbrMtl->metallicMap.getID(), imgSize);
        }
        else
        {
            ImGui::Text("Metallic:");
            ImGui::SliderFloat("Metallic", &pbrMtl->metallic, 0.0f, 1.0f);
        }
        // roughness
        if (pbrMtl->useRoughnessMap)
        {
            ImGui::Text("Roughness: "); ImGui::SameLine();
            ImGui::Image((void *)(intptr_t)pbrMtl->roughnessMap.getID(), imgSize);
        }
        else
        {
            ImGui::Text("Roughness:");
            ImGui::SliderFloat("Roughness", &pbrMtl->roughness, 0.0f, 1.0f);
        }
        if (pbrMtl->useNormalMap)
        {
            ImGui::Text("Normal Map:"); ImGui::SameLine();
            ImGui::Image((void *)(intptr_t)pbrMtl->normalMap.getID(), imgSize);
        }
        if (pbrMtl->useAoMap)
        {
            ImGui::Text("Ao Map:    "); ImGui::SameLine();
            ImGui::Image((void *)(intptr_t)pbrMtl->aoMap.getID(), imgSize);
        }
    }
    else
    {
        ImVec2 imgSize(50, 50);
        // ambient
        vec3 ambient = mtl->ambient;
        float c[3] = { ambient.r, ambient.g, ambient.b };
        ImGui::Text("Ambient:");
        ImGui::ColorEdit3("", c);
        mtl->ambient = vec3(c[0], c[1], c[2]);
        // diffuse
        if (mtl->useDiffuseMap)
        {
            // Textures
            ImGui::Text("Diffuse:    "); ImGui::SameLine();
            ImGui::Image((void *)(intptr_t)mtl->diffuseMap.getID(), imgSize);
        }
        else
        {
            vec3 diffuse = mtl->diffuse;
            float c[3] = { diffuse.r, diffuse.g, diffuse.b };
            ImGui::Text("Diffuse:");
            ImGui::ColorEdit3("", c);
            mtl->diffuse = vec3(c[0], c[1], c[2]);
        }
        // specular
        if (mtl->useSpecularMap)
        {
            ImGui::Text("Specular: "); ImGui::SameLine();
            ImGui::Image((void *)(intptr_t)mtl->specularMap.getID(), imgSize);
        }
        else
        {
            vec3 specular = mtl->specular;
            float c[3] = { specular.r, specular.g, specular.b };
            ImGui::Text("Specular:");
            ImGui::ColorEdit3("", c);
            mtl->specular = vec3(c[0], c[1], c[2]);
        }
        // normal
        if (mtl->useNormalMap)
        {
            ImGui::Text("Normal:  "); ImGui::SameLine();
            ImGui::Image((void *)(intptr_t)mtl->normalMap.getID(), imgSize);
        }

    }
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
