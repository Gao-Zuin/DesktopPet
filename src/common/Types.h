#pragma once

enum class PetType
{
    Spider, // 蜘蛛
    Cassidy // 卡西迪
};

// 打工类型枚举
enum class WorkType
{
    Photosynthesis = 0, // 光合作用
    Mining = 1,         // 挖矿
    Adventure = 2       // 冒险
};

// 打工状态枚举
enum class WorkStatus
{
    Idle,   // 空闲
    Working // 工作中
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
enum class AutoMovementMode
{
    Disabled,      // 禁用自动移动
    RandomMovement // 随机移动
};