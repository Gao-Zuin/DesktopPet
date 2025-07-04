# Git 分支管理指南

## 🌿 分支管理策略

### 分支类型和用途

#### 主要分支
- **`main`** - 主分支，存放稳定的生产代码
- **`develop`** - 开发分支（可选），集成最新功能

#### 辅助分支
- **`feature/*`** - 功能分支，开发新功能
- **`bugfix/*`** - 修复分支，修复已知问题
- **`hotfix/*`** - 热修复分支，紧急修复生产问题
- **`release/*`** - 发布分支，准备版本发布

### 分支命名规范

```text
feature/user-authentication
feature/pet-feeding-system
feature/ui-improvements

bugfix/memory-leak-fix
bugfix/crash-on-startup
bugfix/animation-glitch

hotfix/critical-security-fix
hotfix/data-corruption-fix

release/v1.0.0
release/v1.1.0-beta
```

## 🔄 工作流程

### 1. 功能开发流程

```bash
# 1. 从 main 分支创建功能分支
git checkout main
git pull origin main
git checkout -b feature/awesome-new-feature

# 2. 开发功能
# ... 编写代码 ...

# 3. 定期提交
git add .
git commit -m "feat: add user authentication"

# 4. 推送分支
git push origin feature/awesome-new-feature

# 5. 创建 Pull Request
# 在 GitHub 上创建 PR

# 6. 合并后清理
git checkout main
git pull origin main
git branch -d feature/awesome-new-feature
```

### 2. 问题修复流程

```bash
# 1. 从 main 分支创建修复分支
git checkout main
git pull origin main
git checkout -b bugfix/fix-memory-leak

# 2. 修复问题
# ... 修复代码 ...

# 3. 提交修复
git add .
git commit -m "fix: resolve memory leak in pet animation"

# 4. 推送和创建 PR
git push origin bugfix/fix-memory-leak
```

### 3. 紧急修复流程

```bash
# 1. 从 main 分支创建热修复分支
git checkout main
git pull origin main
git checkout -b hotfix/critical-crash-fix

# 2. 快速修复
# ... 修复关键问题 ...

# 3. 提交和推送
git add .
git commit -m "hotfix: fix critical crash on startup"
git push origin hotfix/critical-crash-fix

# 4. 创建紧急 PR（可能需要跳过某些检查）
```

### 4. 发布准备流程

```bash
# 1. 从 main 分支创建发布分支
git checkout main
git pull origin main
git checkout -b release/v1.0.0

# 2. 更新版本信息
# 更新 CMakeLists.txt 中的版本号
# 更新 README.md
# 更新 CHANGELOG.md

# 3. 提交版本更新
git add .
git commit -m "release: prepare v1.0.0"

# 4. 推送发布分支
git push origin release/v1.0.0

# 5. 创建 PR 合并到 main
# 6. 合并后创建标签
git checkout main
git pull origin main
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

## 📋 分支保护规则

### main 分支保护
- ✅ 要求 PR 审查
- ✅ 要求状态检查通过
- ✅ 要求分支是最新的
- ✅ 限制推送权限
- ✅ 不允许强制推送

### GitHub 设置步骤
1. 进入仓库设置
2. 点击 "Branches"
3. 点击 "Add rule"
4. 输入分支名称模式：`main`
5. 勾选保护选项：
   - [x] Require pull request reviews before merging
   - [x] Require status checks to pass before merging
   - [x] Require branches to be up to date before merging
   - [x] Restrict pushes that create files larger than 100 MB
   - [x] Restrict force pushes
   - [x] Restrict deletions

## 🛠️ 常用 Git 命令

### 分支操作
```bash
# 查看所有分支
git branch -a

# 创建并切换到新分支
git checkout -b feature/new-feature

# 切换分支
git checkout main

# 删除本地分支
git branch -d feature/old-feature

# 删除远程分支
git push origin --delete feature/old-feature

