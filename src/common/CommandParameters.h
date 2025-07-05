#ifndef __COMMAND_PARAMETERS_H__
#define __COMMAND_PARAMETERS_H__

#include "CommandBase.h"
#include "Types.h"
#include <QPoint>

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

#endif
