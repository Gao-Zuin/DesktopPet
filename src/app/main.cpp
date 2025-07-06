#include "PetApp.h"
#include <QApplication>
#include <QDebug>
#include <io.h>
#include <fcntl.h>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    // 在Windows上分配控制台窗口以显示调试输出
    AllocConsole();
    
    // 重定向stdout, stdin, stderr到控制台
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
    
    // 使iostream同步到控制台
    std::ios::sync_with_stdio(true);
    
    // 设置控制台标题
    SetConsoleTitle(L"DesktopPet Debug Console");
    
    qDebug() << "Debug console initialized";
#endif
    
    QApplication app(argc, argv);
    
    PetApp petApp;
    if (!petApp.initialize()) {
        return 1;
    }
    
    petApp.show_main_window();
    
    return app.exec();
}