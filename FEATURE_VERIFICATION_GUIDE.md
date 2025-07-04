# 桌宠自动移动功能验证指南

## 🎯 验证目标

确保新实现的自动移动功能正常工作，包括三种移动模式和相关的UI交互。

## 📋 验证清单

### ✅ 基本功能验证

#### 1. 编译验证
- [ ] 项目能够成功编译
- [ ] 没有链接错误
- [ ] 所有新增的头文件都能正确包含

#### 2. UI菜单验证
- [ ] 右键菜单显示"自动移动"子菜单
- [ ] 子菜单包含以下选项：
  - [ ] "开启边框行走"
  - [ ] "开启随机移动"  
  - [ ] "跟随鼠标"
  - [ ] "停止自动移动"

#### 3. 边框行走模式验证
- [ ] 点击"开启边框行走"后桌宠开始移动
- [ ] 桌宠沿着屏幕边缘移动
- [ ] 到达屏幕角落时会停留2-5秒
- [ ] 从角落离开后继续沿边缘移动
- [ ] 移动过程中偶尔会暂停
- [ ] 桌宠不会移出屏幕范围

#### 4. 随机移动模式验证
- [ ] 点击"开启随机移动"后桌宠开始移动
- [ ] 桌宠在屏幕边缘区域随机移动
- [ ] 到达目标点后会选择新的随机目标
- [ ] 移动轨迹看起来自然流畅
- [ ] 桌宠不会移出屏幕范围

#### 5. 鼠标跟随模式验证
- [ ] 点击"跟随鼠标"后桌宠开始跟随
- [ ] 移动鼠标时桌宠会朝鼠标方向移动
- [ ] 鼠标距离太近时桌宠会停止移动
- [ ] 鼠标距离太远时桌宠不会过度移动
- [ ] 跟随移动看起来平滑自然

#### 6. 停止功能验证
- [ ] 点击"停止自动移动"能正确停止移动
- [ ] 手动拖拽桌宠时自动移动会停止
- [ ] 停止后桌宠保持在当前位置不动

### 🔍 高级功能验证

#### 7. 边界处理验证
- [ ] 桌宠在屏幕左边缘移动正常
- [ ] 桌宠在屏幕右边缘移动正常
- [ ] 桌宠在屏幕上边缘移动正常
- [ ] 桌宠在屏幕下边缘移动正常
- [ ] 四个角落的停留功能正常

#### 8. 性能验证
- [ ] 自动移动时CPU占用率正常（< 5%）
- [ ] 长时间运行不会内存泄漏
- [ ] 移动动画流畅，无卡顿现象
- [ ] 不会影响其他应用程序性能

#### 9. 兼容性验证
- [ ] 与现有的手动拖拽功能兼容
- [ ] 与其他面板（数值、背包等）功能兼容
- [ ] 与宠物切换功能兼容
- [ ] 程序退出时能正确清理资源

## 🛠️ 调试验证

### 日志检查
启动应用后，在控制台查看是否有以下日志输出：

```
AutoMovementModel initialized with screen rect: QRect(0,0 1920x1080)
AutoMovement config updated: mode=1 speed=2 interval=50
AutoMovement started with mode: 1 initial target: QPoint(100,1070)
Moving to next edge position: QPoint(500,10) on edge: 1
Corner reached, staying for a while
```

### 命令系统验证
验证命令管理器是否正确注册了自动移动命令：

```cpp
// 在代码中添加验证
ICommandBase* autoCmd = commandManager.get_command(CommandType::AUTO_MOVEMENT);
if (autoCmd) {
    qDebug() << "AutoMovement command registered successfully";
} else {
    qDebug() << "ERROR: AutoMovement command not found";
}
```

## 🚨 常见问题排查

### 问题1：菜单不显示
**症状**：右键菜单中没有"自动移动"选项  
**检查**：
- [ ] PetMainWindow.cpp 中是否正确添加了菜单代码
- [ ] 是否包含了必要的头文件
- [ ] setupContextMenu() 方法是否被调用

### 问题2：点击菜单无反应
**症状**：点击自动移动菜单选项后桌宠没有移动  
**检查**：
- [ ] 回调函数是否正确实现
- [ ] 命令是否正确注册到CommandManager
- [ ] AutoMovementModel是否正确初始化
- [ ] 是否有错误日志输出

### 问题3：移动异常
**症状**：桌宠移动过快/过慢/移出屏幕  
**检查**：
- [ ] AutoMovementConfig 配置是否合理
- [ ] 屏幕尺寸检测是否正确
- [ ] 边界约束函数是否工作正常

### 问题4：编译错误
**症状**：编译时出现链接错误或头文件找不到  
**检查**：
- [ ] CMakeLists.txt 是否包含新文件
- [ ] 头文件路径是否正确
- [ ] 前向声明是否正确

## 📱 测试用例

### 测试用例1：基本边框行走
1. 启动应用
2. 右键点击桌宠
3. 选择"自动移动" -> "开启边框行走"
4. 观察桌宠移动5分钟
5. 验证是否沿边缘移动且在角落停留

### 测试用例2：模式切换
1. 启动边框行走模式
2. 等待桌宠移动到屏幕一侧
3. 切换到随机移动模式
4. 观察移动行为是否立即改变

### 测试用例3：手动干预
1. 启动任意自动移动模式
2. 用鼠标拖拽桌宠到新位置
3. 验证自动移动是否停止
4. 重新启动自动移动验证功能正常

### 测试用例4：长时间运行
1. 启动自动移动模式
2. 让程序运行2小时以上
3. 观察是否有内存泄漏或性能下降
4. 验证功能是否持续稳定

## ✅ 验证完成标准

当以上所有验证项都通过时，自动移动功能验证完成。特别注意：

1. **功能正确性** - 所有三种移动模式都能正常工作
2. **UI完整性** - 菜单显示正确，操作响应及时
3. **性能稳定性** - 长时间运行不出现问题
4. **兼容性良好** - 不影响现有功能

## 📝 验证报告模板

```
# 自动移动功能验证报告

## 验证环境
- 操作系统：Windows 11
- Qt版本：6.x
- 屏幕分辨率：1920x1080
- 测试时间：2024-XX-XX

## 验证结果
- 基本功能：✅ 通过 / ❌ 失败
- UI交互：✅ 通过 / ❌ 失败  
- 性能测试：✅ 通过 / ❌ 失败
- 兼容性测试：✅ 通过 / ❌ 失败

## 发现的问题
1. [问题描述]
2. [问题描述]

## 建议改进
1. [改进建议]
2. [改进建议]
```

---

按照这个验证指南逐步检查，确保自动移动功能的质量和稳定性！
