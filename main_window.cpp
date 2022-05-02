#include "main_window.h"
#include "ui_main_window.h"
//#include <stdlib.h>
#include <QDoubleValidator>

MainWindow::MainWindow(ConnectionHandler *connectionHandler, DialIndicatorHandler *deviceHandler, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , connectionHandler(connectionHandler)
    , deviceHandler(deviceHandler)
{
    ui->setupUi(this);
    ui->lineEditSetPosition->setValidator(new QDoubleValidator(-1000, 1000, 3, this));

    connect(ui->actionConnect, &QAction::triggered, this, [this]() {
        auto address = this->ui->lineEditAddress->text();
        this->deviceHandler->setDevice(address);
    });

    connect(ui->actionZero, &QAction::triggered, this, [this]() {
        this->deviceHandler->setPosition(0);
    });

    connect(ui->actionSet, &QAction::triggered, this, [this]() {
        auto setPos = this->ui->lineEditSetPosition->text().toDouble();
        this->deviceHandler->setPosition(setPos);
    });

    connect(deviceHandler, &DialIndicatorHandler::infoChanged, this, [this]() {
        this->ui->statusbar->showMessage(this->deviceHandler->info());
    });

    connect(deviceHandler, &DialIndicatorHandler::errorChanged, this, [this]() {
        this->ui->statusbar->showMessage(this->deviceHandler->error());
    });

    connect(deviceHandler, &DialIndicatorHandler::aliveChanged, this, [this]() {
        QColor red(Qt::red);
        QColor green(Qt::green);
        this->ui->label->setTextColor(this->deviceHandler->alive() ? green : red);
        this->deviceHandler->startMeasurement();
    });

    connect(deviceHandler, &DialIndicatorHandler::positionChanged, this, [this]() {
        double position = this->deviceHandler->position();
        ui->label->setText(QString::asprintf("%9.3f", position));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
