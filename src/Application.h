#pragma once
#include <iostream>
#include "GLFW/glfw3.h"
#include "imgui.h"

class Application
{
private:
	GLFWwindow* mWindow{0};
	int width = 1100;
	int height = 650;
	bool mIsFocused;

    // FPS Management
    bool mNeedsContinuousUpdate = false;

    // Smooth Scrolling
    const float scroll_multiplier = 1.0f;
    const float scroll_smoothing = 8.0f;
    ImVec2 scroll_energy;


public:
	Application(const Application&) = delete;
	~Application() {}

	static Application& Get()
	{
		static Application instance;
		return instance;
	}

    // FPS Management
    static bool NeedsContinuousUpdate() { return Get().mNeedsContinuousUpdate; }
    static void SetContinuousUpdate(bool needsUpdate)
    {
        Get().mNeedsContinuousUpdate = needsUpdate;
    }
    static void RequestNextFrame() { glfwPostEmptyEvent(); } // Add this!
    static bool IsScrolling() { return std::abs(Application::Get().scroll_energy.y) > 0.01f; }

    static void ApplySmoothScrolling();

	static void Draw();
	static void Render();
	static void PreRender();
	static void PostRender();
	static bool Init();
	static bool InitImGui();
	static void SetApplicationIcon(unsigned char* img, int length);
	void BackupDataBeforeCrash(); // Unimplemented still working but making a commit

	static void HandleCrash(int signal);
	static void SetupSystemSignalHandling();

	// Update TitleBar Color based on the width and current UserInterface/Page
	static void UpdateTitleBarColor();

	static void Destroy();
	static void HandleArguments(std::wstring args);
	static void CenterWindowOnScreen();
	static GLFWwindow* GetGLFWwindow() { return Get().mWindow; }

    // GLFW Callbacks
    static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
    static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void GLFWItemDropCallback(GLFWwindow* window, int count, const char** droppedPaths);
    static void GLFWWindowResizeCallback(GLFWwindow* window, int width, int height);
    static void ContentScaleCallback(GLFWwindow* window, float xscale, float yscale);

#ifdef GL_DEBUG
    static void ShowDebugFPSInTitle();
#endif

private:
	Application() {};
};
