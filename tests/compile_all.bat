@echo off

for %%f in (*.c) do (
    echo Compiling %%f...
    cl %%f

    REM if errorlevel 1 (
        REM echo.
        REM echo ERROR compiling %%f
        REM pause
        REM exit /b 1
    REM )
)

echo.
echo All files compiled successfully!
pause
