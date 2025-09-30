@echo off
setlocal

rem --- Setup ANSI Colors for the terminal ---
for /f %%a in ('echo prompt $E^| cmd') do set "ESC=%%a"
set "yellow=%ESC%[93m"
set "green=%ESC%[92m"
set "red=%ESC%[91m"
set "reset=%ESC%[0m"

rem --- Check if Git is installed ---
git --version > nul 2>&1
if %errorlevel% neq 0 (
    echo %red%Error: git is not installed or not in the PATH.%reset%
    exit /b 1
)

rem --- Create main packages directory ---
if not exist .\packages mkdir .\packages

rem --- Package: LunaSVG ---
echo|set /p="- Installing %yellow%LunaSVG%reset%          ... "
git clone -b v3.5.0 --single-branch https://github.com/sammycage/lunasvg.git --depth 1 > nul 2>&1
if exist lunasvg (
    if not exist .\packages\lunasvg mkdir .\packages\lunasvg
    move /y ".\lunasvg\plutovg" ".\packages\lunasvg" > nul
    move /y ".\lunasvg\include" ".\packages\lunasvg" > nul
    move /y ".\lunasvg\source" ".\packages\lunasvg" > nul
    rmdir /s /q lunasvg
    echo %green%Done.%reset%
) else (
    echo %red%Failed.%reset%
    exit /b 1
)

rem --- Package: GLFW ---
echo|set /p="- Installing %yellow%GLFW%reset%             ... "
git clone --depth 1 https://github.com/glfw/glfw.git > nul 2>&1
if exist glfw (
    if not exist .\packages\glfw mkdir .\packages\glfw
    move /y ".\glfw\deps" ".\packages\glfw" > nul
    move /y ".\glfw\include" ".\packages\glfw" > nul
    move /y ".\glfw\src" ".\packages\glfw" > nul
    rmdir /s /q glfw
    echo %green%Done.%reset%
) else (
    echo %red%Failed.%reset%
    exit /b 1
)

rem --- Package: ImAnim ---
echo|set /p="- Installing %yellow%ImAnim%reset%           ... "
if not exist .\packages\ImAnim mkdir .\packages\ImAnim
git clone --depth 1 https://github.com/akash1474/ImAnim.git .\packages\ImAnim > nul 2>&1
if exist .\packages\ImAnim (
    echo %green%Done.%reset%
) else (
    echo %red%Failed.%reset%
    exit /b 1
)


rem --- Package: ImGui ---
echo|set /p="- Installing %yellow%ImGui%reset%            ... "
if /i [%1] == [imgui-docking] (
    git clone -b v1.92.3-docking --single-branch https://github.com/ocornut/imgui.git --depth 1 > nul 2>&1
) else (
    git clone -b v1.92.3 --single-branch https://github.com/ocornut/imgui.git --depth 1 > nul 2>&1
)
if exist imgui (
    if not exist .\packages\imgui mkdir .\packages\imgui
    move /y ".\imgui\*.cpp" ".\packages\imgui" > nul
    move /y ".\imgui\*.h" ".\packages\imgui" > nul
    move /y ".\imgui\backends\imgui_impl_glfw.*" ".\packages\imgui" > nul
    move /y ".\imgui\backends\imgui_impl_opengl*.*" ".\packages\imgui" > nul
    rmdir /s /q imgui
    echo %green%Done.%reset%
) else (
    echo %red%Failed.%reset%
    exit /b 1
)

rem --- Package: Nlohmann JSON ---
echo|set /p="- Installing %yellow%Nlohmann JSON%reset%    ... "
git clone --depth 1 https://github.com/nlohmann/json.git > nul 2>&1
if exist json (
    if not exist .\packages\nlohmann mkdir .\packages\nlohmann
    move /y ".\json\include" ".\packages\nlohmann" > nul
    rmdir /s /q json
    echo %green%Done.%reset%
) else (
    echo %red%Failed.%reset%
    exit /b 1
)

rem --- Package: spdlog ---
echo|set /p="- Installing %yellow%spdlog%reset%           ... "
if not exist .\packages\spdlog mkdir .\packages\spdlog
git clone -b v1.15.3 --single-branch https://github.com/gabime/spdlog.git --depth 1 ./packages/spdlog > nul 2>&1
if exist .\packages\spdlog\.git (
    rmdir /s /q .\packages\spdlog\.git
    echo %green%Done.%reset%
) else (
    echo %red%Failed.%reset%
    exit /b 1
)

rem --- Final cleanup message ---
echo|set /p="- Cleaning up temporary files ... "
echo %green%Done.%reset%

echo.
echo %green% ✅ Setup complete! %green%
echo.

rem --- Final build step from original script ---
build

endlocal