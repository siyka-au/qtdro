#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStringListModel>

#include "connection_handler.h"
#include "dial_indicator_handler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(ConnectionHandler *connectionHandler, DialIndicatorHandler *deviceHandler, QWidget *parent = nullptr);
    ~MainWindow();

    void updateDevices();

private slots:
    void increment();
    void decrement();
    void updatePosition();

private:
    Ui::MainWindow *ui;
    int positionCounts;
    ConnectionHandler *connectionHandler;
    DialIndicatorHandler *deviceHandler;
    QStandardItemModel devicesModel;
};
#endif // MAINWINDOW_H
