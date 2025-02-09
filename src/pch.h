#pragma once
#define GL_BUILD_OPENGL3
#define APP_NAME "Dev"

#include <iostream>
#include <cmath>

#include <future>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <list>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#ifdef GL_BUILD_OPENGL2
	#include "imgui_impl_opengl2.h"
#else
	#include "imgui_impl_opengl3.h"
#endif
#include "imgui_internal.h"



#include "FontAwesome6.h"
#include "Log.h"
#include "Utils.h"
#include "Timer.h"
