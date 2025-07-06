#pragma once

enum class PetType
{
    Spider, // 蜘蛛
    Cassidy // 卡西迪
};

// 打工类型枚举
enum class WorkType
{
    Photosynthesis = 0  // 光合作用
};

// 打工状态枚举
enum class WorkStatus
{
    Idle,       // 空闲
    Working     // 工作中
};

enum class EdgePosition
{
    None,
    Left,
    Right,
    Top,
    Bottom
};

// 自动移动模式枚举
enum class AutoMovementMode {
    Disabled,           // 禁用自动移动
    RandomMovement     // 随机移动
};

// 物品品质等级
enum class ItemQuality {
    Common = 1,     // 普通 (白色)
    Uncommon = 2,   // 不常见 (绿色)
    Rare = 3,       // 稀有 (蓝色)
    Epic = 4,       // 史诗 (紫色)
    Legendary = 5   // 传说 (橙色)
};

// 锻造配方类型
enum class ForgeRecipeType {
    ItemUpgrade,    // 物品升级
    WorkUpgrade,    // 工作系统升级
    Special         // 特殊合成
};

// 工作系统等级
enum class WorkSystemLevel {
    Basic = 1,      // 基础级
    Advanced = 2,   // 进阶级
    Expert = 3,     // 专家级
    Master = 4      // 大师级
};