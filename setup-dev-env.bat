@echo off
REM DesktopPet 开发环境设置脚本 (Windows)
setlocal enabledelayedexpansion

echo 🚀 DesktopPet 开发环境设置
echo ==========================

REM 检查CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo ❌ 请先安装 CMake: https://cmake.org/download/
    pause
    exit /b 1
) else (
    echo ✅ CMake 已安装
)

REM 尝试自动检测Qt安装
set QT_FOUND=0
for %%d in (C D E F) do (
    if exist "%%d:\Qt" (
        for /d %%v in ("%%d:\Qt\6.*") do (
            if exist "%%v\mingw*" (
                for /d %%k in ("%%v\mingw*") do (
                    set CMAKE_PREFIX_PATH=%%k
                    set QT_FOUND=1
                    echo ✅ 发现Qt安装: %%k
                    goto :qt_found
                )
            )
        )
    )
    if exist "%%d:\ProgramFiles\Qt" (
        for /d %%v in ("%%d:\ProgramFiles\Qt\6.*") do (
            if exist "%%v\mingw*" (
                for /d %%k in ("%%v\mingw*") do (
                    set CMAKE_PREFIX_PATH=%%k
                    set QT_FOUND=1
                    echo ✅ 发现Qt安装: %%k
                    goto :qt_found
                )
            )
        )
    )
)

:qt_found
if %QT_FOUND%==0 (
    echo ❌ 未找到Qt6安装
    echo 请确保Qt6已安装并设置环境变量CMAKE_PREFIX_PATH
    echo 例如: set CMAKE_PREFIX_PATH=C:\Qt\6.7.0\mingw_64
    pause
    exit /b 1
)

REM 检查编译器
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo ⚠️  系统PATH中未找到gcc，尝试使用Qt自带的MinGW...
    if exist "%CMAKE_PREFIX_PATH%\..\..\..\Tools\mingw*\bin\gcc.exe" (
        for /d %%t in ("%CMAKE_PREFIX_PATH%\..\..\..\Tools\mingw*") do (
            set PATH=%%t\bin;!PATH!
            echo ✅ 使用Qt自带的MinGW: %%t\bin
            goto :compiler_found
        )
    )
    echo ❌ 未找到合适的编译器
    pause
    exit /b 1
) else (
    echo ✅ 编译器已找到
)

:compiler_found

REM 创建构建目录
echo 创建构建目录...
if not exist "build" mkdir build

REM 配置项目
echo 配置项目...
set CMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%
cmake --preset=mingw
if %errorlevel% neq 0 (
    echo 预设配置失败，尝试传统配置方式...
    cd build
    cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX_PATH%" ..
    cd ..
    if !errorlevel! neq 0 (
        echo ❌ 项目配置失败
        pause
        exit /b 1
    )
)

REM 构建项目
echo 正在构建项目...
cmake --build build-mingw --config Release
if %errorlevel% neq 0 (
    echo ❌ 项目构建失败
    pause
    exit /b 1
)

echo ✅ 项目构建成功！

REM 自动部署DLL
echo 正在部署Qt DLL文件...
if exist "build-mingw\DesktopPet.exe" (
    windeployqt "build-mingw\DesktopPet.exe" --verbose 2
    if !errorlevel! equ 0 (
        echo ✅ 主程序DLL部署成功
    ) else (
        echo ⚠️ 主程序DLL部署可能有问题，但程序可能仍可运行
    )
)

if exist "build-mingw\DesktopPetTests.exe" (
    windeployqt "build-mingw\DesktopPetTests.exe" --verbose 2
    if !errorlevel! equ 0 (
        echo ✅ 测试程序DLL部署成功
    ) else (
        echo ⚠️ 测试程序DLL部署可能有问题
    )
)

echo.
echo 🎉 环境设置完成！
echo.
echo 下一步：
echo 1. 运行主程序: .\build-mingw\DesktopPet.exe
echo 2. 运行测试: .\build-mingw\DesktopPetTests.exe
echo 3. 如需重新部署DLL: .\build-mingw\deploy-windows.bat
echo.
pause