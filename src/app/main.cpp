#include "PetApp.h"
#include <QApplication>
#include <QDebug>
#include <QImageReader>
#include <io.h>
#include <fcntl.h>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

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