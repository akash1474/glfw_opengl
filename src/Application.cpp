#include "pch.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "Timer.h"
#include "Log.h"
#include "Application.h"
#include <csignal>
#include <filesystem>
#include <shellapi.h>
#include <stdio.h>
#include <winuser.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include "imgui_impl_glfw.h"
#include <dwmapi.h>
#include <windef.h>
#include <wingdi.h>
#include "Utils.h"
#include "stb/stb_image.h"
#include "managers/FontManager.h"
#include "Notification.h"
#include "MultiThreading.h"
#include "DebuggerWindow.h"
#include "Types.h"


#include "resources/FontAwesomeRegular.embed"
#include "resources/FontAwesomeSolid.embed"
#include "resources/JetBrainsMonoNLRegular.embed"
#include "resources/JetBrainsMonoNLItalic.embed"

#undef min
#undef max



bool Application::Init()
{
    OpenGL::ScopedTimer timer("Application::Init");


    if(!glfwInit())
        return false;

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, 1);

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    Get().mWindow = glfwCreateWindow(
        (int)((float)Get().width * main_scale),
        (int)((float)Get().height * main_scale),
        APP_NAME,
        NULL,
        NULL
    );
    if(Get().mWindow == nullptr)
    {
        GL_CRITICAL("Failed to create GLFWwindow!");
        return false;
    }
    glfwSetWindowSizeLimits(Get().mWindow, 380, 500, GLFW_DONT_CARE, GLFW_DONT_CARE);
    if(!Get().mWindow)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(Get().mWindow);
    GL_INFO("OPENGL - {}", (const char*)glGetString(GL_VERSION));
    HWND WinHwnd = glfwGetWin32Window(Application::GetGLFWwindow());
    BOOL USE_DARK_MODE = true;
    BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
        WinHwnd,
        DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
        &USE_DARK_MODE,
        sizeof(USE_DARK_MODE)
    ));


    return true;
}


void Application::SetApplicationIcon(unsigned char* logo_img, int length)
{
    OpenGL::ScopedTimer timer("AppIconLoadTime");
    GLFWimage images[1];
    images[0].pixels = stbi_load_from_memory(
        logo_img,
        length,
        &images[0].width,
        &images[0].height,
        0,
        4
    ); // rgba channels
    glfwSetWindowIcon(Get().mWindow, 1, images);
    stbi_image_free(images[0].pixels);
}

// For mp3 files being dragged and dropped
void Application::GLFWItemDropCallback(GLFWwindow* /*window*/, int count, const char** droppedPaths)
{
    for(int i = 0; i < count; i++)
    {
        if(std::filesystem::is_directory(droppedPaths[i]))
        {
            GL_INFO("Folder: {}", droppedPaths[i]);
        }
        else
        {
            GL_INFO("File: {}", droppedPaths[i]);
        }
    }
}

void Application::FrameBufferResizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
    Application::Draw();
}

void Application::GLFWWindowResizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    GL_INFO("WindowResized:{}x{}", width, height);
}


// GLFW's callback for when the content scale changes.
void Application::ContentScaleCallback(GLFWwindow* /*window*/, float xscale, float /*yscale*/)
{
    FontManager::Init(xscale);
}


