#include "mainwindow.h"
#include <QDebug>
#include <QString>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::debugConnect(QString str, int id)
{
    qDebug() << str << id;
}

void MainWindow::showWindow()
{
    this->show();
}
