#include "mainwindow.h"
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

void MainWindow::debugConnect(ICamera *camera)
{
    std::clog << "ICamera pointer : " << camera << std::endl;
    this->show();
}
