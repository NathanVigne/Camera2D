#include "mainwindow.h"
#include <QDebug>
#include <QString>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon/cam_ico.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::debugConnect(void *cam_handle)
{
    qDebug() << cam_handle;
    this->show();
}
