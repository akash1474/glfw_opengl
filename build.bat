@echo off
if /i [%1]==[]  (
	premake5 vs2022
	MsBuild GLFWProject.sln /p:configuration=Debug
)
"bin/glfw_opengl.exe"

