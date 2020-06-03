#include "main_window.h"

#include <QApplication>

#include "connection_handler.h"
#include "dial_indicator_handler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConnectionHandler connectionHandler;
    DialIndicatorHandler deviceHandler;

    MainWindow w(&connectionHandler, &deviceHandler);
    w.show();
    return a.exec();
}
