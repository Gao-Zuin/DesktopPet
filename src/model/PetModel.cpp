#include "PetModel.h"
#include <QApplication>
#include <QScreen>

PetModel::PetModel()
    : petType(PetType::Spider), movingMode(false)
{
    // 初始位置设置为右下角
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    position = QPoint(screenGeometry.width() - 150, screenGeometry.height() - 150);
}

QString PetModel::getCurrentImagePath() const
{
    switch (petType)
    {
    case PetType::Spider:
        return ":/resources/gif/spider.gif";
    case PetType::Cassidy:
        return ":/resources/img/cassidy.png";
    default:
        return ":/resources/gif/spider.gif";
    }
}