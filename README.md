# DesktopPet 桌面宠物项目

一个基于Qt6和C++的跨平台桌面宠物应用程序。

## 📋 系统要求

- **CMake** >= 3.14
- **Qt6** (推荐 6.5 或更高版本)
- **C++17** 兼容的编译器：
  - Windows: MinGW 或 MSVC
  - Linux: GCC 或 Clang
  - macOS: Clang

## 🚀 快速开始

### 1. 安装依赖

#### Windows
- 下载并安装 [Qt6](https://www.qt.io/download-qt-installer)
- 确保Qt的bin目录在系统PATH中，或设置环境变量：
  ```powershell
  # 在PowerShell中设置环境变量（请替换为您的实际Qt安装路径）
  $env:PATH = "<您的Qt工具路径>\mingw_64\bin;" + $env:PATH
  $env:CMAKE_PREFIX_PATH = "<您的Qt安装路径>\6.x.x\mingw_64"
  
  # 示例：
  # $env:PATH = "C:\Qt\Tools\mingw1310_64\bin;" + $env:PATH
  # $env:CMAKE_PREFIX_PATH = "C:\Qt\6.9.1\mingw_64"
  
  # 或在CMD中设置
  set CMAKE_PREFIX_PATH=<您的Qt安装路径>\6.x.x\mingw_64
  ```

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential
```

#### macOS
```bash
brew install qt6 cmake
```

### 2. 克隆并构建项目

```bash
# 克隆项目
git clone <repository-url>
cd DesktopPet

# Windows环境配置（使用MinGW）
# 1. 设置环境变量（请替换为您的实际Qt安装路径）
$env:PATH = "<您的Qt工具路径>\mingw_64\bin;" + $env:PATH
$c

# 2. 使用CMake预设构建
cmake --preset=mingw -DCMAKE_PREFIX_PATH="<您的Qt安装路径>\6.x.x\mingw_64"
cmake --build --preset=mingw

# 或者手动构建
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="<您的Qt安装路径>\6.x.x\mingw_64"
cmake --build . --config Release

# 示例（请根据实际情况修改路径）：
# cmake --preset=mingw -DCMAKE_PREFIX_PATH="C:\Qt\6.9.1\mingw_64"
```

### 3. 运行程序

```bash
# Windows
./build/DesktopPet.exe

# Linux/macOS
./build/DesktopPet
```

## 🛠️ 开发环境配置

### VS Code (推荐)

1. 安装必要扩展：
   - C/C++ Extension Pack
   - CMake Tools
   - Qt for Python (可选，用于语法高亮)

2. 打开项目文件夹，CMake Tools会自动检测配置

3. 选择合适的Kit：
   - Windows: 选择Qt6 MinGW或MSVC
   - Linux: 选择GCC
   - macOS: 选择Clang

### Qt Creator

1. 打开 `CMakeLists.txt` 作为项目文件
2. 配置Kit并构建

## 🏗️ 构建选项

项目提供了多种构建预设，特别针对Windows MinGW环境优化：

- `mingw`: MinGW构建（推荐Windows用户使用）
- `default`: 默认Release构建
- `debug`: Debug构建，包含调试信息

使用特定预设：
```bash
# Windows MinGW构建（推荐）
cmake --preset=mingw -DCMAKE_PREFIX_PATH="<您的Qt安装路径>\6.x.x\mingw_64"
cmake --build --preset=mingw

# 示例：
# cmake --preset=mingw -DCMAKE_PREFIX_PATH="C:\Qt\6.9.1\mingw_64"

# Debug构建
cmake --preset=debug -DCMAKE_PREFIX_PATH="<您的Qt安装路径>\6.x.x\mingw_64"
cmake --build --preset=debug
```

## 🧪 运行测试

项目支持单元测试和集成测试，测试基于Google Test框架。

```bash
# 构建所有测试
cmake --build build --target DesktopPetAllTests

# 运行单元测试
./build/DesktopPetUnitTests

# 运行集成测试  
./build/DesktopPetIntegrationTests

# 运行所有测试
./build/DesktopPetAllTests

# 或通过CTest运行
ctest -C Release -V
```

## 📦 部署

### Windows部署
项目已配置自动DLL部署功能。构建完成后，windeployqt工具会自动复制所需的Qt DLL文件。

#### 自动部署（推荐）
```bash
# 使用CMake构建时会自动部署DLL
cmake --build build-mingw

# 或使用一键设置脚本
setup-dev-env.bat
```

#### 手动部署
```bash
# 使用生成的部署脚本
.\build-mingw\deploy-windows.bat

# 或直接使用windeployqt
windeployqt.exe .\build-mingw\DesktopPet.exe
```

### Linux部署
```bash
# 使用linuxdeployqt或手动安装Qt运行时
sudo apt install qt6-base-runtime
```

### macOS部署
```bash
# 使用macdeployqt
macdeployqt ./build/DesktopPet.app
```

## 🔧 常见问题

### Qt找不到
- 确保Qt安装正确
- 设置 `CMAKE_PREFIX_PATH` 环境变量指向Qt安装目录：
  ```powershell
  # PowerShell
  $env:CMAKE_PREFIX_PATH = "D:\ProgramFiles\Qt\6.9.1\mingw_64"
  
  # 或在CMake配置时指定
  cmake -DCMAKE_PREFIX_PATH="D:\ProgramFiles\Qt\6.9.1\mingw_64" ..
  ```

### 编译器错误
- 确保使用C++17兼容的编译器
- Windows用户建议使用Qt安装包自带的MinGW
- 确保MinGW在PATH中：
  ```powershell
  $env:PATH = "D:\ProgramFiles\Qt\Tools\mingw1310_64\bin;" + $env:PATH
  ```

### 缺少DLL (Windows)
- 运行 `windeployqt` 工具
- 或手动复制Qt DLL文件到可执行文件目录

### CMake配置失败
- 检查环境变量设置是否正确
- 确保Qt路径没有拼写错误
- 尝试清理build目录后重新配置：
  ```bash
  Remove-Item build -Recurse -Force
  cmake --preset=mingw -DCMAKE_PREFIX_PATH="D:\ProgramFiles\Qt\6.9.1\mingw_64"
  ```

## 🏗️ 项目结构

```text
DesktopPet/
├── src/                    # 源代码
│   ├── app/               # 应用程序入口
│   ├── model/             # 数据模型
│   ├── view/              # 视图层
│   ├── viewmodel/         # 视图模型层
│   └── common/            # 公共工具
├── test/                   # 测试文件
│   ├── unit/              # 单元测试
│   └── integration/       # 集成测试
├── resources/             # 资源文件
├── CMakeLists.txt         # CMake配置
├── CMakePresets.json      # CMake预设
└── README.md             # 本文件
```

## 🤝 贡献指南

1. Fork 本项目
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add some amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建 Pull Request

## 📄 许可证

本项目采用 [MIT License](LICENSE) 许可证。