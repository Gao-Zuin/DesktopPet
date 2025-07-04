# GitHub Actions CI/CD 配置说明

本项目包含完整的 GitHub Actions CI/CD 流水线，提供自动化构建、测试、安全检查和发布功能。

## 🔄 工作流概览

### 1. 主 CI/CD 流水线 (`ci.yml`)
- **触发条件**: 推送到 main/develop/feature 分支，PR 到 main/develop
- **功能**:
  - 多平台构建 (Windows MinGW/MSVC, Linux, macOS)
  - 自动化测试 (单元测试 + 集成测试)
  - 代码质量检查
  - 自动部署 Qt 依赖
  - 生成发布包

### 2. 快速测试 (`quick-test.yml`)
- **触发条件**: 推送到 feature/develop 分支，PR
- **功能**:
  - 快速构建和基本测试
  - 使用缓存加速构建
  - 仅运行单元测试
  - 适用于开发阶段的快速反馈

### 3. 自动发布 (`release.yml`)
- **触发条件**: 推送版本标签 (如 `v1.0.0`)
- **功能**:
  - 创建 GitHub Release
  - 多平台构建和打包
  - 自动上传发布包
  - 生成发布说明

### 4. 安全和依赖检查 (`security.yml`)
- **触发条件**: 定期执行，推送到 main，PR
- **功能**:
  - 依赖漏洞扫描
  - 代码格式检查
  - 静态代码分析
  - 许可证检查
  - 生成软件物料清单 (SBOM)

### 5. 性能测试 (`performance.yml`)
- **触发条件**: 推送到 main/develop，定期执行
- **功能**:
  - 内存使用测试
  - 启动时间测试
  - 动画性能测试
  - 生成性能报告

## 🚀 使用指南

### 首次设置

1. **启用 GitHub Actions**
   ```bash
   # 提交 .github/workflows 目录到仓库
   git add .github/
   git commit -m "Add GitHub Actions CI/CD workflows"
   git push origin main
   ```

2. **配置仓库设置**
   - 进入仓库设置 → Actions → General
   - 确保 "Allow all actions and reusable workflows" 已启用

### 开发工作流

1. **特性开发**
   ```bash
   git checkout -b feature/new-feature
   # 开发代码...
   git push origin feature/new-feature
   ```
   → 触发 `quick-test.yml` 进行快速验证

2. **创建 Pull Request**
   ```bash
   # 在 GitHub 上创建 PR 到 main 或 develop
   ```
   → 触发 `ci.yml` 进行完整测试

3. **发布新版本**
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```
   → 触发 `release.yml` 自动发布

### 构建输出

每个工作流都会生成相应的构件：

- **Windows MinGW**: `DesktopPet-Windows-MinGW-x64.zip`
- **Windows MSVC**: `DesktopPet-Windows-MSVC-x64.zip`
- **Linux**: `DesktopPet-Linux-x64.tar.gz`
- **macOS**: `DesktopPet-macOS-x64.tar.gz`

## 📊 监控和维护

### 查看构建状态

在 GitHub 仓库页面可以看到：
- ✅ 绿色：所有检查通过
- ❌ 红色：有测试失败
- 🟡 黄色：正在运行中

### 徽章 (Badges)

可以在 README.md 中添加状态徽章：

```markdown
![CI](https://github.com/your-username/DesktopPet/workflows/CI/CD%20Pipeline/badge.svg)
![Release](https://github.com/your-username/DesktopPet/workflows/Auto%20Release/badge.svg)
![Security](https://github.com/your-username/DesktopPet/workflows/Security%20%26%20Dependencies/badge.svg)
```

### 故障排除

1. **构建失败**
   - 查看 Actions 标签页的详细日志
   - 检查依赖版本是否兼容
   - 验证代码格式是否符合要求

2. **测试失败**
   - 查看测试输出和错误信息
   - 在本地运行相同的测试
   - 检查平台特定的问题

3. **部署问题**
   - 确认 Qt 版本兼容性
   - 检查 windeployqt 输出
   - 验证所有依赖库都已正确部署

## 🔧 自定义配置

### 修改 Qt 版本
在工作流文件中修改 `QT_VERSION` 环境变量：
```yaml
env:
  QT_VERSION: 6.8.0  # 更新到新版本
```

### 添加新的构建目标
在 `ci.yml` 的 matrix 中添加新配置：
```yaml
matrix:
  include:
    - os: ubuntu-20.04  # 添加 Ubuntu 20.04 支持
      qt_version: '6.7.0'
```

### 自定义测试配置
修改测试命令：
```yaml
- name: Run tests
  run: |
    cd build
    ctest --output-on-failure --parallel --timeout 300
```

## 📝 最佳实践

1. **分支策略**
   - `main`: 稳定版本，受保护
   - `develop`: 开发分支，集成测试
   - `feature/*`: 特性分支，快速测试

2. **提交规范**
   - 使用清晰的提交信息
   - 遵循 [Conventional Commits](https://www.conventionalcommits.org/) 规范

3. **版本标记**
   - 使用语义化版本 (SemVer): `v1.0.0`
   - 主版本.次版本.修订版本

4. **测试覆盖**
   - 确保新功能有对应测试
   - 维持高测试覆盖率 (>80%)

## 🎯 下一步优化

- [ ] 添加代码覆盖率报告
- [ ] 集成更多静态分析工具
- [ ] 添加端到端测试
- [ ] 实现自动更新机制
- [ ] 添加性能基准测试
- [ ] 配置通知机制 (Slack/Discord)

---

这套 CI/CD 流水线为 DesktopPet 项目提供了完整的自动化支持，确保代码质量和发布稳定性。如有问题或改进建议，请创建 Issue 讨论。
