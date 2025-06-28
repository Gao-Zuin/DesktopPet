#include "PetViewModel.h"

PetViewModel::PetViewModel(QObject *parent)
    : QObject(parent), model(new PetModel())
{
}

QPoint PetViewModel::getPetPosition() const
{
    return model->getPosition();
}

void PetViewModel::updatePetPosition(const QPoint &position)
{
    model->setPosition(position);
    emit petPositionChanged(position);
}

QString PetViewModel::getCurrentImagePath() const
{
    return model->getCurrentImagePath();
}

void PetViewModel::changePetType(PetType type)
{
    model->setPetType(type);
    emit petImageChanged(model->getCurrentImagePath());
}

PetType PetViewModel::getCurrentPetType() const
{
    return model->getPetType();
}

void PetViewModel::enterMovingMode()
{
    model->setMovingMode(true);
    emit movingModeChanged(true);
}

void PetViewModel::exitMovingMode()
{
    model->setMovingMode(false);
    emit movingModeChanged(false);
}

bool PetViewModel::isInMovingMode() const
{
    return model->isMovingMode();
}