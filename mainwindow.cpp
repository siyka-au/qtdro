#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deviceinfo.h"

MainWindow::MainWindow(ConnectionHandler *connectionHandler, DeviceFinder *deviceFinder, DeviceHandler *deviceHandler, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , connectionHandler(connectionHandler)
    , deviceFinder(deviceFinder)
    , deviceHandler(deviceHandler)
    , devices()
{
    ui->setupUi(this);
//    this->setCentralWidget(ui->lcdNumberPosition);

    connect(ui->actionScan, &QAction::triggered, this->deviceFinder, &DeviceFinder::startSearch);
    connect(deviceFinder, &DeviceFinder::devicesChanged, this, &MainWindow::updateDevices);
    connect(ui->actionIncrement, &QAction::triggered, this, &MainWindow::increment);
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
    QList<QObject*> devices;
    if (deviceFinder->devices().canConvert<QList<QObject*>>()) {
        devices = deviceFinder->devices().value<QList<QObject*>>();
        ui->comboBoxDevices->clear();
        for (int i = 0; i < devices.count(); i++) {
            DeviceInfo* device = (DeviceInfo*) devices.at(i);
            qInfo() << device->getName();
            ui->comboBoxDevices->addItem(device->getName());
        }
    }

}
