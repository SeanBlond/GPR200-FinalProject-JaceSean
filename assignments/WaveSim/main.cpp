#include <iostream>
#include <stdio.h>
#include <math.h>

// IMGUI Stuff
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/external/stb_image.h>
#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <object/object.h>
#include <Camera/camera.h>
#include <shader/shader.h>
#include <math/smath.h>
#include <Wave/waves.h>


// Screen Info
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

// Time Managers
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Function Prototypes
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Mouse Management
bool firstMouse = true;
glm::vec2 previousMousePos = glm::vec2(float(SCREEN_WIDTH) / 2.0, float(SCREEN_HEIGHT) / 2.0);
glm::vec2 mouseDelta = glm::vec2(0.0f);

// Creating Camera
FlyCamera camera(glm::vec3(0.0f, 1.0f, 4.0f), 60.0f);

// Rendering Options
const char* renderingOptions[] = {
    "UVs",
    "Normals",
    "Shaded"
};
const char* renderingOptionIndex = renderingOptions[2];
int selectedRenderOption = 2;
bool renderLines = false;
bool renderPoints = false;

int obj::Object::nextID = 0;

int main() 
{
    printf("Initializing...\n");
    if (!glfwInit()) {
        printf("GLFW failed to init!");
        return 1;
    }
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
    if (window == NULL) {
        printf("GLFW failed to create window");
        return 1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("GLAD Failed to load GL headers");
        return 1;
    }

    // Callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Setting Up ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();


    //Initialization goes here!
    glEnable(GL_DEPTH_TEST);

    // Creating Shader(s)
    shdr::Shader waveShader("assets/shaders/waveVert.glsl", "assets/shaders/cubeFrag.glsl");
    shdr::Shader lightShader("assets/shaders/cubeVert.glsl", "assets/shaders/lightFrag.glsl");

    // Creating the Rendered Object
    obj::Object wavePlane("ocean", mesh::createPlane(8.0f, 8.0f, 64), &waveShader, glm::vec3(0));

    // Creating pointers to the primitive's settings
    PlaneMesh* wavePlanePtr = dynamic_cast<PlaneMesh*>(wavePlane.getMesh());

    // Wave Settings
    hiWave::WaveSystem wavez;
    wavez.AddRandomWave();

    // Objects Material Settings
    float ambient = 0.5f;
    float diffuse = 0.5f;
    float specular = 0.5f;
    float shininess = 4.0f;

    // Creating the light
    obj::Object light("light", mesh::createSphere(0.1f, 8), &lightShader);

    // Light Properties
    glm::vec3 lightColor = glm::vec3(1);
    glm::vec3 lightPosition = glm::vec3(0.0f, 2.0f, 0.0f);

    // Creating an Image Texture
    shdr::Texture2D pattern("assets/lobster-pattern.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
    shdr::Texture2D earth("assets/earth-map.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);



    // Projection Matrix
    glm::mat4 projection = smath::perspective(camera.getFOV(), ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT), 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();

    //Render loop
    while (!glfwWindowShouldClose(window)) {
        // Managing Inputs
        processInput(window);

        //Clear framebuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Time Management
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Perspective/View Transformation Matrices
        if (camera.getDoPerspective())
            projection = smath::perspective(camera.getFOV(), ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT), 0.1f, 100.0f);
        else
        {
            float scale = 4.0f;
            float tempZoom = scale * (121.0f - camera.getFOV());
            projection = smath::orthographic(-SCREEN_WIDTH / tempZoom,
                                            SCREEN_WIDTH / tempZoom,
                                            -SCREEN_HEIGHT / tempZoom,
                                            SCREEN_HEIGHT / tempZoom,
                                            -100.0f, 100.0f);
        }

        view = camera.getViewMatrix();

        // Wave Settings Management
        waveShader.useShader();
        waveShader.setInt("texture1", 0);
        waveShader.setFloat("time", glfwGetTime());
        wavez.PassValues(&waveShader);

        // Drawing the Light
        light.shader->useShader();
        light.shader->setMat4("projection", projection);
        light.shader->setMat4("view", view);
        light.shader->setVec3("color", lightColor);
        light.transform.position = lightPosition;
        light.DrawMesh(renderLines, renderPoints);

        // Mesh Settings
        waveShader.useShader();
        waveShader.setMat4("projection", projection);
        waveShader.setMat4("view", view);
        waveShader.setInt("renderOption", selectedRenderOption);

        // Light Settings
        waveShader.setVec3("viewPos", camera.getPosition());
        waveShader.setVec3("light.lightPos", light.transform.position);
        waveShader.setVec3("light.lightColor", lightColor);

        // Cube Materials
        waveShader.setFloat("material.diffuse", diffuse);
        waveShader.setFloat("material.ambient", ambient);
        waveShader.setFloat("material.specular", specular);
        waveShader.setFloat("material.shininess", shininess);

        // Drawing the Meshes
        pattern.Bind(0);
        wavePlane.DrawMesh(renderLines, renderPoints);

        // ImGUI Rendering
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // Creating Settings Window
        ImGui::Begin("Settings");

        // Render Settings
        if (ImGui::CollapsingHeader("Render Settings"))
        {
            // Rendering Mode
            if (ImGui::BeginCombo("Rendering Mode", renderingOptionIndex, ImGuiComboFlags_NoArrowButton))
            {
                for (int n = 0; n < IM_ARRAYSIZE(renderingOptions); n++)
                {
                    bool is_selected = (renderingOptionIndex == renderingOptions[n]);
                    if (ImGui::Selectable(renderingOptions[n], is_selected))
                    {
                        renderingOptionIndex = renderingOptions[n];
                        selectedRenderOption = n;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            // Mesh Rendering Options
            ImGui::Checkbox("Render Lines", &renderLines);
            ImGui::Checkbox("Render Points", &renderPoints);
        }

        // Light Settings
        if (ImGui::CollapsingHeader("Light Settings"))
        {
            ImGui::DragFloat3("Light Position", &lightPosition.x, 0.1f);
            ImGui::ColorEdit3("Light Color", &lightColor.r);
        }

        // Material Settings
        if (ImGui::CollapsingHeader("Material Settings"))
        {
            ImGui::DragFloat("Ambient", &ambient, 0.1f, 0.0f, 1.0f);
            ImGui::DragFloat("Diffuse", &diffuse, 0.1f, 0.0f, 1.0f);
            ImGui::DragFloat("Specular", &specular, 0.1f, 0.0f, 1.0f);
            ImGui::DragFloat("Shininess", &shininess, 1.0f, 2.0f, 1024.0f);
        }

        // Plane Settings
        if (ImGui::CollapsingHeader("Wave Settings"))
        {
            ImGui::DragFloat("Width", &(wavePlanePtr->width), 0.1f, 0.0f);
            ImGui::DragFloat("Height", &(wavePlanePtr->height), 0.1f, 0.0f);
            ImGui::DragInt("Subdivisions", &(wavePlanePtr->subdivisions), 1, 1, 32);
            ImGui::Text("\nWave Vertex Shader");
        }

        ImGui::End();

        // Rendering ImGUI Window
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // De-Allocating Resources
    waveShader.deallocateShader();
    lightShader.deallocateShader();

    printf("Shutting down...");
}

// Keyboard Inputs
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // Camera Key Input
    camera.processKeyInput(window, deltaTime);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        previousMousePos.x = xpos;
        previousMousePos.y = ypos;
        firstMouse = false;
    }

    mouseDelta.x = xpos - previousMousePos.x;
    mouseDelta.y = previousMousePos.y - ypos; // reversed since y-coordinates go from bottom to top
    previousMousePos.x = xpos;
    previousMousePos.y = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    mouseDelta.x *= sensitivity;
    mouseDelta.y *= sensitivity;

    // Changing Camera Only if mouse button is down
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
    {
        camera.processMouseMovement(mouseDelta);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Locks
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //Unlocks
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.addFOV(-2.0f * yoffset);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}