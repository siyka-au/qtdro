#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    this->setCentralWidget(ui->lcdNumberPosition);

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
