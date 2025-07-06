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

// 声明外部静态变量
extern PetApp* s_appInstance;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PetApp petApp;
    
    // 设置全局实例指针
    s_appInstance = &petApp;
    
    if (!petApp.initialize())
    {
        return 1;
    }

    petApp.show_main_window();

    int result = app.exec();
    
    // 清理全局实例指针
    s_appInstance = nullptr;
    
    return result;
}