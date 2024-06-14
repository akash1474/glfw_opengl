#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_img.h"
#include <dwmapi.h>
#include <windef.h>
#include <wingdi.h>
#include "recursive_linear_medium.h"
#include "fa-solid-900.h"

#define WIDTH 400
#define HEIGHT 600

int width{0};
int height{0};

void draw(GLFWwindow* window)
{
    glfwGetWindowSize(window, &width, &height);
    // Rendering code goes here
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    #ifdef GL_USE_OPENGL_LATEST
        ImGui_ImplOpenGL3_NewFrame();
    #else
        ImGui_ImplOpenGL2_NewFrame();
    #endif
        ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // Render Other Stuff


    // Render Imgui Stuff


    // End of render
    ImGui::Render();
    #ifdef GL_USE_OPENGL_LATEST
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    #else
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    #endif
        glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    draw(window);
}


int main(void){
    GLFWwindow* window;
    #ifdef GL_DEBUG
    OpenGL::Log::Init();
    #endif

    if (!glfwInit()) return -1;

#ifdef GL_USE_OPENGL_LATEST
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#endif

    window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Starter", NULL, NULL);
    glfwSetWindowSizeLimits(window, 330, 500, GLFW_DONT_CARE, GLFW_DONT_CARE);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    GL_INFO("OPENGL - {}",(const char*)glGetString(GL_VERSION));
    HWND WinHwnd=glfwGetWin32Window(window);
    BOOL USE_DARK_MODE = true;
    BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(WinHwnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,&USE_DARK_MODE, sizeof(USE_DARK_MODE)));


    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    #ifdef GL_USE_OPENGL_LATEST
        if(!ImGui_ImplOpenGL3_Init()){
    #else
        if(!ImGui_ImplOpenGL2_Init()){
    #endif
        GL_CRITICAL("FAILED INIT IMGUI");
        return false;
    }

    GL_INFO("Initializing Fonts");
    io.Fonts->Clear();
    io.IniFilename=nullptr;
    io.LogFilename=nullptr;

    static const ImWchar icons_ranges[] = {ICON_MIN_FA,ICON_MAX_FA,0};
    ImFontConfig icon_config;
    icon_config.MergeMode = true;
    icon_config.PixelSnapH = true;
    icon_config.FontDataOwnedByAtlas=false;

    const int font_data_size = IM_ARRAYSIZE(data_font);
    const int icon_data_size = IM_ARRAYSIZE(data_icon);

    ImFontConfig font_config;
    font_config.FontDataOwnedByAtlas=false;
    io.Fonts->AddFontFromMemoryTTF((void*)data_font, font_data_size,16,&font_config);
    io.Fonts->AddFontFromMemoryTTF((void*)data_icon, icon_data_size,20*2.0f/3.0f,&icon_config,icons_ranges);

    // glfwSwapInterval(1);
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 2.0f;
    style.ItemSpacing.y=6.0f;
    style.ScrollbarRounding=2.0f;

    while (!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &width, &height);
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    #ifdef GL_USE_OPENGL_LATEST
        ImGui_ImplOpenGL3_NewFrame();
    #else
        ImGui_ImplOpenGL2_NewFrame();
    #endif
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Render Other Stuff


        // Render Imgui Stuff
        ImGui::ShowDemoWindow();


        // End of render
        ImGui::Render();
    #ifdef GL_USE_OPENGL_LATEST
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    #else
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    #endif        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    #ifdef GL_USE_OPENGL_LATEST
        ImGui_ImplOpenGL3_Shutdown();
    #else 
        ImGui_ImplOpenGL2_Shutdown();
    #endif
        ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
