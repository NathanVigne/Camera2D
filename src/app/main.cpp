#include "connectwindow.h"
#include "mainwindow.h"

#include <thorlabs.h>

#include <QApplication>
#include <QObject>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    thor_intializeDLLs();

    MainWindow mW;
    ConnectWindow cW;
    QObject::connect(&cW, &ConnectWindow::cameraConnect, &mW, &MainWindow::debugConnect);
    QObject::connect(&cW, &ConnectWindow::cameraConnect, &mW, &MainWindow::showWindow);


    cW.show();
    return a.exec();
}
