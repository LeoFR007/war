@echo off
cd /d "%~dp0"
where cmake >nul 2>&1
if errorlevel 1 (
    echo ERRO: CMake nao encontrado. Instale CMake e um compilador C++17.
    pause
    exit /b 1
)
cmake -S . -B build
if errorlevel 1 goto erro
cmake --build build --config Release
if errorlevel 1 goto erro
if exist "build\Release\war.exe" (
    "build\Release\war.exe" --jogadores 3
) else if exist "build\war.exe" (
    "build\war.exe" --jogadores 3
) else (
    echo Executavel nao encontrado.
)
pause
exit /b 0
:erro
echo Falha na compilacao. Verifique a instalacao do compilador C++17.
pause
exit /b 1
