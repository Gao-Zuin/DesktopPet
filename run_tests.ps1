#!/usr/bin/env powershell

# 桌面宠物项目测试运行脚本
# 用于自动化编译和运行单元测试

Write-Host "=== 桌面宠物项目测试运行器 ===" -ForegroundColor Green

# 检查是否存在build目录
if (-not (Test-Path "build")) {
    Write-Host "创建build目录..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Name "build"
}

# 进入build目录
Set-Location "build"

try {
    # 配置项目
    Write-Host "配置CMake项目..." -ForegroundColor Yellow
    cmake .. -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake配置失败"
    }

    # 编译项目
    Write-Host "编译项目..." -ForegroundColor Yellow
    cmake --build . --config Debug
    
    if ($LASTEXITCODE -ne 0) {
        throw "编译失败"
    }

    # 检查测试可执行文件是否存在
    $testExe = "DesktopPet_tests.exe"
    if (Test-Path $testExe) {
        Write-Host "运行单元测试..." -ForegroundColor Yellow
        & ".\$testExe" --gtest_output=xml:test_results.xml --gtest_color=yes
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "所有测试通过！" -ForegroundColor Green
        } else {
            Write-Host "部分测试失败，请检查输出" -ForegroundColor Red
        }
    } else {
        Write-Host "未找到测试可执行文件，可能编译时没有包含测试代码" -ForegroundColor Yellow
        Write-Host "运行主程序..." -ForegroundColor Yellow
        if (Test-Path "DesktopPet.exe") {
            Write-Host "主程序编译成功！" -ForegroundColor Green
        }
    }

    # 运行CTest（如果可用）
    Write-Host "运行CTest..." -ForegroundColor Yellow
    ctest --output-on-failure --verbose

} catch {
    Write-Host "错误: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
} finally {
    # 返回项目根目录
    Set-Location ".."
}

Write-Host "测试完成！" -ForegroundColor Green
