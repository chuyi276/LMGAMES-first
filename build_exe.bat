@echo off
title LMG Game - Build EXE Tool
echo ============================================================
echo            LMG Xiuxian Game - Build EXE
echo ============================================================
echo.

REM ---- Check MSVC compiler ----
echo [1/5] Checking build environment...
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] MSVC compiler not found!
    echo.
    echo Please do the following:
    echo 1. Install Visual Studio 2022 (check "Desktop development with C++")
    echo 2. Open "x64 Native Tools Command Prompt for VS 2022" from Start Menu
    echo 3. cd to this project folder and run build_exe.bat again
    echo.
    pause
    exit /b 1
)
echo       MSVC compiler: OK

REM ---- Check Python ----
where python >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Python not found! Please install Python 3.8+ (64-bit)
    pause
    exit /b 1
)
for /f "tokens=*" %%i in ('python --version 2^>^&1') do set PYVER=%%i
echo       Python: %PYVER%

REM ---- Install dependencies ----
echo.
echo [2/5] Installing Python dependencies (pybind11 pygame pyinstaller)...
pip install --upgrade pip >nul
pip install pybind11 pygame pyinstaller
if %errorlevel% neq 0 (
    echo [ERROR] Failed to install dependencies!
    pause
    exit /b 1
)
echo       Dependencies installed

REM ---- Compile C++ module ----
echo.
echo [3/5] Compiling C++ game logic module (gamelogic.pyd)...
python setup.py build_ext --inplace
if %errorlevel% neq 0 (
    echo [ERROR] C++ module compilation failed! Check errors above.
    pause
    exit /b 1
)
echo       C++ module compiled

REM ---- Verify module ----
echo.
echo [4/5] Verifying C++ module...
python -c "import gamelogic; g=gamelogic.GameSession(); print('       Module OK, initial day:', g.t)"
if %errorlevel% neq 0 (
    echo [ERROR] gamelogic module import failed!
    pause
    exit /b 1
)

REM ---- PyInstaller pack ----
echo.
echo [5/5] Packing with PyInstaller (may take 1-3 minutes)...
echo       Please wait...
pyinstaller --clean --noconfirm gamelogic.spec
if %errorlevel% neq 0 (
    echo [ERROR] Packaging failed!
    pause
    exit /b 1
)

echo.
echo ============================================================
echo                  BUILD SUCCESS!
echo ============================================================
echo.
echo Output location:
echo   %cd%\dist\LMG_Game\LMG_Game.exe
echo.
echo How to distribute:
echo   1. Zip the entire "dist\LMG_Game" folder
echo   2. Send to others, they unzip and double-click LMG_Game.exe
echo   3. No Python or any dependencies needed on their computer
echo.
echo Notes:
echo   - This is 64-bit, only runs on 64-bit Windows
echo   - First launch may be slow (loading resources), this is normal
echo   - If antivirus flags it, add to trust list
echo.
pause
