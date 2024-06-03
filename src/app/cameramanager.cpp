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

    std::clog << "Destroy Cam Manager" << std::endl;
}

/*!
    \fn void CameraManager::DiscoverCameras()
    
    Public slot.
    Scan for camera using the ICamera interface
    then emits the CameraList to ConnecWindow.
    After it quit the thread
*/
void CameraManager::DiscoverCameras()
{
    std::clog << "CameraManager :: DiscoverCamera. Thread : " << QThread::currentThreadId()
              << std::endl;
    CamNamesIDs names_ids;
    camera_list.id.clear();
    camera_list.name.clear();
    camera_list.type.clear();
    // Loop over all suported camera types
    for (int type = THORLABS; type != CAMTYPE_LAST; type++) {
        switch (type) {
        case THORLABS:
            camera = new ThorCam();
            isCamera = true;
            names_ids = camera->SearchCam();
            for (int ind = 0; ind < names_ids.names.size(); ind++) {
                camera_list.name.push_back(names_ids.names[ind]);
                camera_list.id.push_back(names_ids.ids[ind]);
                camera_list.type.push_back(THORLABS);
            }
            delete camera;
            isCamera = false;
            break;
        case MIGHTEX:
            camera = new Mightex();
            isCamera = true;
            names_ids = camera->SearchCam();
            for (int ind = 0; ind < names_ids.names.size(); ind++) {
                camera_list.name.push_back(names_ids.names[ind]);
                camera_list.id.push_back(names_ids.ids[ind]);
                camera_list.type.push_back(MIGHTEX);
            }
            delete camera;
            isCamera = false;
            break;
        case ALLIEDVISION:
            // TO DO AlliedVision implementation
            camera = new AlliedVision();
            isCamera = true;
            names_ids = camera->SearchCam();
            for (int ind = 0; ind < names_ids.names.size(); ind++) {
                camera_list.name.push_back(names_ids.names[ind]);
                camera_list.id.push_back(names_ids.ids[ind]);
                camera_list.type.push_back(ALLIEDVISION);
            }
            delete camera;
            isCamera = false;
            break;
        case CAMTYPE_LAST:

            break;
        }
    }
    emit signal_EndOfCamScan(&camera_list);
}

/*!
    \fn void CameraManager::CameraConnect(int id)
    
    Public slot.
    Connect to the selectred camera using the interface
    ICamera then emits either the ICamera pointer if
    succesfull or a nothing if not. 
    After it quit the thread
*/
void CameraManager::CameraConnect(int id)
{
    int result = -1;
    CAMERATYPE type;
    if (id < camera_list.id.size()) {
        switch (camera_list.type[id]) {
        case THORLABS:
            camera = new ThorCam();
            isCamera = true;
            result = camera->Connect(camera_list.id[id]);
            type = CAMERATYPE::THORLABS;
            break;
        case MIGHTEX:
            // TO DO mightex implementation
            camera = new Mightex();
            isCamera = true;
            result = camera->Connect(camera_list.id[id]);
            type = CAMERATYPE::MIGHTEX;
            break;
        case ALLIEDVISION:
            // TO DO mightex implementation
            camera = new AlliedVision();
            isCamera = true;
            result = camera->Connect(camera_list.id[id]);
            type = CAMERATYPE::ALLIEDVISION;
            break;
        case CAMTYPE_LAST:
            break;
        }
        if (result < 0) {
            std::cerr << "CameraManager :: CameraConect. Erreur connecting camera" << std::endl;
            delete camera;
            isCamera = false;
            emit signal_FailledCamConnect();
            return;
        }
        emit signal_EndOfCamConnect(camera, type);
        return;
    }

    std::cerr << "CameraManager :: CameraConect. Out of bound Id for connecting Camera"
              << std::endl;
    emit signal_FailledCamConnect();
}
