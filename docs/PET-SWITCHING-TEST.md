# 宠物切换数据独立性测试指南

## 🔍 问题分析

### 📋 原因
**所有宠物的名称都显示为 "Pet"** 的原因是：
1. 在 `PetInfo.h` 中，默认名称设置为 `"Pet"`
2. 初始化时没有为不同宠物类型设置不同的名称

### ✅ 已修复
现在每个宠物类型都有独特的名称：
- **Spider (蜘蛛)**: "小蜘蛛"
- **Cassidy (卡西迪)**: "卡西迪"

## 🧪 测试步骤

### 1. 验证不同宠物的独立数据
1. **启动程序** - 默认是蜘蛛宠物
2. **打开数值面板** - 记录初始数据
3. **增加经验值/金钱** - 让蜘蛛升级或获得金钱
4. **切换到卡西迪** - 右键菜单选择切换宠物
5. **再次打开数值面板** - 验证数据是否独立

### 2. 预期结果
- 蜘蛛的数据：名称 "小蜘蛛"，保持修改后的等级/经验/金钱
- 卡西迪的数据：名称 "卡西迪"，从初始状态开始（等级1，经验0，金钱0）

### 3. 验证数据保存
1. **修改卡西迪的数据**
2. **切换回蜘蛛** - 验证蜘蛛数据未受影响
3. **再切换回卡西迪** - 验证卡西迪的修改被保存

## 🎯 技术实现细节

### 数据存储机制
```cpp
// PetModel 中每个宠物类型独立存储
QMap<PetType, PetInfo> m_pet_data;

// 切换宠物时的处理
void PetModel::change_pet_type(PetType type) {
    // 1. 保存当前宠物数据
    save_current_pet_data();
    
    // 2. 加载目标宠物数据
    load_pet_data_for_type(type);
    
    // 3. 触发UI更新
    m_trigger.fire(PROP_ID_PET_TYPE);
    m_trigger.fire(PROP_ID_PET_LEVEL);
    // ... 其他属性通知
}
```

### 名称设置
```cpp
// 现在不同宠物有不同的默认名称
case PetType::Spider:
    info.name = "小蜘蛛";
    break;
case PetType::Cassidy:
    info.name = "卡西迪";
    break;
```

## 🔧 如果仍有问题

### 可能的调试步骤
1. **检查控制台输出** - 查看是否有切换相关的调试信息
2. **验证触发通知** - 确认 `PROP_ID_PET_TYPE` 等通知被正确触发
3. **检查UI更新** - 确认 PetStatsPanel 接收到更新通知

### 添加更多调试信息
可以在切换宠物时添加日志：
```cpp
void PetModel::change_pet_type(PetType type) noexcept
{
    qDebug() << "切换宠物类型从" << (int)m_current_info.petType << "到" << (int)type;
    qDebug() << "当前宠物名称:" << m_current_info.name;
    
    if (m_current_info.petType != type) {
        save_current_pet_data();
        load_pet_data_for_type(type);
        
        qDebug() << "切换后宠物名称:" << m_current_info.name;
        qDebug() << "切换后等级:" << m_current_info.level;
        // ... 其他调试信息
    }
}
```

## 📊 数据独立性验证表

| 操作 | 蜘蛛数据 | 卡西迪数据 | 期望结果 |
|------|----------|------------|----------|
| 初始状态 | 名称: 小蜘蛛, 等级: 1 | 名称: 卡西迪, 等级: 1 | ✅ 不同名称 |
| 蜘蛛升级 | 等级: 2 | 等级: 1 | ✅ 独立数据 |
| 切换到卡西迪 | 保持等级: 2 | 显示等级: 1 | ✅ 数据保存 |
| 卡西迪升级 | 保持等级: 2 | 等级: 2 | ✅ 独立修改 |

现在宠物切换应该能正确显示不同的名称和独立的数据了！
