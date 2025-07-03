#ifndef __PET_INFO_H__
#define __PET_INFO_H__

#include <QPoint>
#include <QString>
#include <QSize>
#include "../../common/Types.h"

enum class PetState {
    Idle,
    Walking,
    Running,
    Sitting,
    Sleeping
};

struct PetInfo
{
    QPoint position{100, 100};
    PetState state{PetState::Idle};
    QString currentAnimation;
    bool isVisible{true};
    QSize size{200, 200};
    int speed{50};
    QString name{"Pet"};
    PetType petType{PetType::Spider};
    
    // 经验值和等级系统
    int level{1};                    // 当前等级
    int experience{0};               // 当前经验值
    int experienceToNextLevel{100};  // 升级所需经验值
    int money{0};                    // 金钱
};

#endif
