@echo off
REM GitHub Actions 状态检查脚本 (Windows)

echo 🔍 DesktopPet GitHub Actions 配置检查
echo =====================================

REM 检查工作流文件
echo.
echo 📋 检查工作流文件...
set workflows_dir=.github\workflows

if not exist "%workflows_dir%" (
    echo ❌ .github\workflows 目录不存在
    pause
    exit /b 1
)

set workflows[0]=ci.yml:Windows MinGW CI/CD 流水线
set workflows[1]=quick-test.yml:Windows 快速测试  
set workflows[2]=release.yml:Windows 自动发布
set workflows[3]=security.yml:安全检查
set workflows[4]=performance.yml:性能测试

for /l %%i in (0,1,4) do (
    call set workflow=%%workflows[%%i]%%
    for /f "tokens=1,2 delims=:" %%a in ("!workflow!") do (
        if exist "%workflows_dir%\%%a" (
            echo ✅ %%a - %%b
        ) else (
            echo ❌ %%a - %%b ^(缺失^)
        )
    )
)

REM 检查必要的配置文件
echo.
echo 🔧 检查配置文件...

if exist "CMakeLists.txt" (
    echo ✅ CMakeLists.txt - CMake 构建配置
) else (
    echo ❌ CMakeLists.txt - CMake 构建配置 ^(缺失^)
)

if exist "CMakePresets.json" (
    echo ✅ CMakePresets.json - CMake 预设
) else (
    echo ❌ CMakePresets.json - CMake 预设 ^(缺失^)
)

if exist ".gitignore" (
    echo ✅ .gitignore - Git 忽略规则
) else (
    echo ❌ .gitignore - Git 忽略规则 ^(缺失^)
)

REM 检查源代码结构
echo.
echo 📁 检查项目结构...

if exist "src\" (
    echo ✅ src\ - 源代码目录
) else (
    echo ❌ src\ - 源代码目录 ^(缺失^)
)

if exist "test\" (
    echo ✅ test\ - 测试代码目录
) else (
    echo ❌ test\ - 测试代码目录 ^(缺失^)
)

if exist "resources\" (
    echo ✅ resources\ - 资源文件目录
) else (
    echo ❌ resources\ - 资源文件目录 ^(缺失^)
)

REM 检查 GitHub 仓库设置建议
echo.
echo ⚙️  GitHub 仓库设置建议...
echo 📌 请确保以下设置已正确配置：
echo    1. Settings → Actions → General → Actions permissions
echo       └─ 允许所有操作和可重用工作流
echo    2. Settings → Branches → Branch protection rules
echo       └─ 为 main 分支启用保护规则
echo    3. Settings → Secrets and variables → Actions
echo       └─ 确保 GITHUB_TOKEN 权限足够

REM 检查可选功能
echo.
echo 🌟 可选功能检查...

if exist "README.md" (
    findstr /i "badge" README.md >nul 2>nul
    if !errorlevel! equ 0 (
        echo ✅ README.md 包含状态徽章
    ) else (
        echo 💡 建议在 README.md 中添加 GitHub Actions 状态徽章
    )
) else (
    echo 💡 建议创建 README.md 文件
)

if exist "CHANGELOG.md" (
    echo ✅ 存在 CHANGELOG.md
) else (
    echo 💡 建议创建 CHANGELOG.md 追踪版本变更
)

if exist "LICENSE" (
    echo ✅ 存在 LICENSE 文件
) else if exist "LICENSE.txt" (
    echo ✅ 存在 LICENSE 文件
) else if exist "LICENSE.md" (
    echo ✅ 存在 LICENSE 文件
) else (
    echo 💡 建议添加 LICENSE 文件
)

REM 本地测试建议
echo.
echo 🧪 本地测试建议...
echo 在推送代码前，请在本地运行以下命令：
echo    1. 构建测试：
echo       cmake --preset=mingw
echo       cmake --build build-mingw --config Release
echo    2. 运行测试：
echo       cd build-mingw ^&^& ctest --output-on-failure
echo    3. 代码格式检查 ^(如果安装了 clang-format^)：
echo       for /r %%f in ^(*.cpp *.h^) do clang-format -n "%%f"

REM 发布流程说明
echo.
echo 🚀 发布新版本流程...
echo    1. 更新版本号和 CHANGELOG
echo    2. 提交并推送更改到 main 分支
echo    3. 创建版本标签：
echo       git tag v1.0.0
echo       git push origin v1.0.0
echo    4. GitHub Actions 将自动创建发布包

echo.
echo 🎉 配置检查完成!
echo 如有问题，请参考 .github\README.md 获取详细说明。

pause
