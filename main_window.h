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

private:
    Ui::MainWindow *ui;

    ConnectionHandler *connectionHandler;
    DialIndicatorHandler *deviceHandler;


    int positionCounts;
};
#endif // MAINWINDOW_H
