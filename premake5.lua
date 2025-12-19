-- Building
-- MsBuild XPlayer.sln /p:configuration=Release

-- newaction {
--     trigger = "setup",
--     description = "Runs a custom Lua script",
--     execute = function()
--         dofile("myscript.lua") -- Runs 'myscript.lua' in the same context
--     end
-- }

--[[
| Build Type                       | Output Files               | Description                                                                                                                                                                         |
| -------------------------------- | -------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Dynamic (DLL) build**          | ✅ `.dll` + `.lib` + `.exp` | Normal DLL build. `.dll` contains code; `.lib` is a *small import library* (used by other projects to link easily); `.exp` is an intermediate file containing exported symbol info. |
| **Static Library build**         | ✅ `.lib` only              | When you compile with `/LIB` or `/MT` (static CRT), you get a `.lib` containing *actual object code* — no `.dll` is produced.                                                       |
| **Dynamic build with no `.lib`** | ✅ `.dll` only              | Possible if you don’t export/import with `__declspec(dllexport)` and instead use `GetProcAddress()` manually (no import library needed).                                            |

--------------------------------------------------------------------------------------------------------------------

| Mode                 | Files Needed    | Load Time | How to Call          | Typical Use                       |
| -------------------- | --------------- | --------- | -------------------- | --------------------------------- |
| **Implicit linking** | `.dll` + `.lib` | Load-time | Normal call (linked) | Normal shared libs                |
| **Explicit linking** | `.dll` only     | Runtime   | `GetProcAddress()`   | Plugin systems, optional features |

--------------------------------------------------------------------------------------------------------------------

| Keyword                 | Used in       | Purpose                   | Effect                                   |
| ----------------------- | ------------- | ------------------------- | ---------------------------------------- |
| `__declspec(dllexport)` | DLL build     | Export function/class     | Adds symbol to DLL export table          |
| `__declspec(dllimport)` | App using DLL | Import function/class     | Tells linker to resolve from DLL         |
| `extern "C"`            | Both          | Disable C++ name mangling | Keeps plain names for cross-language use |

--------------------------------------------------------------------------------------------------------------------

Static CRT vs Dynamic CRT
- Static CRT (/MT): Each module (EXE or DLL) gets its own copy of the runtime.
- Dynamic CRT (/MD): All modules share one runtime DLL (msvcrt.dll).

| Main EXE CRT  | Library Type (`kind`) | Library CRT (`staticruntime`) | How it Links / Loads                       |
| ------------- | --------------------- | ----------------------------- | ------------------------------------------ |
| Static (/MT)  | StaticLib             | On                            | Library included in EXE; CRT static        |
| Static (/MT)  | StaticLib             | Off                           | Library included in EXE; CRT dynamic       |
| Static (/MT)  | SharedLib (DLL)       | On                            | EXE loads DLL dynamically; DLL CRT static  |
| Static (/MT)  | SharedLib (DLL)       | Off                           | EXE loads DLL dynamically; DLL CRT dynamic |
| Dynamic (/MD) | StaticLib             | On                            | Library included in EXE; CRT static        |
| Dynamic (/MD) | StaticLib             | Off                           | Library included in EXE; CRT dynamic       |
| Dynamic (/MD) | SharedLib (DLL)       | On                            | EXE loads DLL dynamically; DLL CRT static  |
| Dynamic (/MD) | SharedLib (DLL)       | Off                           | EXE loads DLL dynamically; DLL CRT dynamic |

Notes / Rules:
- kind → decides if the library itself is static (.lib) or dynamic (.dll).
- staticruntime → decides if the C/C++ runtime (CRT) is linked statically or dynamically inside that library or EXE.
- DLLs are always loaded dynamically, even if staticruntime "On".
- Avoid mixing CRT types between EXE and libraries to prevent crashes (e.g., EXE dynamic CRT + DLL static CRT is usually safe, but DLL static CRT + EXE dynamic CRT may cause issues if memory is shared).

Safe combinations
- EXE and all static libraries use the same CRT setting (all /MT or all /MD).
- DLL and EXE both use dynamic CRT (/MD) → safe to share memory, files, etc.
- If a DLL uses static CRT, it should not share CRT-managed objects with the EXE. Only use its own memory internally.
- “DLL static CRT + EXE dynamic CRT may cause issues if memory is shared” means: 
   If you allocate memory or use CRT-managed objects in the DLL and pass them to the EXE (or vice versa), they might be handled by different CRTs, leading to crashes, corruption, or leaks.

]]



workspace "GLFWProject"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDirs={}
includeDirs["glfw"]="packages/glfw/include"
includeDirs["SpdLog"]="packages/spdlog/include"
includeDirs["ImGui"]="packages/imgui"
includeDirs["LunaSVG"]="packages/lunasvg/include"
includeDirs["nlohmann"]="packages/nlohmann/include"
includeDirs["ImAnim"]="packages/ImAnim/src"
includeDirs["freetype"]="packages/freetype/include"

-- /MP -- Multithreaded build 
-- /MT -- Static Linking. Defines _MT 
-- /MD -- Dynamic Linking. Defines _MT and _DLL 
include "packages/glfw"
include "packages/imgui"
include "packages/lunasvg"
include "packages/ImAnim"

project "ImComponents"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin"
    objdir "bin/obj"
    pchheader "pch.h"
    pchsource "src/pch.cpp"
    staticruntime "On"

    links {
        "glfw","ImGui","opengl32","LunaSVG","dwmapi","Shlwapi","winmm","ImAnim","freetype"
    }

    includedirs{
        "src",
        "src/external",
        "%{includeDirs.glfw}",
        "%{includeDirs.ImGui}",
        "%{includeDirs.LunaSVG}",
        "%{includeDirs.SpdLog}",
        "%{includeDirs.nlohmann}",
        "%{includeDirs.ImAnim}",
        "%{includeDirs.freetype}",
    }

    files { 
        "src/**.cpp"
    }


    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"
        buildoptions { "/MP","/DEBUG:FULL","/utf-8" }
        defines {"GL_DEBUG"}

    filter {"configurations:Release"}
        runtime "Release"
        optimize "On"
        symbols "Off"
        characterset ("MBCS")
        buildoptions { "/MP","/utf-8" }
        defines {"GL_DEBUG","_CRT_SECURE_NO_WARNINGS"}

    filter "configurations:Dist"
        kind "WindowedApp"
        runtime "Release"
        optimize "On"
        symbols "Off"
        characterset ("MBCS")
        buildoptions { "/MP","/utf-8"}
        linkoptions {"/ENTRY:mainCRTStartup"}
