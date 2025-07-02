# 集成测试说明

本目录包含DesktopPet项目的集成测试，用于验证各个模块之间的通信和协作。

## 测试文件结构

### PetIntegrationTest.cpp
完整的端到端集成测试，测试内容包括：
- **初始化流程测试**：验证Model、ViewModel、View的完整初始化
- **用户操作流程测试**：模拟用户拖拽、切换宠物等操作
- **数据绑定测试**：验证属性指针绑定的正确性
- **命令执行测试**：验证命令从View到Model的完整执行流程
- **错误处理测试**：验证异常情况的处理
- **应用程序生命周期测试**：验证PetApp的完整生命周期

### CommunicationIntegrationTest.cpp
专门测试模块间通信机制，测试内容包括：
- **通知链测试**：Model → ViewModel → View的通知传播
- **命令链测试**：View → ViewModel → Model的命令执行
- **双向通信测试**：验证完整的双向数据流
- **并发通信测试**：验证多个操作并发时的稳定性
- **通知时序测试**：验证通知的时间顺序和性能
- **错误通信测试**：验证错误情况下的通信稳定性

### CommandSystemIntegrationTest.cpp
专门测试命令系统的集成，测试内容包括：
- **命令执行流程测试**：验证各种命令的完整执行流程
- **命令错误处理测试**：验证命令系统的错误处理能力
- **View命令集成测试**：验证View与命令系统的集成
- **并发命令测试**：验证多个命令并发执行的稳定性
- **命令参数完整性测试**：验证命令参数的正确传递

## 运行测试

### 构建测试
```bash
cmake --preset=mingw
cmake --build --preset=mingw --target DesktopPetIntegrationTests
```

### 运行集成测试
```bash
# 运行集成测试
cd build-mingw
./DesktopPetIntegrationTests.exe

```

### 运行所有测试（单元测试 + 集成测试）
```bash
# 构建所有测试
cmake --build --preset=mingw --target DesktopPetAllTests

# 运行所有测试
./build-mingw/DesktopPetAllTests.exe

```

## 测试覆盖的场景

### 1. 数据流测试
- Model数据变化 → ViewModel通知 → View更新
- View用户操作 → Command执行 → Model数据变化
- 属性指针绑定的一致性验证

### 2. 命令系统测试
- 移动命令（MoveCommand）的完整流程
- 切换宠物命令（SwitchPetCommand）的完整流程
- 命令参数的类型安全性

### 3. 通信机制测试
- PropertyTrigger的通知机制
- 回调函数的正确调用
- 多层通知的传播（Model → ViewModel → View）

### 4. 错误处理测试
- 无效命令参数的处理
- 空指针的安全处理
- 错误状态下的系统稳定性

### 5. 性能和稳定性测试
- 高频操作的稳定性
- 并发操作的安全性
- 内存管理的正确性

## 测试环境要求

- Qt 6.x 环境
- Google Test框架
- C++17支持
- 如果在无显示器环境中运行，某些UI相关测试可能会跳过

## 扩展测试

可以基于现有框架添加更多测试：
- 新的Command类型测试
- 更复杂的用户交互场景
- 性能基准测试
- 内存泄漏检测

## 注意事项

1. 集成测试需要完整的Qt环境，包括GUI组件
2. 测试中创建的QApplication实例会在整个测试期间保持
3. 某些测试可能需要事件循环处理，已经包含了适当的等待机制
4. 测试使用智能指针管理内存，确保资源正确释放
