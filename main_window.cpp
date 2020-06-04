#include "main_window.h"
#include "ui_main_window.h"
#include <stdlib.h>
#include <QDoubleValidator>

MainWindow::MainWindow(ConnectionHandler *connectionHandler, DialIndicatorHandler *deviceHandler, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , connectionHandler(connectionHandler)
    , deviceHandler(deviceHandler)
{
    ui->setupUi(this);
    ui->lineEditASetPosition->setValidator(new QDoubleValidator(-2000, 2000, 3, this));

    connect(ui->actionConnect, &QAction::triggered, this, [this]() {
        auto address = this->ui->lineEditAddress->text();
        this->deviceHandler->setDevice(address);
    });

    connect(ui->actionZero, &QAction::triggered, this, [this]() {
        this->deviceHandler->setPosition(0);
    });

    connect(ui->actionSet, &QAction::triggered, this, [this]() {
        auto setPos = this->ui->lineEditASetPosition->text().toDouble();
        this->deviceHandler->setPosition(setPos);
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

    connect(deviceHandler, &DialIndicatorHandler::positionChanged, this, [this]() {
        double position = this->deviceHandler->position();
        ui->label->setText(QString::asprintf("%s%8.3f", position < 0 ? "-" : " ", abs(position)));
//        ui->label->setText(QString::asprintf("%9.3f", position));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
