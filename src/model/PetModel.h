#pragma once
#include <QString>
#include <QPoint>

enum class PetType
{
    Spider, // 蜘蛛
    Cassidy // 卡西迪
};

class PetModel
{
public:
    PetModel();

    // 位置相关
    QPoint getPosition() const { return position; }
    void setPosition(const QPoint &pos) { position = pos; }

    // 桌宠类型相关
    PetType getPetType() const { return petType; }
    void setPetType(PetType type) { petType = type; }

    // 获取当前桌宠的资源路径
    QString getCurrentImagePath() const;

    // 移动模式
    bool isMovingMode() const { return movingMode; }
    void setMovingMode(bool mode) { movingMode = mode; }

private:
    QPoint position;
    PetType petType;
    bool movingMode;
};