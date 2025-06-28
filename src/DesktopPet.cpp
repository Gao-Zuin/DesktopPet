#include "DesktopPet.h"

DesktopPet::DesktopPet(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_DesktopPet)
{
    ui->setupUi(this);
}

DesktopPet::~DesktopPet()
{
    delete ui; 
}