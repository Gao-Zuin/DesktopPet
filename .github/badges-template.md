# GitHub Actions 状态徽章模板

将以下徽章代码添加到你的主 README.md 文件中：

```markdown
<!-- GitHub Actions 状态徽章 -->
![CI/CD](https://github.com/your-username/DesktopPet/workflows/CI/CD%20Pipeline/badge.svg)
![Quick Test](https://github.com/your-username/DesktopPet/workflows/Quick%20Test/badge.svg)
![Release](https://github.com/your-username/DesktopPet/workflows/Auto%20Release/badge.svg)
![Security](https://github.com/your-username/DesktopPet/workflows/Security%20%26%20Dependencies/badge.svg)
![Performance](https://github.com/your-username/DesktopPet/workflows/Performance%20Tests/badge.svg)

<!-- 发布信息徽章 -->
![GitHub release (latest by date)](https://img.shields.io/github/v/release/your-username/DesktopPet)
![GitHub Release Date](https://img.shields.io/github/release-date/your-username/DesktopPet)
![GitHub Downloads](https://img.shields.io/github/downloads/your-username/DesktopPet/total)

<!-- 代码质量徽章 -->
![GitHub language count](https://img.shields.io/github/languages/count/your-username/DesktopPet)
![GitHub top language](https://img.shields.io/github/languages/top/your-username/DesktopPet)
![Lines of code](https://img.shields.io/tokei/lines/github/your-username/DesktopPet)

<!-- 项目活跃度徽章 -->
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/your-username/DesktopPet)
![GitHub last commit](https://img.shields.io/github/last-commit/your-username/DesktopPet)
![GitHub contributors](https://img.shields.io/github/contributors/your-username/DesktopPet)
```

## 使用说明

1. **替换用户名**: 将 `your-username` 替换为你的 GitHub 用户名
2. **选择需要的徽章**: 你可以选择性地使用上述徽章，不需要全部添加
3. **自定义样式**: 可以添加 `?style=flat-square` 等参数来改变徽章样式

## 徽章说明

- **CI/CD**: 显示主构建流水线状态
- **Quick Test**: 显示快速测试状态
- **Release**: 显示发布流水线状态
- **Security**: 显示安全检查状态
- **Performance**: 显示性能测试状态
- **GitHub release**: 显示最新版本号
- **Downloads**: 显示总下载次数
- **Language**: 显示主要编程语言
- **Commit activity**: 显示项目活跃度

## 示例效果

徽章会显示为：
- 🟢 绿色：通过/成功
- 🔴 红色：失败/错误
- 🟡 黄色：进行中/警告
- 🔵 蓝色：信息性内容

## 高级自定义

你也可以使用 [Shields.io](https://shields.io/) 创建自定义徽章：

```markdown
![Custom Badge](https://img.shields.io/badge/Qt-6.7.0-green)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)
![License](https://img.shields.io/badge/license-MIT-blue)
```
