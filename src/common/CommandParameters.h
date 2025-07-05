#ifndef __COMMAND_PARAMETERS_H__
#define __COMMAND_PARAMETERS_H__

#include "CommandBase.h"
#include "Types.h"
#include <QPoint>
#include <QString>

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

// 开始打工命令参数
class StartWorkCommandParameter : public ICommandParameter
{
public:
    StartWorkCommandParameter(int workType) : workType(workType) {}
    int workType; // 打工类型：0-光合作用
};

// 停止打工命令参数
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
        Start,          // 开始自动移动
        Stop,           // 停止自动移动
        SetMode         // 设置移动模式
    };
    
    AutoMovementCommandParameter(Action act) : action(act) {}
    
    AutoMovementCommandParameter(AutoMovementMode mode) 
        : action(Action::SetMode), movementMode(mode) {}
    
    Action action;
    AutoMovementMode movementMode = AutoMovementMode::Disabled;
};

#endif
