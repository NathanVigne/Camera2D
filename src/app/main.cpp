#include "cameramanager.h"
#include "connectwindow.h"
#include "mainwindow.h"

#include <QApplication>
#include <QObject>

#ifndef LM_DBL_PREC
#error Program assumes that levmar has been compiled with double precision, see LM_DBL_PREC!
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CameraManager camManager;
    ConnectWindow cW;
    MainWindow mW;

    // Connect signals of camera connect Window
    QObject::connect(&cW, &ConnectWindow::signal_cameraConnected, &mW, &MainWindow::slot_CameraOpen);
    QObject::connect(&cW,
                     &ConnectWindow::signal_Connect,
                     &camManager,
                     &CameraManager::slot_StartConnect,
                     Qt::DirectConnection);
    QObject::connect(&cW,
                     &ConnectWindow::signal_Refresh,
                     &camManager,
                     &CameraManager::slot_StartScan,
                     Qt::DirectConnection);

    // Connect signals of camera loader
    QObject::connect(&camManager,
                     &CameraManager::signal_EndOfCamScan,
                     &cW,
                     &ConnectWindow::slot_cameraFound,
                     Qt::QueuedConnection);
    QObject::connect(&camManager,
                     &CameraManager::signal_EndOfCamConnect,
                     &cW,
                     &ConnectWindow::slot_cameraConnect,
                     Qt::QueuedConnection);
    QObject::connect(&camManager,
                     &CameraManager::signal_FailledCamConnect,
                     &cW,
                     &ConnectWindow::slot_failledCamConnect,
                     Qt::QueuedConnection);

    cW.refresh();
    cW.show();
    //mW.show();
    return a.exec();
}