# 重命名分支
git branch -m old-name new-name
```

### 同步操作
```bash
# 获取远程更新
git fetch origin

# 拉取并合并
git pull origin main

# 推送分支
git push origin feature/my-feature

# 推送并设置上游
git push -u origin feature/my-feature
```

### 合并操作
```bash
# 合并分支
git merge feature/my-feature

# 变基合并（保持线性历史）
git rebase main

# 交互式变基（整理提交历史）
git rebase -i HEAD~3
```

### 撤销操作
```bash
# 撤销工作目录更改
git checkout -- file.txt

# 撤销暂存区更改
git reset HEAD file.txt

# 撤销最近的提交
git reset HEAD~1

# 撤销远程推送（危险操作）
git push --force-with-lease
```

## 🔍 分支状态检查

### 检查脚本
```bash
# 检查当前分支状态
git status

# 查看分支图
git log --oneline --graph --all

# 检查未合并的分支
git branch --no-merged

# 检查已合并的分支
git branch --merged

# 检查远程分支状态
git remote show origin
```

### 自动化检查
```bash
#!/bin/bash
# check-branch-health.sh

echo "=== 分支健康检查 ==="

# 检查当前分支
current_branch=$(git branch --show-current)
echo "当前分支: $current_branch"

# 检查是否有未提交的更改
if ! git diff --quiet; then
    echo "⚠️  工作目录有未提交的更改"
fi

# 检查是否有未推送的提交
if ! git diff --quiet HEAD origin/$current_branch; then
    echo "⚠️  有未推送的提交"
fi

# 检查是否需要合并主分支
git fetch origin
if ! git merge-base --is-ancestor origin/main HEAD; then
    echo "⚠️  建议合并最新的 main 分支"
fi

echo "✅ 分支健康检查完成"
```

## 🎯 最佳实践

### 分支生命周期
1. **短期分支**：功能分支应该尽快合并
2. **定期清理**：删除已合并的分支
3. **同步更新**：定期从 main 分支拉取更新

### 提交规范
```bash
# 功能提交
git commit -m "feat: add user login functionality"

# 修复提交
git commit -m "fix: resolve memory leak in animation"

# 文档提交
git commit -m "docs: update installation guide"

# 样式提交
git commit -m "style: format code with clang-format"

# 重构提交
git commit -m "refactor: extract pet behavior logic"

# 测试提交
git commit -m "test: add unit tests for pet model"
```

### 分支合并策略
1. **Squash Merge**：整理提交历史，适合功能分支
2. **Merge Commit**：保留分支历史，适合发布分支
3. **Rebase Merge**：线性历史，适合简单修复

## 🆘 常见问题解决

### 问题1：分支冲突
```bash
# 解决合并冲突
git merge main
# 手动解决冲突
git add .
git commit -m "resolve merge conflict"
```

### 问题2：误删分支
```bash
# 查找已删除分支的提交
git reflog
# 恢复分支
git checkout -b recovered-branch [commit-hash]
```

### 问题3：错误的分支操作
```bash
# 撤销合并
git reset --hard HEAD~1

# 撤销变基
git reset --hard ORIG_HEAD
```

### 问题4：分支同步问题
```bash
# 强制同步远程分支
git fetch origin
git reset --hard origin/main
```

## 📚 进阶技巧

### 使用 Git Flow
```bash
# 安装 git-flow
# 初始化 git flow
git flow init

# 创建功能分支
git flow feature start my-feature

# 完成功能分支
git flow feature finish my-feature

# 创建发布分支
git flow release start v1.0.0

# 完成发布分支
git flow release finish v1.0.0
```

### 使用 Git Hooks
```bash
# 创建 pre-commit 钩子
cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
# 运行代码格式检查
clang-format --dry-run --Werror src/**/*.cpp
EOF

chmod +x .git/hooks/pre-commit
```

---

*记住：良好的分支管理是团队协作的基础。遵循这些约定可以让项目更加整洁和可维护。*
