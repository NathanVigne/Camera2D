#include "connectwindow.h"
#include "mainwindow.h"

#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mW;
    ConnectWindow cW;
    QObject::connect(&cW, &ConnectWindow::cameraConnect, &mW, &MainWindow::debugConnect);
    QObject::connect(&cW, &ConnectWindow::cameraConnect, &mW, &MainWindow::showWindow);

    cW.show();
    return a.exec();
}
