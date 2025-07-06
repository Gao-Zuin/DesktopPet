#ifndef __COMMAND_PARAMETERS_H__
#define __COMMAND_PARAMETERS_H__

#include "CommandBase.h"
#include "Types.h"
#include "ForgeTypes.h"
#include <QPoint>
#include <QString>
#include <QVector>

// 移动命令参数
class MoveCommandParameter : public ICommandParameter
{
public:
    MoveCommandParameter(const QPoint &pos) : position(pos) {}
    QPoint position;
};

// 切换宠物命令参数
class SwitchPetCommandParameter : public ICommandParameter
{
public:
    SwitchPetCommandParameter(PetType type) : petType(type) {}
    PetType petType;
};

// 显示数值面板命令参数
class ShowStatsPanelCommandParameter : public ICommandParameter
{
public:
    ShowStatsPanelCommandParameter() {}
};

class ShowBackpackPanelCommandParameter : public ICommandParameter
{
public:
    ShowBackpackPanelCommandParameter() {}
};

class ShowCollectionPanelCommandParameter : public ICommandParameter
{
public:
    ShowCollectionPanelCommandParameter() {}
};

class UseItemCommandParameter : public ICommandParameter
{
public:
    UseItemCommandParameter(int id, int count) : id(id), count(count) {}
    int id;
    int count;
};

// 添加经验值命令参数
class AddExperienceCommandParameter : public ICommandParameter
{
public:
    AddExperienceCommandParameter(int exp) : experience(exp) {}
    int experience;
};

// 添加金钱命令参数
class AddMoneyCommandParameter : public ICommandParameter
{
public:
    AddMoneyCommandParameter(int amount) : money(amount) {}
    int money;
};

// 显示打工面板命令参数
class ShowWorkPanelCommandParameter : public ICommandParameter
{
public:
    ShowWorkPanelCommandParameter() {}
};

// 开始工作命令参数
class StartWorkCommandParameter : public ICommandParameter
{
public:
    StartWorkCommandParameter(int workTypeId) : workTypeId(workTypeId) {}
    int workTypeId;
};

// 停止工作命令参数
class StopWorkCommandParameter : public ICommandParameter
{
public:
    StopWorkCommandParameter() {}
};

// 自动移动命令参数
class AutoMovementCommandParameter : public ICommandParameter
{
public:
    enum class Action {
        Start,      // 开始自动移动
        Stop,       // 停止自动移动
        SetMode     // 设置移动模式
    };
    
    AutoMovementCommandParameter(Action act) : action(act) {}
    
    AutoMovementCommandParameter(AutoMovementMode mode) 
        : action(Action::SetMode), movementMode(mode) {}
    
    Action action;
    AutoMovementMode movementMode = AutoMovementMode::Disabled;
};

// 显示锻造面板命令参数
class ShowForgePanelCommandParameter : public ICommandParameter
{
public:
    ShowForgePanelCommandParameter() {}
};

// 锻造命令参数
class ForgeCommandParameter : public ICommandParameter
{
public:
    enum class Action {
        ForgeItem,              // 锻造物品
        UpgradeWorkSystem,      // 升级工作系统
        GetRecipes,             // 获取配方列表
        GetMaterials,           // 获取材料需求
        ForgeItemWithCustomMaterials // 使用自定义材料锻造物品
    };
    
    ForgeCommandParameter(Action act) : action(act) {}
    
    ForgeCommandParameter(int recipeId) 
        : action(Action::ForgeItem), forgeRecipeId(recipeId) {}
    
    ForgeCommandParameter(WorkType workType, WorkSystemLevel targetLevel)
        : action(Action::UpgradeWorkSystem), workType(workType), targetLevel(targetLevel) {}
    
    // 用于自定义材料锻造的构造函数
    ForgeCommandParameter(int recipeId, const QVector<ForgeMaterial>& customMaterials)
        : action(Action::ForgeItemWithCustomMaterials), forgeRecipeId(recipeId), customMaterials(customMaterials) {}
    
    Action action;
    int forgeRecipeId = 0;
    WorkType workType = WorkType::Photosynthesis;
    WorkSystemLevel targetLevel = WorkSystemLevel::Basic;
    QVector<ForgeMaterial> customMaterials; // 自定义消耗材料
};

#endif
