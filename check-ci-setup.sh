#!/bin/bash
# GitHub Actions 状态检查脚本

echo "🔍 DesktopPet GitHub Actions 配置检查"
echo "====================================="

# 检查工作流文件
echo ""
echo "📋 检查工作流文件..."
workflows_dir=".github/workflows"

if [ ! -d "$workflows_dir" ]; then
    echo "❌ .github/workflows 目录不存在"
    exit 1
fi

declare -A workflows=(
    ["ci.yml"]="主 CI/CD 流水线"
    ["quick-test.yml"]="快速测试"
    ["release.yml"]="自动发布"
    ["security.yml"]="安全检查"
    ["performance.yml"]="性能测试"
)

for workflow in "${!workflows[@]}"; do
    if [ -f "$workflows_dir/$workflow" ]; then
        echo "✅ $workflow - ${workflows[$workflow]}"
    else
        echo "❌ $workflow - ${workflows[$workflow]} (缺失)"
    fi
done

# 检查必要的配置文件
echo ""
echo "🔧 检查配置文件..."
config_files=(
    "CMakeLists.txt:CMake 构建配置"
    "CMakePresets.json:CMake 预设"
    ".gitignore:Git 忽略规则"
)

for config in "${config_files[@]}"; do
    file="${config%%:*}"
    desc="${config##*:}"
    if [ -f "$file" ]; then
        echo "✅ $file - $desc"
    else
        echo "❌ $file - $desc (缺失)"
    fi
done

# 检查源代码结构
echo ""
echo "📁 检查项目结构..."
required_dirs=(
    "src:源代码目录"
    "test:测试代码目录"
    "resources:资源文件目录"
)

for dir_info in "${required_dirs[@]}"; do
    dir="${dir_info%%:*}"
    desc="${dir_info##*:}"
    if [ -d "$dir" ]; then
        echo "✅ $dir/ - $desc"
    else
        echo "❌ $dir/ - $desc (缺失)"
    fi
done

# 检查 GitHub 仓库设置建议
echo ""
echo "⚙️  GitHub 仓库设置建议..."
echo "📌 请确保以下设置已正确配置："
echo "   1. Settings → Actions → General → Actions permissions"
echo "      └─ 允许所有操作和可重用工作流"
echo "   2. Settings → Branches → Branch protection rules"
echo "      └─ 为 main 分支启用保护规则"
echo "   3. Settings → Secrets and variables → Actions"
echo "      └─ 确保 GITHUB_TOKEN 权限足够"

# 检查可选功能
echo ""
echo "🌟 可选功能检查..."

if [ -f "README.md" ]; then
    if grep -q "badge" README.md; then
        echo "✅ README.md 包含状态徽章"
    else
        echo "💡 建议在 README.md 中添加 GitHub Actions 状态徽章"
    fi
else
    echo "💡 建议创建 README.md 文件"
fi

if [ -f "CHANGELOG.md" ]; then
    echo "✅ 存在 CHANGELOG.md"
else
    echo "💡 建议创建 CHANGELOG.md 追踪版本变更"
fi

if [ -f "LICENSE" ] || [ -f "LICENSE.txt" ] || [ -f "LICENSE.md" ]; then
    echo "✅ 存在 LICENSE 文件"
else
    echo "💡 建议添加 LICENSE 文件"
fi

# 本地测试建议
echo ""
echo "🧪 本地测试建议..."
echo "在推送代码前，请在本地运行以下命令："
echo "   1. 构建测试："
echo "      cmake --preset=mingw"
echo "      cmake --build build-mingw --config Release"
echo "   2. 运行测试："
echo "      cd build-mingw && ctest --output-on-failure"
echo "   3. 代码格式检查 (如果安装了 clang-format)："
echo "      find src test -name '*.cpp' -o -name '*.h' | xargs clang-format -n"

# 发布流程说明
echo ""
echo "🚀 发布新版本流程..."
echo "   1. 更新版本号和 CHANGELOG"
echo "   2. 提交并推送更改到 main 分支"
echo "   3. 创建版本标签："
echo "      git tag v1.0.0"
echo "      git push origin v1.0.0"
echo "   4. GitHub Actions 将自动创建发布包"

echo ""
echo "🎉 配置检查完成!"
echo "如有问题，请参考 .github/README.md 获取详细说明。"
