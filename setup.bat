@echo off
rem "Use `setup imgui-docking` to setup with imgui-docking branch"


set glfw=https://github.com/glfw/glfw.git
set nlohmannjson=https://github.com/nlohmann/json.git
set lunasvg=https://github.com/sammycage/lunasvg.git
set imgui=-o imgui.zip https://codeload.github.com/ocornut/imgui/zip/refs/tags/v1.91.8
set spdlog=-o spdlog.zip https://codeload.github.com/gabime/spdlog/zip/refs/tags/v1.12.0



echo -- Cloning glfw github repo
git clone --depth 1 %glfw%
echo.

echo -- Cloning nlohmannjson github repo
git clone --depth 1 %nlohmannjson%
echo.

echo -- Cloning spdlog
if not exist .\packages\spdlog mkdir .\packages\spdlog
git clone -b v1.12.0 --single-branch https://github.com/gabime/spdlog.git --depth 1 ./packages/spdlog
rmdir /s /q .\packages\spdlog\.git
echo.

echo -- Cloning imgui
if /i [%1] == [imgui-docking] (
	echo "ImGui::Docking"
	git clone -b v1.91.8-docking --single-branch https://github.com/ocornut/imgui.git --depth 1
) else (
	echo "ImGui::Main"
	git clone -b v1.91.8 --single-branch https://github.com/ocornut/imgui.git --depth 1
)
echo.

echo -- Cloning lunasvg
git clone -b v2.3.9 --single-branch https://github.com/sammycage/lunasvg.git --depth 1
echo.



if exist glfw (
	echo.
	echo Setting Up GLFW
	move /y ".\glfw\deps" ".\packages\glfw"
	move /y ".\glfw\include" ".\packages\glfw"
	move /y ".\glfw\src" ".\packages\glfw"
) else (
	echo [ GLFW ] No directory found
	exit /b
)

if exist json (
	echo.
	if not exist .\packages\nlohmann (
		echo "CreatedDir: .\packages\nlohmann"
		mkdir .\packages\nlohmann
	)

	echo Setting Up nlohmannjson
	move /y ".\json\include" ".\packages\nlohmann"
) else (
	echo "[ JSON(nlohmannjson) ] No directory found"
	exit /b
)

if exist lunasvg (
	echo.
	echo Setting Up LunaSVG
	move /y ".\lunasvg\*" ".\packages\lunasvg"
	move /y ".\lunasvg\3rdparty" ".\packages\lunasvg"
	move /y ".\lunasvg\include" ".\packages\lunasvg"
	move /y ".\lunasvg\source" ".\packages\lunasvg"
) else (
	echo [ LunaSVG ] No directory found
	exit /b
)

if not exist .\packages\imgui mkdir .\packages\imgui


echo -- Setting up imgui
move /y ".\imgui\*.cpp" ".\packages\imgui"
move /y ".\imgui\*.h" ".\packages\imgui"
move /y ".\imgui\backends\imgui_impl_opengl2.cpp" ".\packages\imgui"
move /y ".\imgui\backends\imgui_impl_opengl2.h" ".\packages\imgui"
move /y ".\imgui\backends\imgui_impl_opengl3.cpp" ".\packages\imgui"
move /y ".\imgui\backends\imgui_impl_opengl3.h" ".\packages\imgui"
move /y ".\imgui\backends\imgui_impl_opengl3_loader.h" ".\packages\imgui"
move /y ".\imgui\backends\imgui_impl_glfw.cpp" ".\packages\imgui"
move /y ".\imgui\backends\imgui_impl_glfw.h" ".\packages\imgui"

echo -- Cleaning
rmdir /s /q imgui
rmdir /s /q glfw
rmdir /s /q lunasvg
rmdir /s /q json
build