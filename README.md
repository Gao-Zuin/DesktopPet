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
  ```bash
  set CMAKE_PREFIX_PATH=C:\Qt\6.x.x\mingw_64
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

# 使用CMake Presets构建（推荐）
cmake --preset=default
cmake --build --preset=default

# 或者手动构建
mkdir build && cd build
cmake ..
cmake --build . --config Release
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

项目提供了多种构建预设：

- `default`: 默认Release构建
- `debug`: Debug构建，包含调试信息
- `mingw`: 专门为MinGW优化的构建
- `msvc`: 专门为MSVC优化的构建

使用特定预设：
```bash
cmake --preset=debug
cmake --build --preset=debug
```

## 🧪 运行测试

```bash
# 构建测试
cmake --build build --target DesktopPetTests

# 运行测试
./build/DesktopPetTests
```

## 📦 部署

### Windows
项目已配置自动DLL部署功能。构建完成后，windeployqt工具会自动复制所需的Qt DLL文件。

#### 自动部署（推荐）：
```bash
# 使用CMake构建时会自动部署DLL
cmake --build build-mingw

# 或使用一键设置脚本
setup-dev-env.bat
```

#### 手动部署：
```bash
# 使用生成的部署脚本
.\build-mingw\deploy-windows.bat

# 或直接使用windeployqt
windeployqt.exe .\build-mingw\DesktopPet.exe
```

### Linux
```bash
# 使用linuxdeployqt或手动安装Qt运行时
sudo apt install qt6-base-runtime
```

### macOS
```bash
# 使用macdeployqt
macdeployqt ./build/DesktopPet.app
```

## 🔧 常见问题

### Qt找不到
- 确保Qt安装正确
- 设置 `CMAKE_PREFIX_PATH` 环境变量指向Qt安装目录
- 或在CMake配置时指定：`cmake -DCMAKE_PREFIX_PATH=/path/to/qt6 ..`

### 编译器错误
- 确保使用C++17兼容的编译器
- Windows用户建议使用Qt安装包自带的MinGW

### 缺少DLL (Windows)
- 运行 `windeployqt` 工具
- 或手动复制Qt DLL文件到可执行文件目录

## 🏗️ 项目结构

```
DesktopPet/
├── src/                    # 源代码
│   ├── app/               # 应用程序入口
│   ├── model/             # 数据模型
│   ├── view/              # 视图层
│   ├── viewmodel/         # 视图模型层
│   └── common/            # 公共工具
├── test/                   # 单元测试
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