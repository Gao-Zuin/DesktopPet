# DesktopPet 桌面宠物项目

一个基于Qt6.9.1和C++17的Windows桌面宠物应用程序，采用MVVM架构设计，支持丰富的宠物交互功能。

## ✨ 项目特色

### 🎮 核心功能
- **桌面宠物显示**：支持GIF动画和静态图片，窗口透明无边框，自然融入桌面
- **自由拖拽移动**：鼠标拖拽宠物自由移动，支持自动移动和桌面图标交互
- **多种宠物形态**：支持小蜘蛛耄耋、卡西迪耄耋等多种宠物，可随时切换
- **丰富的数值系统**：经验值、等级、金钱系统，支持升级成长

### 🎯 游戏玩法
- **打工系统**：光合作用、挖矿、冒险三种工作类型，获得经验和材料
- **背包系统**：4×4网格背包，支持物品收集、分类和详细信息显示
- **图鉴系统**：完整的物品图鉴，按类别和稀有度分类展示
- **锻造升级**：使用材料升级工作系统，提高效率和产出
- **自动移动**：智能桌面漫游，与桌面图标产生有趣交互

## 📋 系统要求

- **操作系统**: Windows 10/11 (仅支持Windows平台)
- **CMake** >= 3.14
- **Qt6.9.1** (MinGW 64位版本)
- **C++17** 兼容的编译器 (推荐MinGW)

## 🚀 快速开始

### 1. 环境准备

