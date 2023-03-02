#include "EudoxeInjector.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    FreeConsole();
    AllocConsole();

    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "w", stdin);
    freopen("CONOUT$", "w", stderr);

    QApplication a(argc, argv);
    EudoxeInjector w;
    w.show();
    return a.exec();
}
