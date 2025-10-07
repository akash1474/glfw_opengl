project "ImGui"
	kind "SharedLib"
	staticruntime "Off"
	language "C++"
    buildoptions { "/MP" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"**.cpp",
		"**.h"
	}

	links{"glfw"}
	libdirs {"%{wks.location}/packages/glfw/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/glfw"}
	
	includedirs{".","../glfw/include"}
	defines { "IMGUI_API=__declspec(dllexport)", "GLFW_DLL" }

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		optimize "off"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

    filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
