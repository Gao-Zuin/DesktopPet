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
};

#endif
