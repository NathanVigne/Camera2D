#include "camloader.h"
#include <QDebug>

/*!
    \fn CamLoader::CamLoader(QObject *parent) : QObject(parent)
    
    Constructor for the CamLoader, inherit from QObject.
    Connect relevent signals to private slot !
    Set-up new thread for long computation
*/
CamLoader::CamLoader(QObject *parent)
    : QObject(parent)
{
    connect(this, SIGNAL(signal_startScan()), this, SLOT(slot_scanForCamera()));
    connect(this, SIGNAL(signal_startConnect(int)), this, SLOT(slot_connectToCamera(int)));
    moveToThread(&thread);
    qDebug() << "Cam Loader construct" << QThread::currentThreadId();
}

/*!
    \fn CamLoader::~CamLoader()
    
    Destructor for the CamLoader.
    Closse camera ressouces
*/
CamLoader::~CamLoader()
{
    thor_closeRessources();
    // TO DO : implementation for Migthex cam
}

/*!
    \fn void CamLoader::slot_StartScan()
    
    Public slot to launch camera scanning operation.
    Start new thread then emit signal starting work 
*/
void CamLoader::slot_StartScan()
{
    thread.start();
    qDebug() << "start scan" << QThread::currentThreadId();
    emit signal_startScan();
}

/*!
    \fn void CamLoader::slot_StartConnect(int id)
    
    Public slot to launch camera connect operation.
    Start new thread then emit signal starting work 
*/
void CamLoader::slot_StartConnect(int id)
{
    thread.start();
    qDebug() << "start connect" << QThread::currentThreadId();
    emit signal_startConnect(id);
}

/*!
    \fn void CamLoader::slot_scanForCamera()
    
    Private slot to launch inside a new thread.
    Scan for camera using thorlabs.h then emits
    the camera name to ConnecWindow.
    After it quit the thread
*/
void CamLoader::slot_scanForCamera()
{
    names_ids = thor_searchCamera();
    // TO DO : implement scan for Migtex cam
    emit signal_endOfCamScan(&names_ids);
    qDebug() << "Scanning" << QThread::currentThreadId();
    QThread::currentThread()->quit();
}

/*!
    \fn void CamLoader::slot_connectToCamera(int id)
    
    Private slot to launch inside a new thread.
    Connect to the selectred camera using thorlabs.h
    then emits either the camera_handle if succesfull
    or a nothing if not. 
    After it quit the thread
*/
void CamLoader::slot_connectToCamera(int id)
{
    QString CamID = names_ids.camera_ID.at(id);

    // TO DO : implement scan for Migtex cam
    open_cam_handle = thor_connectCamera(&CamID);

    if (!open_cam_handle) {
        emit signal_failledCamConnect();
    } else {
        emit signal_endOfCamConnect(open_cam_handle);
    }
    qDebug() << "Cam Loader connecting" << QThread::currentThreadId();
    QThread::currentThread()->quit();
}
