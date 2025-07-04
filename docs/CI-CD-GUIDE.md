# DesktopPet CI/CD 持续集成指南

## 📋 目录
- [CI/CD 概述](#cicd-概述)
- [分支管理策略](#分支管理策略)
- [工作流程说明](#工作流程说明)
- [如何使用 CI/CD](#如何使用-cicd)
- [故障排除](#故障排除)
- [最佳实践](#最佳实践)

## 🔄 CI/CD 概述

### 什么是 CI/CD？
**持续集成（CI）**：开发者频繁地将代码集成到主干，每次集成都通过自动化构建和测试来验证。
**持续部署（CD）**：自动化地将已验证的代码部署到生产环境。

### 我们的 CI/CD 流水线包含
- ✅ **自动构建**：多平台（Windows/Linux/macOS）
- ✅ **自动测试**：单元测试、集成测试
- ✅ **代码质量检查**：静态分析、格式检查
- ✅ **安全扫描**：漏洞检测、依赖安全检查
- ✅ **性能测试**：构建性能、运行时测试
- ✅ **自动发布**：标签发布、资产管理

## 🌿 分支管理策略

### 分支类型
```text
main (主分支)
├── feature/feature-name (功能分支)
├── hotfix/fix-name (热修复分支)
├── release/version-name (发布分支)
└── develop (开发分支，可选)
```

### 分支命名规范
- `feature/` - 新功能开发
- `bugfix/` - 问题修复
- `hotfix/` - 紧急修复
- `release/` - 发布准备
- `docs/` - 文档更新
- `test/` - 测试相关

### 保护分支设置
- `main` 分支：需要 PR 审查，CI 通过后才能合并
- 不允许直接推送到 `main` 分支
- 需要线性历史记录（squash merge）

## 🚀 工作流程说明

### 1. 快速检查（PR 时触发）
```yaml
# .github/workflows/quick-test.yml
on: [pull_request]
jobs:
  - 代码格式检查
  - 基础编译检查
  - 单元测试
  - 代码覆盖率
```

### 2. 完整 CI（Push 到 main 时触发）
```yaml
# .github/workflows/ci.yml
on: 
  push:
    branches: [main]
jobs:
  - 多平台构建
  - 完整测试套件
  - 性能测试
  - 文档生成
```

### 3. 安全扫描（定期 + PR 时触发）
```yaml
# .github/workflows/security.yml
schedule:
  - cron: '0 2 * * 1' # 每周一凌晨2点
jobs:
  - 依赖漏洞扫描
  - 静态代码安全分析
  - 镜像安全扫描
```

### 4. 发布流程（Tag 时触发）
```yaml
# .github/workflows/release.yml
on:
  push:
    tags: ['v*']
jobs:
  - 构建发布版本
  - 创建 GitHub Release
  - 上传构建产物
```

## 🛠️ 如何使用 CI/CD

### 步骤 1：创建功能分支
```bash
# 从 main 分支创建新分支
git checkout main
git pull origin main
git checkout -b feature/my-new-feature
```

### 步骤 2：开发和提交
```bash
# 进行开发工作
# 提交更改
git add .
git commit -m "feat: add new feature"

# 推送到远程分支
git push origin feature/my-new-feature
```

### 步骤 3：创建 Pull Request
1. 在 GitHub 上创建 PR
2. 等待 CI 检查通过
3. 请求代码审查
4. 修复任何 CI 失败的问题

### 步骤 4：合并到主分支
```bash
# PR 合并后，清理本地分支
git checkout main
git pull origin main
git branch -d feature/my-new-feature
```

### 步骤 5：创建发布
```bash
# 创建发布标签
git tag -a v1.0.0 -m "Release version 1.0.0"
git push origin v1.0.0
```

## 🔧 本地开发环境设置

### 运行 CI 检查脚本
```bash
# Windows
.\check-ci-setup.bat

# Linux/macOS
./check-ci-setup.sh
```

### 本地构建验证
```bash
# 使用 CMake 预设
cmake --preset=mingw
cmake --build --preset=mingw

# 运行测试
ctest --preset=mingw
```

## 🔍 CI/CD 监控和调试

### 查看构建状态
- 在 GitHub Actions 标签页查看工作流运行情况
- 点击具体的工作流查看详细日志
- 查看构建产物和测试报告

### 常见问题和解决方案

#### 问题 1：构建失败
```bash
# 检查构建日志
# 常见原因：依赖缺失、环境配置问题
```

**解决方案**：
1. 检查 CMakeLists.txt 配置
2. 确保所有依赖都在 setup-dev-env.bat 中安装
3. 检查平台特定的构建配置

#### 问题 2：测试失败
```bash
# 本地运行测试
ctest --preset=mingw --verbose
```

**解决方案**：
1. 确保所有测试在本地通过
2. 检查测试数据和资源文件
3. 检查平台特定的测试配置

#### 问题 3：安全扫描告警
**解决方案**：
1. 更新有漏洞的依赖
2. 修复代码中的安全问题
3. 如果是误报，在配置中添加例外

#### 问题 4：性能测试失败
**解决方案**：
1. 检查性能基准设置
2. 优化代码性能
3. 调整性能测试的阈值

## 📊 CI/CD 指标监控

### 关键指标
- **构建成功率**：>95%
- **测试覆盖率**：>80%
- **构建时间**：<10分钟
- **部署频率**：每周至少1次
- **故障恢复时间**：<1小时

### 监控工具
- GitHub Actions 内置统计
- 构建时间趋势
- 测试覆盖率报告
- 性能基准测试

## 🎯 最佳实践

### 代码提交
- 使用约定式提交（Conventional Commits）
- 每次提交应该是一个逻辑完整的更改
- 提交信息要清晰描述更改内容

### 分支管理
- 保持分支生命周期短
- 及时合并到主分支
- 删除已合并的分支

### 测试策略
- 编写单元测试覆盖核心功能
- 集成测试验证组件交互
- 性能测试确保系统稳定性

### 安全考虑
- 不在代码中硬编码敏感信息
- 定期更新依赖
- 使用安全的编码实践

## 📚 延伸阅读

- [GitHub Actions 文档](https://docs.github.com/en/actions)
- [CMake 最佳实践](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
- [Qt6 CI/CD 指南](https://doc.qt.io/qt-6/cmake-get-started.html)
- [C++ 测试最佳实践](https://github.com/google/googletest)

## 🆘 获取帮助

如果遇到问题：
1. 查看 GitHub Actions 日志
2. 检查 CI-CD-DEPLOYMENT-GUIDE.md
3. 运行本地检查脚本
4. 在团队聊天中寻求帮助

---

*这个指南会随着项目的发展而更新。如果您有任何建议或发现问题，请创建 Issue 或 PR。*
