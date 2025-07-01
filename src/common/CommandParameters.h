#ifndef __COMMAND_PARAMETERS_H__
#define __COMMAND_PARAMETERS_H__

#include "CommandBase.h"
#include "Types.h"
#include <QPoint>

// 移动命令参数
class MoveCommandParameter : public ICommandParameter
{
public:
    MoveCommandParameter(const QPoint& pos) : position(pos) {}
    QPoint position;
};

// 切换宠物命令参数
class SwitchPetCommandParameter : public ICommandParameter
{
public:
    SwitchPetCommandParameter(PetType type) : petType(type) {}
    PetType petType;
};


#endif
