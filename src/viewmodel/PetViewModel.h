#pragma once
#include <QObject>
#include <QPoint>
#include "../model/PetModel.h"

class PetViewModel : public QObject
{
    Q_OBJECT

public:
    explicit PetViewModel(QObject *parent = nullptr);

    // 位置管理
    QPoint getPetPosition() const;
    void updatePetPosition(const QPoint &position);

    // 桌宠类型管理
    QString getCurrentImagePath() const;
    void changePetType(PetType type);
    PetType getCurrentPetType() const;

    // 移动模式管理
    void enterMovingMode();
    void exitMovingMode();
    bool isInMovingMode() const;

signals:
    void petPositionChanged(const QPoint &position);
    void petImageChanged(const QString &imagePath);
    void movingModeChanged(bool enabled);

private:
    PetModel *model;
};