@echo off
setlocal

set "BASE_DIR=packages"

if not exist "%BASE_DIR%" (
  echo Base directory "%BASE_DIR%" not found.
  exit /b 1
)

for /d %%D in ("%BASE_DIR%\*") do (
  rem Extract just the folder name (no path)
  for %%N in ("%%~nxD") do (
    if /i "%%~nxD"=="ImAnim" (
      echo [Deleted] - %%D
      rmdir /s /q "%%D"
    ) else (
      if exist "%%D\premake5.lua" (
        echo [Cleaned] - %%D
        rem Delete files except premake5.lua
        for /f "delims=" %%F in ('dir /b /a-d "%%D" 2^>nul') do (
          if /i not "%%F"=="premake5.lua" del /f /q "%%D\%%F"
        )
        rem Delete subfolders
        for /f "delims=" %%S in ('dir /b /ad "%%D" 2^>nul') do (
          rmdir /s /q "%%D\%%S"
        )
      ) else (
        echo [Deleted] - %%D
        rmdir /s /q "%%D"
      )
    )
  )
)

endlocal