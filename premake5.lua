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

includeDirs={}
includeDirs["glfw"]="packages/glfw/include"
includeDirs["SpdLog"]="packages/spdlog/include"
includeDirs["ImGui"]="packages/imgui"
includeDirs["LunaSVG"]="packages/lunasvg/include"
includeDirs["nlohmann"]="packages/nlohmann"

-- /MP -- Multithreaded build 
-- /MT -- Static Linking. Defines _MT 
-- /MD -- Dynamic Linking. Defines _MT and _DLL 
include "packages/glfw"
include "packages/imgui"
include "packages/lunasvg"

project "glfw_opengl"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin"
   objdir "bin/obj"
   pchheader "pch.h"
   pchsource "src/pch.cpp"

   links {
      "glfw","ImGui","opengl32","LunaSVG","dwmapi","Shlwapi","winmm"
   }

   includedirs{
      "src",
      "%{includeDirs.glfw}",
      "%{includeDirs.ImGui}",
      "%{includeDirs.LunaSVG}",
      "%{includeDirs.SpdLog}",
      "%{includeDirs.nlohmann}"
   }

   files { 
      "src/**.cpp"
   }


   filter "system:windows"
      systemversion "latest"

   filter "configurations:Debug"
      runtime "Debug"
      symbols "On"
      staticruntime "On"
      optimize "Off"
      buildoptions { "/MP","/DEBUG:FULL" }
      defines {"GL_DEBUG"}

   filter {"configurations:Release"}
      runtime "Release"
      optimize "On"
      symbols "Off"
      characterset ("MBCS")
      staticruntime "On"
      buildoptions { "/MP","/utf-8" }
      defines {"GL_DEBUG","_CRT_SECURE_NO_WARNINGS"}

   filter "configurations:Dist"
      kind "WindowedApp"
      runtime "Release"
      optimize "On"
      symbols "Off"
      characterset ("MBCS")
      staticruntime "On"
      buildoptions { "/MP","/utf-8"}
      linkoptions {"/ENTRY:mainCRTStartup"}
