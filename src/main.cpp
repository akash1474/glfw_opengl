#include "pch.h"
#include "Log.h"
#include "Timer.h"
#include "resources/AppIcon.embed"
#include <minwindef.h>
#include <processenv.h>
#include <wingdi.h>
#include "Application.h"
#include "GLFW/glfw3.h"
#include "MultiThreading.h"
#include "AbstractAnimation.h"



// Make sure Visual Leak Detector (VLD) is installed
#ifdef DETECT_MEMORY_LEAKS_VLD
#include <vld.h>
#endif




int main(int argc, char* argv[])
{
#ifdef GL_DEBUG
    OpenGL::Log::Init();
#endif

    OpenGL::Timer timer;
    if(!Application::Init())
        return -1;
    Application::SetupSystemSignalHandling();
    Application::SetApplicationIcon(AppIcon, IM_ARRAYSIZE(AppIcon));
    MultiThreading::ImageLoader::SetThreadCount(std::thread::hardware_concurrency());


    // Initialize ImGUI
    if(!Application::InitImGui())
        return -1;

    if(argc > 1)
        Application::HandleArguments(GetCommandLineW());

    GL_CRITICAL("BootUp Time: {}ms", timer.ElapsedMillis());


    while(!glfwWindowShouldClose(Application::GetGLFWwindow()))
    {
#ifdef GL_DEBUG
        Application::ShowDebugFPSInTitle();
#endif
        bool isScrolling = Application::IsScrolling();
        Application::SetContinuousUpdate(isScrolling);
        if(Application::NeedsContinuousUpdate() || ImAnim::AreAnimationRunning())
        {
            // When animating or scrolling, poll for events to render as fast as VSync allows.
            glfwPollEvents();
        }
        else
        {
            // When idle, wait for an event or timeout after a short duration.
            glfwWaitEventsTimeout(1.0 / 15.0);
        }

        Application::Draw();
    }

    Application::Destroy();
    return 0;
}




#ifdef _WIN32
#ifdef XP_DIST
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    int argc;
    // Use the correct Windows API to parse the command line, handling paths with spaces.
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
    if(!argvW)
    {
        return -1; // Error handling
    }

    // Convert wide-character arguments to UTF-8 for the standard main() function.
    char** argv = new char*[argc];
    for(int i = 0; i < argc; ++i)
    {
        int size = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
        argv[i] = new char[size];
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], size, NULL, NULL);
    }

    int result = main(argc, argv);

    // Clean up allocated memory.
    LocalFree(argvW);
    for(int i = 0; i < argc; ++i)
    {
        delete[] argv[i];
    }
    delete[] argv;

    return result;
}
#endif
#endif

