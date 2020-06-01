#include "mainwindow.h"

#include <QApplication>

#include "connectionhandler.h"
#include "devicefinder.h"
#include "devicehandler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConnectionHandler connectionHandler;
    DeviceHandler deviceHandler;
    DeviceFinder deviceFinder(&deviceHandler);

    MainWindow w(&connectionHandler, &deviceFinder, &deviceHandler);
    w.show();
    return a.exec();
}
