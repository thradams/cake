@echo off
setlocal enabledelayedexpansion

rem Compiles every .c file in the current folder with cl (one at a time).
rem Usage: build_all.bat [extra cl options...]

set "FAILED="

for %%F in (*.c) do (
    echo === cl %%F ===
    cl /nologo %* "%%F"
    if errorlevel 1 set "FAILED=!FAILED! %%F"
)

if defined FAILED (
    echo.
    echo FAILED:!FAILED!
    exit /b 1
)

echo.
echo All files compiled.
exit /b 0
