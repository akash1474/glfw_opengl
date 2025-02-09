#include "pch.h"
#include "Log.h"
#include "Timer.h"
#include "resources/AppIcon.embed"
#include <minwindef.h>
#include <processenv.h>
#include <wingdi.h>
#include "Application.h"
#include "GLFW/glfw3.h"




//Make sure Visual Leak Detector (VLD) is installed
#ifdef DETECT_MEMORY_LEAKS_VLD
    #include <vld.h>
#endif





int main(int argc, char* argv[])
{
    OpenGL::Timer timer;
    if (!Application::Init())
        return -1;
    Application::SetupSystemSignalHandling();
    Application::SetApplicationIcon(AppIcon, IM_ARRAYSIZE(AppIcon));


    // Initialize ImGUI
    if (!Application::InitImGui())
        return -1;
    Application::InitFonts();

    if (argc > 1)
        Application::HandleArguments(GetCommandLineW());

    GL_CRITICAL("BootUp Time: {}ms", timer.ElapsedMillis());
    Application::SetWindowIsFocused(true);
    Application::SetFrameRate(120.0f);

    
    while (!glfwWindowShouldClose(Application::GetGLFWwindow())) {
        #ifdef GL_DEBUG
        Application::RenderFPSInTitleBar();
        #endif

        Application::HandleFPSCooldown();

        int targetFPS = Application::GetTargetFPS();
        if(targetFPS>1)
            Application::Draw();

        if(!Application::RunAtMaxRefreshRate())
            Application::SleepForFPS(targetFPS);
    }

    Application::Destroy();
    return 0;
}







#ifdef _WIN32
    #ifdef GL_DIST
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::stringstream test(lpCmdLine);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(test, segment, ' ')) seglist.push_back(segment);

    return main((int)seglist.size(), &lpCmdLine);
}
    #endif
#endif
