#include "camloader.h"
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

    CamLoader loader;
    ConnectWindow cW;
    MainWindow mW;

    // Connect signals of camera connect
    QObject::connect(&cW, &ConnectWindow::signal_cameraConnected, &mW, &MainWindow::debugConnect);
    QObject::connect(&cW,
                     &ConnectWindow::signal_Connect,
                     &loader,
                     &CamLoader::slot_StartConnect,
                     Qt::DirectConnection);
    QObject::connect(&cW,
                     &ConnectWindow::signal_Refresh,
                     &loader,
                     &CamLoader::slot_StartScan,
                     Qt::DirectConnection);

    // Connect signals of camera loader
    QObject::connect(&loader,
                     &CamLoader::signal_endOfCamScan,
                     &cW,
                     &ConnectWindow::slot_cameraFound,
                     Qt::QueuedConnection);
    QObject::connect(&loader,
                     &CamLoader::signal_endOfCamConnect,
                     &cW,
                     &ConnectWindow::slot_cameraConnect,
                     Qt::QueuedConnection);
    QObject::connect(&loader,
                     &CamLoader::signal_failledCamConnect,
                     &cW,
                     &ConnectWindow::slot_failledCamConnect,
                     Qt::QueuedConnection);

    cW.refresh();
    cW.show();
    return a.exec();
}
