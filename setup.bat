@echo off
set glfw=https://github.com/glfw/glfw.git
set spdlog=-o spdlog.zip https://codeload.github.com/gabime/spdlog/zip/refs/tags/v1.12.0
set imgui=-o imgui.zip https://codeload.github.com/ocornut/imgui/zip/refs/tags/v1.89.7

git clone %glfw%
curl %spdlog%
curl %imgui%
if exist glfw (
	move ./glfw/* ./packages/glfw
	del glfw
) else (
	echo [ GLFW ] No directory found
	exit /b
)
echo Extracting spdlog.zip to spdlog-1.12.0
winrar x spdlog.zip *
move ./spdlog-1.12.0/* ./packages/spdlog
del spdlog-1.12.0

echo Extracting imgui.zip to imgui-1.89.7
winrar x imgui.zip *
move ./imgui-1.89.7/*.cpp ./packages/imgui
move ./imgui-1.89.7/*.h ./packages/imgui
move ./imgui-1.89.7/backends/imgui_impl_opengl2.cpp ./packages/imgui
move ./imgui-1.89.7/backends/imgui_impl_opengl2.h ./packages/imgui
move ./imgui-1.89.7/backends/imgui_impl_glfw.cpp ./packages/imgui
move ./imgui-1.89.7/backends/imgui_impl_glfw.h ./packages/imgui
del imgui-1.89.7