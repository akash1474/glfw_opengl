project "LunaSVG"
	kind "StaticLib"
	staticruntime "Off"
	language "C++"
    buildoptions { "/MP" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"source/**.cpp",
		"source/**.h",
		"plutovg/source/**.c",
		"plutovg/include/**.h",
		"include/**.h"
	}

	includedirs{"src","include","plutovg/include"}

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