bool Application::InitImGui()
{
    GL_INFO("ImGui Init");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();


    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#ifdef IMGUI_HAS_DOCK
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // Enable when you what the imgui window to be dragged outside the window
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

#ifdef GL_DIST
    io.ConfigDebugHighlightIdConflicts = false;
#endif


#ifdef IMGUI_HAS_DOCK
    ImGuiStyle& style = ImGui::GetStyle();
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif

    if(!ImGui_ImplGlfw_InitForOpenGL(Get().mWindow, true))
    {
        GL_CRITICAL("FAILED INIT IMGUI");
        return false;
    }
#ifdef GL_BUILD_OPENGL2
    if(!ImGui_ImplOpenGL2_Init())
    {
        GL_INFO("INIT OPENGL2");
#else
    if(!ImGui_ImplOpenGL3_Init())
    {
        GL_INFO("INIT OPENGL3");
#endif
        GL_ERROR("Failed to initit OpenGL");
        return false;
    }
    GL_INFO("OPENGL - {}", (const char*)glGetString(GL_VERSION));

    glfwSwapInterval(1);
    // SetStyleColorDarkness();
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    FontManager::Init(main_scale);

    glfwSetDropCallback(Get().mWindow, Application::GLFWItemDropCallback);
    glfwSetFramebufferSizeCallback(Get().mWindow, Application::FrameBufferResizeCallback);
    glfwSetScrollCallback(Application::GetGLFWwindow(), Application::GLFWScrollCallback);
    glfwSetWindowSizeCallback(Application::GetGLFWwindow(), Application::GLFWWindowResizeCallback);
    glfwSetWindowContentScaleCallback(
        Application::GetGLFWwindow(),
        Application::ContentScaleCallback
    );


    return true;
}

void Application::CenterWindowOnScreen()
{
    int horizontal = 0;
    int vertical = 0;
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
}

void Application::BackupDataBeforeCrash() {}

#ifdef GL_DEBUG
void Application::ShowDebugFPSInTitle()
{
    static double prevTime = 0.0;
    static double crntTime = 0.0;
    static double timeDiff;
    static unsigned int counter = 0;

    crntTime = glfwGetTime();
    timeDiff = crntTime - prevTime;
    counter++;
    if(timeDiff >= 1.0 / 30.0)
    {
        std::string FPS = std::to_string(int((1.0 / timeDiff) * counter));
        std::string ms = std::to_string((timeDiff / counter) * 1000);
        std::string newTitle = "Dev(Debug) - " + FPS + "FPS / " + ms + "ms";
        glfwSetWindowTitle(Application::GetGLFWwindow(), newTitle.c_str());

        prevTime = crntTime;
        counter = 0;
    }
}
#endif

void Application::GLFWScrollCallback(GLFWwindow* /*window*/, double xoffset, double yoffset)
{
    xoffset *= Get().scroll_multiplier;
    yoffset *= Get().scroll_multiplier;
    // Immediately stop if direction changes
    if(Get().scroll_energy.x * xoffset < 0.0f)
    {
        Get().scroll_energy.x = 0.0f;
    }
    if(Get().scroll_energy.y * yoffset < 0.0f)
    {
        Get().scroll_energy.y = 0.0f;
    }
    Get().scroll_energy.x += (float)xoffset;
    Get().scroll_energy.y += (float)yoffset;
}

void Application::ApplySmoothScrolling()
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2& scrollEnergy = Get().scroll_energy;
    const float smoothing = Get().scroll_smoothing;
    const float cutoff = 0.01f; // Threshold to stop scrolling

    ImVec2 scrollNow(0.0f, 0.0f);

    // Calculate the scroll delta for the current frame
    // Horizontal scrolling
    if(std::abs(scrollEnergy.x) > cutoff)
    {
        scrollNow.x = scrollEnergy.x * io.DeltaTime * smoothing;
        scrollEnergy.x -= scrollNow.x;
    }
    else
    {
        scrollEnergy.x = 0.0f; // Snap to zero when slow enough
    }

    // Vertical scrolling
    if(std::abs(scrollEnergy.y) > cutoff)
    {
        scrollNow.y = scrollEnergy.y * io.DeltaTime * smoothing;
        scrollEnergy.y -= scrollNow.y;
    }
    else
    {
        scrollEnergy.y = 0.0f; // Snap to zero when slow enough
    }

    // Apply the calculated scroll delta to ImGui's input state.
    // This must be done before ImGui::NewFrame().
    io.MouseWheelH = -scrollNow.x; // Horizontal scroll is often inverted
    io.MouseWheel = scrollNow.y;
}

void Application::SetScriptPath(const std::string& path) { Get().m_scriptPath = path; }

// Reloads the Lua script and updates the last-modified time
void Application::ReloadScript()
{
    Application& app = Get();
    if(!std::filesystem::exists(app.m_scriptPath))
    {
        GL_ERROR("Script file '{}' does not exist!", app.m_scriptPath.string());
        return;
    }

    GL_INFO("Reloading script: {}", app.m_scriptPath.string());
    try
    {
        // This is the crucial part: we re-run the script file.
        // This will overwrite the OnImGuiDraw function with the new version.
        app.mLua.script_file(app.m_scriptPath.string());

        // Update the timestamp so we don't reload it again immediately
        app.m_lastWriteTime = std::filesystem::last_write_time(app.m_scriptPath);
    }
    catch(const sol::error& e)
    {
        // IMPORTANT: Catch syntax errors so a typo in Lua doesn't crash the app!
        GL_ERROR("LUA ERROR: {}", e.what());
    }
}

// Checks if the script file has been modified
void Application::PollScriptChanges()
{
    Application& app = Get();

    // Throttle the check to once per second to be efficient
    auto now = std::chrono::steady_clock::now();
    if(now - app.m_lastPollTime < std::chrono::seconds(1))
    {
        return;
    }
    app.m_lastPollTime = now;

    if(!std::filesystem::exists(app.m_scriptPath))
    {
        return;
    }

    auto currentWriteTime = std::filesystem::last_write_time(app.m_scriptPath);

    if(currentWriteTime > app.m_lastWriteTime)
    {
        ReloadScript();
    }
}


