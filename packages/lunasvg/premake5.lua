project "LunaSVG"
	kind "StaticLib"
	staticruntime "On"
	language "C++"

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
      	buildoptions { "/MP" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
      	buildoptions { "/MP" }

    filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
        buildoptions { "/MP" }

