@echo off
if /i [%1]==[]  (
	premake5 vs2022
	MsBuild GLFWProject.sln /p:configuration=Release
)
"bin/glfw_opengl.exe"