void Application::Render()
{
    try
    {
        sol::function on_draw = GetLua()["OnImGuiDraw"];
        if(on_draw.valid())
        {
            on_draw();
        }
        else
        {
            GL_WARN("Application::Render() - No Lua OnDraw found!");
        }
    }
    catch(const std::exception& exception)
    {
        GL_CRITICAL("Error:{}", exception.what());
    }
}

void Application::PreRender()
{
    GLFWwindow* glfwWindowPtr = Application::GetGLFWwindow();
    bool tIsWindowFocused = glfwGetWindowAttrib(glfwWindowPtr, GLFW_FOCUSED) != 0;

    MultiThreading::ImageLoader::LoadImages();
    DebuggerWindow::EventListener(tIsWindowFocused);
}

void Application::PostRender()
{
    if(Notification::mDisplayNotification)
    {
        Notification::Render();
    }
#ifdef GL_DEBUG
    DebuggerWindow::Render();

    if(ImGui::IsKeyPressed(ImGuiKey_F1))
    {
        Notification::ShowDummyNotification();
    }
    if(ImGui::IsKeyPressed(ImGuiKey_F2))
    {
        Notification::ShowDummyAsyncNotification();
    }
#endif
}



void Application::Draw()
{
    ApplySmoothScrolling();



    FontManager::ProcessReloadRequests();
#ifdef GL_BUILD_OPENGL2
    ImGui_ImplOpenGL2_NewFrame();
#else
    ImGui_ImplOpenGL3_NewFrame();
#endif
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render application UI
    FontManager::Push(FontFamily::Regular, FontSize::Small);
    Application::PreRender();
    Application::Render();
    Application::PostRender();
    FontManager::Pop();



    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(Get().mWindow, &display_w, &display_h);
    if(display_w > 0 && display_h > 0)
    {
        glViewport(0, 0, display_w, display_h);
    }

    // Clear screen (minimize state check prevents unnecessary clearing)
    glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render ImGui draw data
#ifdef GL_BUILD_OPENGL2
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
#else
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

#ifdef IMGUI_HAS_DOCK
    // Ensure platform windows are updated and rendered
    ImGuiIO& io = ImGui::GetIO();
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(Get().mWindow); // Ensure we are back to the main context
    }
#endif



    glfwSwapBuffers(Get().mWindow);
}


void Application::Destroy()
{
#ifdef GL_BUILD_OPENGL2
    ImGui_ImplOpenGL2_Shutdown();
#else
    ImGui_ImplOpenGL3_Shutdown();
#endif
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(GetGLFWwindow());
    glfwTerminate();
}

void Application::HandleArguments(std::wstring commands)
{
    // TODO: Display Notification to user if something goes wrong
    GL_INFO("Parsing Args");
    namespace fs = std::filesystem;
    int nArgs;

    LPWSTR* wArgList = CommandLineToArgvW(commands.c_str(), &nArgs);
    if(!wArgList)
    {
        GL_ERROR("Failed to parse args");
        return;
    }


    for(int i = 1; i < nArgs; ++i)
    {
        fs::path path(wArgList[i]);
        if(fs::exists(path))
        {
            if(fs::is_regular_file(path))
            {
                GL_INFO("FILE:{}", path.generic_string());
                // core->GetTextEditor()->LoadFile(path.generic_string().c_str());
            }
            else if(fs::is_directory(path))
            {
                GL_INFO("FOLDER:{}", path.generic_string());
            }
            else
            {
                ShowErrorMessage("Invalid File/Folder Selected");
            }
        }
        else
        {
            ShowErrorMessage("Path Doesn't Exists");
        }
    }

    LocalFree(wArgList);
}


void Application::HandleCrash(int signal)
{
    const char* errorMessage = nullptr;
    switch(signal)
    {
    case SIGSEGV: errorMessage = "Segmentation fault"; break;
    case SIGABRT: errorMessage = "Abort signal received"; break;
    case SIGFPE: errorMessage = "Floating point exception"; break;
    case SIGILL: errorMessage = "Illegal instruction"; break;
    case SIGTERM: errorMessage = "Termination request"; break;
    case SIGINT: errorMessage = "User Interrupt(Ctrl+C)";
    default: errorMessage = "Unknown error"; break;
    }

    GL_CRITICAL("Error:{} (Signal: {})", errorMessage, signal);
}


void Application::SetupSystemSignalHandling()
{
    std::signal(SIGSEGV, Application::HandleCrash); // Segmentation fault
    std::signal(SIGABRT, Application::HandleCrash); // Abort
    std::signal(SIGFPE, Application::HandleCrash);  // Floating point exception
    std::signal(SIGILL, Application::HandleCrash);  // Illegal instruction
    std::signal(SIGTERM, Application::HandleCrash); // Termination request
    std::signal(SIGINT, Application::HandleCrash);  // Interrupt (Ctrl+C)
}
