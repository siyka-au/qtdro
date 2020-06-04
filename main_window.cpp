#include "main_window.h"
#include "ui_main_window.h"
#include <stdlib.h>

MainWindow::MainWindow(ConnectionHandler *connectionHandler, DialIndicatorHandler *deviceHandler, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , connectionHandler(connectionHandler)
    , deviceHandler(deviceHandler)
{
    ui->setupUi(this);

    connect(ui->actionConnect, &QAction::triggered, this, [this]() {
        auto address = this->ui->lineEditAddress->text();
        this->deviceHandler->setDevice(address);
    });

    connect(deviceHandler, &DialIndicatorHandler::infoChanged, this, [this]() {
        this->ui->statusbar->showMessage(this->deviceHandler->info());
    });

    connect(deviceHandler, &DialIndicatorHandler::errorChanged, this, [this]() {
        this->ui->statusbar->showMessage(this->deviceHandler->error());
    });

    connect(deviceHandler, &DialIndicatorHandler::aliveChanged, this, [this]() {
        this->deviceHandler->startMeasurement();
    });

    connect(deviceHandler, &DialIndicatorHandler::newMeasurementReceived, this, [this](float position) {
        ui->label->setText(QString::asprintf("%s%8.3f", position < 0 ? "-" : " ", abs(position)));
//        ui->label->setText(QString::asprintf("%9.3f", position));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
