-- Building
-- MsBuild XPlayer.sln /p:configuration=Release

workspace "GLFWProject"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDirs={}
includeDirs["glfw"]="packages/glfw/include"

-- /MP -- Multithreaded build 
-- /MT -- Static Linking. Defines _MT 
-- /MD -- Dynamic Linking. Defines _MT and _DLL 
include "packages/glfw"

project "glfw_opengl"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin"
   objdir "bin/obj"
   pchheader "pch.h"
   pchsource "src/pch.cpp"

   -- libdirs{"packages/glfw/lib"}
   links {
      "glfw","opengl32"
   }

   includedirs{
      "src",
      "%{includeDirs.glfw}"
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
      buildoptions { "/MP" }

   filter {"configurations:Release"}
      runtime "Release"
      optimize "On"
      symbols "Off"
      characterset ("MBCS")
      staticruntime "On"
      buildoptions { "/MP","/utf-8" }
      defines {"FT_DEBUG","_CRT_SECURE_NO_WARNINGS"}

   filter "configurations:Dist"
      kind "WindowedApp"
      runtime "Release"
      optimize "On"
      symbols "Off"
      characterset ("MBCS")
      staticruntime "On"
      buildoptions { "/MP","/utf-8"}
      linkoptions {"/ENTRY:mainCRTStartup"}
