#!/bin/bash
# DesktopPet CI/CD 快速部署脚本

echo "🚀 DesktopPet CI/CD 快速部署向导"
echo "================================="
echo ""

# 检查是否在 git 仓库中
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo "❌ 错误: 当前目录不是 Git 仓库"
    echo "请先初始化 Git 仓库: git init"
    exit 1
fi

# 检查是否有未提交的更改
if ! git diff-index --quiet HEAD --; then
    echo "⚠️  警告: 发现未提交的更改"
    echo "建议先提交现有更改再继续"
    echo ""
    read -p "是否继续? (y/N): " -n 1 -r
    echo ""
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo "📋 即将执行的步骤:"
echo "1. 创建功能分支 'feature/setup-github-actions'"
echo "2. 添加 GitHub Actions 配置文件"
echo "3. 提交并推送到远程仓库"
echo "4. 提供后续步骤指导"
echo ""

read -p "按 Enter 继续，或 Ctrl+C 取消..."

# 步骤 1: 确保在最新的 main 分支
echo ""
echo "📍 步骤 1: 更新 main 分支..."
git checkout main
git pull origin main 2>/dev/null || echo "   (如果是第一次推送可忽略此错误)"

# 步骤 2: 创建功能分支
echo ""
echo "🌿 步骤 2: 创建功能分支..."
git checkout -b feature/setup-github-actions

# 步骤 3: 检查文件是否存在
echo ""
echo "📁 步骤 3: 检查 GitHub Actions 文件..."
required_files=(
    ".github/workflows/ci.yml"
    ".github/workflows/quick-test.yml"
    ".github/workflows/release.yml"
    ".github/workflows/security.yml"
    ".github/workflows/performance.yml"
)

missing_files=()
for file in "${required_files[@]}"; do
    if [ ! -f "$file" ]; then
        missing_files+=("$file")
    fi
done

if [ ${#missing_files[@]} -ne 0 ]; then
    echo "❌ 缺少以下文件:"
    for file in "${missing_files[@]}"; do
        echo "   - $file"
    done
    echo ""
    echo "请确保所有 GitHub Actions 文件都已创建"
    exit 1
fi

echo "✅ 所有必需文件都存在"

# 步骤 4: 添加文件到 Git
echo ""
echo "📦 步骤 4: 添加文件到 Git..."
git add .github/
git add check-ci-setup.* 2>/dev/null || true
git add .gitignore
git add CI-CD-DEPLOYMENT-GUIDE.md 2>/dev/null || true

# 步骤 5: 提交更改
echo ""
echo "💾 步骤 5: 提交更改..."
git commit -m "feat: Add GitHub Actions CI/CD pipeline

🎯 新增功能:
- 多平台自动构建 (Windows MinGW/MSVC, Linux, macOS)
- 自动化测试 (单元测试 + 集成测试)
- 快速测试工作流 (用于功能分支)
- 自动发布系统 (版本标签触发)
- 安全扫描和代码质量检查
- 性能测试工作流

🔧 配置文件:
- .github/workflows/ci.yml - 主 CI/CD 流水线
- .github/workflows/quick-test.yml - 快速测试
- .github/workflows/release.yml - 自动发布
- .github/workflows/security.yml - 安全检查
- .github/workflows/performance.yml - 性能测试

📚 文档:
- 完整的部署指南和使用说明
- 配置检查脚本"

# 步骤 6: 推送到远程
echo ""
echo "🚀 步骤 6: 推送到远程仓库..."
if git push origin feature/setup-github-actions; then
    echo "✅ 成功推送到远程仓库"
else
    echo "❌ 推送失败，请检查网络连接和权限"
    exit 1
fi

# 后续步骤指导
echo ""
echo "🎉 GitHub Actions 配置已成功推送!"
echo ""
echo "📋 接下来的步骤:"
echo ""
echo "1. 🌐 创建 Pull Request:"
echo "   - 访问: https://github.com/$(git config --get remote.origin.url | sed 's/.*github.com[:/]\(.*\)\.git/\1/')"
echo "   - 点击 'Compare & pull request'"
echo "   - 标题: 'Add GitHub Actions CI/CD Pipeline'"
echo ""
echo "2. 👀 观察 CI 运行:"
echo "   - PR 创建后会自动触发 CI 检查"
echo "   - 在 'Actions' 标签页查看运行状态"
echo "   - 如果有红色❌，点击查看错误详情"
echo ""
echo "3. 🐛 如果有错误:"
echo "   - 在本地修复问题"
echo "   - git add . && git commit -m 'fix: 修复 CI 问题'"
echo "   - git push origin feature/setup-github-actions"
echo ""
echo "4. ✅ 当所有检查通过后:"
echo "   - 在 GitHub 上合并 PR"
echo "   - 删除功能分支: git branch -d feature/setup-github-actions"
echo ""
echo "5. 🏷️  测试发布流程 (可选):"
echo "   - git checkout main && git pull"
echo "   - git tag v1.0.0 && git push origin v1.0.0"
echo "   - 观察自动发布过程"
echo ""
echo "💡 提示:"
echo "- 第一次运行 CI 可能需要 10-15 分钟"
echo "- 某些工作流可能需要根据项目调整"
echo "- 查看详细指南: CI-CD-DEPLOYMENT-GUIDE.md"
echo ""
echo "🎯 当前分支: $(git branch --show-current)"
echo "📊 查看状态: git status"

# 提供快捷命令
echo ""
echo "🔧 快捷命令:"
echo "查看 CI 配置: ./check-ci-setup.sh"
echo "返回 main 分支: git checkout main"
echo "查看所有分支: git branch -a"
