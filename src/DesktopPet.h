#pragma once
#include "ui_DesktopPet.h"
#include <QMainWindow>

class DesktopPet : public QMainWindow {
    Q_OBJECT
    
public:
    DesktopPet(QWidget* parent = nullptr);
    ~DesktopPet();

private:
    Ui_DesktopPet* ui;
};