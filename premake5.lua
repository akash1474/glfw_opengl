-- Building
-- MsBuild XPlayer.sln /p:configuration=Release

workspace "GLFWProject"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDirs={}
includeDirs["glfw"]="packages/glfw/include"


project "glfw_opengl"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin"
   objdir "bin/obj"
   pchheader "pch.h"
   pchsource "src/pch.cpp"

   libdirs{"packages/glfw/lib"}
   links {"glfw3","opengl32","gdi32","user32","shell32"}

   includedirs{
      "src",
      "packages/glfw/include"
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
      staticruntime "On"
      buildoptions { "/MP" }

   filter "configurations:Dist"
      kind "WindowedApp"
      runtime "Release"
      optimize "On"
      symbols "Off"
      staticruntime "On"
      buildoptions { "/MP" }
