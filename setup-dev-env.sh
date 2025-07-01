#!/bin/bash
# DesktopPet 开发环境设置脚本 (Linux/macOS)

echo "🚀 DesktopPet 开发环境设置"
echo "=========================="

# 检查操作系统
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "检测到 Linux 系统"
    
    # 检查并安装Qt6
    if ! command -v qmake6 &> /dev/null; then
        echo "正在安装 Qt6..."
        sudo apt update
        sudo apt install -y qt6-base-dev qt6-tools-dev cmake build-essential ninja-build
    else
        echo "✅ Qt6 已安装"
    fi
    
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "检测到 macOS 系统"
    
    # 检查Homebrew
    if ! command -v brew &> /dev/null; then
        echo "请先安装 Homebrew: https://brew.sh/"
        exit 1
    fi
    
    # 安装Qt6
    if ! brew list qt6 &> /dev/null; then
        echo "正在安装 Qt6..."
        brew install qt6 cmake ninja
    else
        echo "✅ Qt6 已安装"
    fi
    
    # 设置环境变量
    export CMAKE_PREFIX_PATH="$(brew --prefix qt6)"
    echo "export CMAKE_PREFIX_PATH=\"$(brew --prefix qt6)\"" >> ~/.bashrc
fi

# 检查CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ 请安装 CMake >= 3.14"
    exit 1
else
    echo "✅ CMake 已安装"
fi

# 创建构建目录
echo "创建构建目录..."
mkdir -p build

# 配置项目
echo "配置项目..."
if cmake --preset=default; then
    echo "✅ 项目配置成功"
else
    echo "尝试传统配置方式..."
    cd build && cmake .. && cd ..
fi

echo ""
echo "🎉 环境设置完成！"
echo ""
echo "下一步："
echo "1. 构建项目: cmake --build build"
echo "2. 运行程序: ./build/DesktopPet"
echo "3. 运行测试: ./build/DesktopPetTests"