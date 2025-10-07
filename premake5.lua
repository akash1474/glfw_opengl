-- Building
-- MsBuild XPlayer.sln /p:configuration=Release

-- newaction {
--     trigger = "setup",
--     description = "Runs a custom Lua script",
--     execute = function()
--         dofile("myscript.lua") -- Runs 'myscript.lua' in the same context
--     end
-- }

workspace "GLFWProject"
architecture "x64"
configurations { "Debug", "Release", "Dist" }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDirs             = {}
includeDirs["glfw"]     = "packages/glfw/include"
includeDirs["SpdLog"]   = "packages/spdlog/include"
includeDirs["ImGui"]    = "packages/imgui"
includeDirs["LunaSVG"]  = "packages/lunasvg/include"
includeDirs["nlohmann"] = "packages/nlohmann/include"
includeDirs["ImAnim"]   = "packages/ImAnim/src"
includeDirs["LuaJIT"]   = "packages/lua"
includeDirs["Sol3"]     = "packages/sol3"

-- /MP -- Multithreaded build
-- /MT -- Static Linking. Defines _MT
-- /MD -- Dynamic Linking. Defines _MT and _DLL
include "packages/glfw"
include "packages/imgui"
include "packages/lunasvg"
include "packages/ImAnim"

project "imgui_lua_bindings"
    kind "SharedLib" -- This creates a DLL
    staticruntime "Off"
    language "C++"
    cppdialect "C++17"
    targetdir "bin"
    objdir "bin/obj"

    files {
        "imguiApiHostGenerated.cpp" -- Add your generated C++ file here
    }

    includedirs {
        "src",
        "src/external",
        "%{includeDirs.ImGui}", -- Needs ImGui headers
        "%{includeDirs.LuaJIT}",
        "%{includeDirs.glfw}",
        "%{includeDirs.SpdLog}",
    }

    links {
        "ImGui", -- Link against the ImGui static library
        "glfw",
        "opengl32"
    }

    filter "system:windows"
    systemversion "latest"

    defines { 
        "IMGUI_API=__declspec(dllimport)",
         "GLFW_DLL"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        buildoptions {"/DEBUG:FULL", "/utf-8" }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"






project "glfw_opengl"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin"
    objdir "bin/obj"
    pchheader "pch.h"
    pchsource "src/pch.cpp"
    staticruntime "Off"

    links {
        "glfw",
        "ImGui",
        "opengl32",
        "LunaSVG",
        "dwmapi",
        "Shlwapi",
        "winmm",
        "ImAnim",
        "luajit",
        "lua51",
        "imgui_lua_bindings"
    }

    includedirs {
        "src",
        "src/external",
        "%{includeDirs.glfw}",
        "%{includeDirs.ImGui}",
        "%{includeDirs.LunaSVG}",
        "%{includeDirs.SpdLog}",
        "%{includeDirs.nlohmann}",
        "%{includeDirs.ImAnim}",
        "%{includeDirs.LuaJIT}",
        "%{includeDirs.Sol3}"
    }

    files {
        "src/**.cpp"
    }


    filter "system:windows"
        systemversion "latest"

    defines { "IMGUI_API=__declspec(dllimport)", "GLFW_DLL" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"
        buildoptions {"/DEBUG:FULL", "/utf-8" }
        defines { "GL_DEBUG" }

    filter { "configurations:Release" }
        runtime "Release"
        optimize "On"
        symbols "Off"
        characterset("MBCS")
        buildoptions { "/utf-8" }
        defines { "GL_DEBUG", "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:Dist"
        kind "WindowedApp"
        runtime "Release"
        optimize "On"
        symbols "Off"
        characterset("MBCS")
        buildoptions {  "/utf-8" }
        linkoptions { "/ENTRY:mainCRTStartup" }
