#include "connectwindow.h"
#include "mainwindow.h"

#include <tl_camera_sdk.h>
#include <tl_camera_sdk_load.h>

#include <QApplication>
#include <QObject>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mW;
    ConnectWindow cW;
    QObject::connect(&cW, &ConnectWindow::cameraConnect, &mW, &MainWindow::debugConnect);
    QObject::connect(&cW, &ConnectWindow::cameraConnect, &mW, &MainWindow::showWindow);


    if (tl_camera_sdk_dll_initialize()){
        qDebug() << "Failled";
    }else{
        qDebug() << "Succes";
    }

    cW.show();
    return a.exec();
}
