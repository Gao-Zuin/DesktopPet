# DesktopPet 平台支持说明

## 🖥️ 支持的平台

### 当前支持
- **Windows 10/11** ✅
- **MinGW-w64** ✅
- **Qt 6.7.0+** ✅

### 构建工具链
- **编译器**: MinGW-w64 GCC
- **构建系统**: CMake 3.14+
- **UI框架**: Qt6

### 为什么只支持 Windows MinGW？

1. **项目定位**: 桌面宠物应用主要面向 Windows 用户
2. **开发环境**: 团队主要使用 Windows 开发环境
3. **Qt 许可**: MinGW 版本的 Qt 可以免费使用
4. **部署简单**: Windows 平台部署相对简单

## 🔧 开发环境要求

### 必需软件
```
Windows 10/11 (x64)
├── Qt 6.7.0 (MinGW 64-bit)
├── MinGW-w64 GCC 9.0+
├── CMake 3.14+
└── Git
```

### 推荐的 IDE
- **Qt Creator** - 最佳Qt开发体验
- **Visual Studio Code** - 通用代码编辑器
- **CLion** - JetBrains IDE

## 🚀 快速开始

### 1. 环境设置
```bash
# 运行环境设置脚本
.\setup-dev-env.bat
```

### 2. 构建项目
```bash
# 配置
cmake --preset=mingw

# 构建
cmake --build build-mingw --config Release
```

### 3. 运行测试
```bash
cd build-mingw
ctest --output-on-failure
```

## 📦 发布说明

### 发布包内容
- `DesktopPet.exe` - 主程序
- Qt6 运行时库 (自动包含)
- MinGW 运行时库
- 资源文件
- 用户手册

### 系统要求
- Windows 10 version 1903 或更高
- 64位系统
- 至少 100MB 可用空间

## 🔮 未来计划

### 可能的扩展
- **Linux 支持**: 如果有足够的用户需求
- **macOS 支持**: 需要相应的开发环境
- **移动平台**: Qt 支持 Android/iOS

### 技术考虑
- **跨平台兼容性**: 代码设计考虑跨平台
- **条件编译**: 使用 `#ifdef` 处理平台差异
- **抽象层**: 平台相关代码进行抽象

## 🤝 贡献指南

### 如果你想添加其他平台支持

1. **创建 Issue** 讨论需求
2. **提供测试环境** 确保能够测试
3. **修改 CMakeLists.txt** 添加平台检测
4. **更新 CI/CD** 添加对应的构建流程
5. **更新文档** 说明新平台的要求

### 代码贡献原则
- 保持 Windows 平台的兼容性
- 添加适当的平台检测
- 更新相关文档
- 添加相应的测试

## 📞 技术支持

### 遇到问题？
1. 查看 [FAQ](docs/FAQ.md)
2. 检查 [已知问题](docs/KNOWN-ISSUES.md)
3. 创建 [Issue](https://github.com/Gao-Zuin/DesktopPet/issues)

### 环境问题排查
```bash
# 运行环境检查
.\check-ci-setup.bat

# 查看详细构建信息
cmake --preset=mingw --debug-output
```

## 📊 平台使用统计

> 注意: 这些是假设的统计数据，仅供参考

- Windows 10/11: 95%
- 其他平台: 5%

基于用户反馈，我们优先支持 Windows 平台。

---

*如果你需要在其他平台上使用 DesktopPet，请创建 Issue 告诉我们你的需求！*
