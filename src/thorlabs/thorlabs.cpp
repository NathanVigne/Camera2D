#include "thorlabs.h"
#include "tl_camera_sdk.h"
#include "tl_camera_sdk_load.h"
#include <tuple>

// Initialisation
void thor_intializeDLLs()
{
    // Initializes camera dll
    if (tl_camera_sdk_dll_initialize()) {
        // deal with error "Failed to initialized"
        thor_errorHandling(thor_ERROR::ERROR_INIT_DLL);
        return;
    }

    // Open the camera SDK
    if (tl_camera_open_sdk()) {
        // deal with error "Failed to open SDK"
        thor_errorHandling(thor_ERROR::ERROR_CONNECT_SDK);
        return;
    }
}

CameraNameId thor_searchCamera()
{
    char buffer_ids[1024];
    buffer_ids[0] = 0;

    CameraNameId name_id = {{""}, {""}};

    // Discover cameras.
    if (tl_camera_discover_available_cameras(buffer_ids, 1024)) {
        // deal with error "Failed to discover camera"
        thor_errorHandling(thor_ERROR::ERROR_SEARCH_CAM);
        return name_id;
    }

    QString camera_ids = buffer_ids;

    // Check for no cameras.
    if (camera_ids.isEmpty()) {
        // need to send a signal or something !
        return name_id;
    }

    QStringList camera_ids_list = camera_ids.split(u' ');
    QStringList camera_names_list;
    void *camera_handle = 0;
    int strLength = 1024;
    char strBuffer[1024];

    foreach (QString item, camera_ids_list) {
        // Connect to the camera (get a handle to it).
        if (tl_camera_open_camera(item.toUtf8().data(), &camera_handle)) {
            // deal with error "Failed to discover camera"
            thor_errorHandling(thor_ERROR::ERROR_CONNECT_CAM);
            camera_names_list.append("Cam Error : Connect");
        } else {
            // Get Name
            if (tl_camera_get_name(camera_handle, strBuffer, strLength)) {
                camera_names_list.append("Cam Error : Name");
            } else {
                camera_names_list.append("Thorlabs : " + QString(strBuffer));
            }

            // disconnect
            if (tl_camera_close_camera(camera_handle)) {
                thor_errorHandling(thor_ERROR::ERROR_DISCONNECT_CAM);
                camera_handle = 0;
            }
        }
    }

    name_id.camera_ID = camera_ids_list;
    name_id.camera_Name = camera_names_list;

    return name_id;
}

// closing
void thor_closeRessources()
{
    tl_camera_close_sdk();
    tl_camera_sdk_dll_terminate();
}

// error handling
void thor_errorHandling(thor_ERROR error)
{
    switch (error) {
    case thor_ERROR::ERROR_INIT_DLL:
        qDebug() << "Thorlab :: Failed to initialized DLL"
                 << "Clossing ressources";
        break;
    case thor_ERROR::ERROR_CONNECT_SDK:
        qDebug() << "Thorlab :: Failed to connect with SDK"
                 << "Clossing ressources";
        tl_camera_sdk_dll_terminate();
        break;
    case thor_ERROR::ERROR_SEARCH_CAM:
        qDebug() << "Thorlab :: Failed to discover camera : " << tl_camera_get_last_error()
                 << "Clossing ressources";
        tl_camera_close_sdk();
        tl_camera_sdk_dll_terminate();
        break;
    case thor_ERROR::ERROR_CONNECT_CAM:
        qDebug() << "Thorlab :: Failed to connect to camera : " << tl_camera_get_last_error();
        //tl_camera_close_sdk();
        //tl_camera_sdk_dll_terminate();
        break;
    case thor_ERROR::ERROR_DISCONNECT_CAM:
        qDebug() << "Thorlab :: Failed to disconnect to camera : " << tl_camera_get_last_error();
        //tl_camera_close_sdk();
        //tl_camera_sdk_dll_terminate();
        break;
    }
}
