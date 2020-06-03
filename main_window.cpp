#include "main_window.h"
#include "ui_main_window.h"
#include "device_info.h"

MainWindow::MainWindow(ConnectionHandler *connectionHandler, DialIndicatorHandler *deviceHandler, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , connectionHandler(connectionHandler)
    , deviceHandler(deviceHandler)
    , devicesModel()
{
    ui->setupUi(this);
//    this->setCentralWidget(ui->lcdNumberPosition);

    qInfo() << devicesModel.columnCount();
    connect(ui->pushButtonConnect, &QAction::triggered, this->deviceHandler)    connect(ui->actionIncrement, &QAction::triggered, this, &MainWindow::increment);
    connect(ui->actionDecrement, &QAction::triggered, this, &MainWindow::decrement);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::increment()
{
    this->positionCounts++;
    this->updatePosition();
}

void MainWindow::decrement()
{
    this->positionCounts--;
    this->updatePosition();
}

void MainWindow::updatePosition()
{
    this->ui->lcdNumber->display(this->positionCounts);
}

void MainWindow::updateDevices()
{

}