#### 安装Qt6.9.1
1. 下载并安装 [Qt6.9.1](https://www.qt.io/download-qt-installer)
2. 安装时选择：
   - Qt 6.9.1
   - MinGW 13.1.0 64-bit
   - CMake

#### 配置环境变量
在PowerShell中设置以下环境变量：

```powershell
# 设置Qt工具链路径
$env:PATH = "D:\ProgramFiles\Qt\Tools\mingw1310_64\bin;" + $env:PATH

# 设置CMake前缀路径
$env:CMAKE_PREFIX_PATH = "D:\ProgramFiles\Qt\6.9.1\mingw_64"
```

**注意**: 请根据您的实际Qt安装路径调整上述路径。

### 2. 构建项目

```powershell
# 克隆项目
git clone https://github.com/Gao-Zuin/DesktopPet.git
cd DesktopPet

# 使用CMake预设构建
cmake --preset=mingw -DCMAKE_PREFIX_PATH="D:\ProgramFiles\Qt\6.9.1\mingw_64"

cmake --build --preset=mingw
```

### 3. 运行程序

```powershell
# 运行桌面宠物
.\build-mingw\DesktopPet.exe
```

程序启动后：
- 桌面将出现默认的小蜘蛛耄耋
- 右键点击宠物打开菜单
- 可以拖拽移动宠物位置
- 通过菜单访问各种功能面板

## 🎮 功能指南

### 基础操作
- **移动宠物**: 鼠标左键拖拽
- **菜单操作**: 鼠标右键点击宠物
- **切换宠物**: 右键菜单 → 切换到小蜘蛛/卡西迪
- **查看面板**: 右键菜单 → 各种功能面板

### 主要功能面板

#### 📊 数值面板
- 查看宠物等级、经验值、金钱
- 测试增加经验和金钱功能

#### 🎒 背包系统  
- 4×4网格背包，最多16个物品
- 鼠标悬浮查看物品详细信息
- 点击物品使用（如果支持）

#### 📖 图鉴系统
- 按类别浏览物品：材料、物品、皮肤、成就
- 按稀有度筛选：普通、稀有、史诗、传说
- 查看收集进度和统计信息

#### 💼 打工系统
- **光合作用**: 获得阳光材料和经验
- **挖矿作业**: 获得矿石材料和经验  
- **冒险探索**: 获得木材材料和经验
- 查看工作进度和剩余时间

#### ⚒️ 锻造升级
- 使用材料升级工作系统
- 提高工作效率和产出
- 查看升级所需材料

### 高级功能

#### 🎯 自动移动
- 右键菜单 → 启用随机移动
- 宠物会在桌面随机漫游
- 遇到桌面图标时会产生交互动画

## 🏗️ 构建选项

项目针对Windows MinGW环境进行了优化，提供以下构建预设：

### 可用预设
- `mingw`: 标准MinGW构建（推荐）
- `debug`: Debug构建，包含调试信息

### 使用特定预设

```powershell
# 标准构建（推荐）
cmake --preset=mingw -DCMAKE_PREFIX_PATH="D:\ProgramFiles\Qt\6.9.1\mingw_64"
cmake --build --preset=mingw

# 或使用一键设置脚本，请根据实际情况修改脚本中的路径
.\setup-dev-env.bat
```


## 🔧 常见问题

### Qt路径问题
如果CMake找不到Qt，请检查环境变量设置：
```powershell
# 确保PATH包含MinGW工具链
$env:PATH = "D:\ProgramFiles\Qt\Tools\mingw1310_64\bin;" + $env:PATH

# 确保CMAKE_PREFIX_PATH指向正确的Qt版本
$env:CMAKE_PREFIX_PATH = "D:\ProgramFiles\Qt\6.9.1\mingw_64"
```

### 编译错误
- 确保使用Qt 6.9.1版本
- 确保使用MinGW 13.1.0 64位编译器
- 检查C++17支持是否启用

### 宠物不显示或显示异常
- 检查resources.qrc是否正确加载
- 确保GIF动画文件路径正确
- 检查窗口是否被其他应用遮挡

## 🏗️ 项目架构

### 代码结构
```text
DesktopPet/
├── src/                    # 源代码
│   ├── app/               # 应用程序入口和主逻辑
│   ├── model/             # 数据模型层 (PetModel, WorkModel, BackpackModel等)
│   ├── view/              # 视图层 (主窗口, 各功能面板)
│   ├── viewmodel/         # 视图模型层 (PetViewModel, 命令管理)
│   └── common/            # 公共组件
├── test/                  # 测试文件
│   ├── unit/              # 单元测试
│   └── integration/       # 集成测试
├── resources/             # 资源文件
│   ├── gif/               # 宠物动画文件
│   ├── img/               # 静态图片资源
│   └── csv/               # 配置数据文件
├── build-mingw/           # MinGW构建输出
├── CMakeLists.txt         # CMake配置
├── CMakePresets.json      # CMake预设
└── README.md             # 本文件
```

## 🎯 开发路线图

### 已完成功能 ✅
- [x] 基础桌面宠物显示系统
- [x] 鼠标拖拽和右键菜单交互
- [x] 多宠物形态切换
- [x] 完整的MVVM架构实现
- [x] 数值系统（等级、经验、金钱）
- [x] 打工系统（光合作用、挖矿、冒险）
- [x] 背包和物品管理系统
- [x] 图鉴收集系统
- [x] 锻造升级系统
- [x] 自动移动和桌面交互
- [x] 数据持久化存储

### 计划中功能 🔄
- [ ] 更多宠物类型和动画
- [ ] 音效系统支持
- [ ] 宠物状态系统（饥饿、疲劳等）
- [ ] 自定义宠物编辑器
- [ ] 社交分享功能
- [ ] 主题皮肤系统

## 🤝 贡献指南

我们欢迎任何形式的贡献！

### 如何贡献
1. Fork 本项目
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 遵循项目的代码规范和架构设计
4. 添加必要的测试用例
5. 提交更改 (`git commit -m 'Add some amazing feature'`)
6. 推送到分支 (`git push origin feature/amazing-feature`)
7. 创建 Pull Request

### 代码规范
- 使用C++17标准
- 遵循MVVM架构模式
- 添加适当的注释和文档
- 确保所有测试通过

### 问题反馈
如果您发现bug或有功能建议，请在GitHub Issues中提交。

## 📄 许可证

本项目采用 [MIT License](LICENSE) 许可证。

---

## 💝 致谢

感谢所有为本项目做出贡献的开发者和用户！

如果您喜欢这个项目，请给它一个 ⭐️！