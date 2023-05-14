#include "cameramanager.h"

/*!
    \fn CameraManager::CameraManager(QObject *parent) : QObject(parent)
    
    Constructor for the CameraManager, inherit from QObject.
    Connect relevent signals to private slot !
    Set-up new thread for long computation
*/
CameraManager::CameraManager(QObject *parent)
    : QObject(parent)
{
    connect(this, SIGNAL(signal_StartScan()), this, SLOT(DiscoverCameras()));
    connect(this, SIGNAL(signal_StartConnect(int)), this, SLOT(CameraConnect(int)));
    moveToThread(&thread);
    std::clog << "CameraManager :: Contructor. Thread : " << QThread::currentThreadId()
              << std::endl;
}

/*!
    \fn CameraManager::~CameraManager()
    
    Destructor for the CameraManager.
    Closse camera ressouces
*/
CameraManager::~CameraManager()
{
    if (isCamera)
        delete camera;
}

/*!
    \fn void CameraManager::slot_StartScan()
    
    Public slot to launch camera scanning operation.
    Start new thread then emit signal starting work 
*/
void CameraManager::slot_StartScan()
{
    thread.start();
    emit signal_StartScan();
    std::clog << "CameraManager :: StartScan. Thread : " << QThread::currentThreadId() << std::endl;
}

/*!
    \fn void CameraManager::slot_StartConnect(int id)
    
    Public slot to launch camera connect operation.
    Start new thread then emit signal starting work 
*/
void CameraManager::slot_StartConnect(int id)
{
    thread.start();
    std::clog << "CameraManager :: StartConnect. Thread : " << QThread::currentThreadId()
              << std::endl;
    emit signal_StartConnect(id);
}

/*!
    \fn void CameraManager::DiscoverCameras()
    
    Private slot to launch inside a new thread.
    Scan for camera using the ICamera interface
    then emits the CameraList to ConnecWindow.
    After it quit the thread
*/
void CameraManager::DiscoverCameras()
{
    std::clog << "CameraManager :: DiscoverCamera. Thread : " << QThread::currentThreadId()
              << std::endl;
    CamNamesIDs names_ids;
    // Loop over all suported camera types
    for (int type = THORLABS; type != CAMTYPE_LAST; type++) {
        switch (type) {
        case THORLABS:
            camera = new ThorCam();
            isCamera = true;
            names_ids = camera->SearchCam();
            for (int ind; ind < names_ids.names.size(); ind++) {
                camera_list.name.push_back(names_ids.names[ind]);
                camera_list.id.push_back(names_ids.ids[ind]);
                camera_list.type.push_back(THORLABS);
            }
            delete camera;
            isCamera = false;
            break;
        case MIGHTEX:
            // TO DO mightex implementation
            break;
        case CAMTYPE_LAST:

            break;
        }
    }
    emit signal_EndOfCamScan(&camera_list);
    QThread::currentThread()->quit();
}

/*!
    \fn void CameraManager::CameraConnect(int id)
    
    Private slot to launch inside a new thread.
    Connect to the selectred camera using the interface
    ICamera then emits either the ICamera pointer if
    succesfull or a nothing if not. 
    After it quit the thread
*/
void CameraManager::CameraConnect(int id)
{
    int result = -1;
    if (id < camera_list.id.size()) {
        switch (camera_list.type[id]) {
        case THORLABS:
            camera = new ThorCam();
            isCamera = true;
            result = camera->Connect(camera_list.id[id]);
            break;
        case MIGHTEX:
            // TO DO mightex implementation
            break;
        case CAMTYPE_LAST:
            break;
        }
        if (result < 0) {
            std::cerr << "CameraManager :: CameraConect. Erreur connecting camera" << std::endl;
            isCamera = false;
            emit signal_FailledCamConnect();
            return;
        }
        emit signal_EndOfCamConnect(camera);
    }

    std::cerr << "CameraManager :: CameraConect. Out of bound Id for connecting Camera"
              << std::endl;
    emit signal_FailledCamConnect();
}
