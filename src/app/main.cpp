#include "PetApp.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PetApp petApp;
    if (!petApp.initialize())
    {
        return 1;
    }

    petApp.show_main_window();

    return app.exec();
}