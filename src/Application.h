#pragma once
#include <iostream>
#include "GLFW/glfw3.h"

class Application
{
private:
	GLFWwindow* mWindow{0};
	int width = 1100;
	int height = 650;
	double mFrameRate;
	double mFrameTime;
	bool mIsFocused;
	bool mRunAtMaxRefreshRate=false;
	bool mEnableRunAtMaxRefreshRate=false;



public:
	Application(const Application&) = delete;
	~Application() {}

	static Application& Get()
	{
		static Application instance;
		return instance;
	}



	static bool IsWindowFocused(){return Get().mIsFocused;}
	static void SetWindowIsFocused(bool aIsFocused=false) {Get().mIsFocused=aIsFocused;}

	//FPS Management
	static void SetFrameRate(double aFrameRate)
	{
		Get().mFrameRate=aFrameRate;
		Get().mFrameTime=1.0f/aFrameRate;
	}
	static void SleepForFPS(int aTargetFPS);
	static int GetTargetFPS();
	static void HandleFPSCooldown();
	static void EnableHighFPS(){Get().mEnableRunAtMaxRefreshRate=true;}
	static bool RunAtMaxRefreshRate(){return Get().mRunAtMaxRefreshRate;}
	static double GetFrameRate(){return Get().mFrameRate;}
	static double GetFrameTime(){return Get().mFrameTime;}
	static void RenderFPSInTitleBar();

	static void Draw();
	static bool Init();
	static bool InitImGui();
	static void InitFonts();
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


private:
	Application() {};
};
