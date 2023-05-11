#ifndef THORLABS_H
#define THORLABS_H
#include <QDebug>
#include "tl_camera_sdk.h"
#include "tl_camera_sdk_load.h"

/*!
    Header file with wrapper funtion to use a thorlabs camera 
*/

/*!
    Error enum to display and do specific actions
*/
enum thor_ERROR {
    ERROR_INIT_DLL,
    ERROR_CONNECT_SDK,
    ERROR_SEARCH_CAM,
    ERROR_CONNECT_CAM,
    ERROR_DISCONNECT_CAM

};

/*!
    Struc to encapsulate names and IDs of connected camera
*/
struct CameraNameId
{
    QStringList camera_Name;
    QStringList camera_ID;
};

// Initialisation
void thor_intializeDLLs();
CameraNameId thor_searchCamera();
void *thor_connectCamera(QString *ID);

// callback function (Pas sur d'avoir besoin de les mettre ici !!
// a réfléchir
void thor_frameCallback();
void thor_connectCallback();
void thor_disconnectCallback();

// camera control
void thor_startCamera();
void thor_stopCamera();
void thor_singleShot();
void thor_setExposure();
void thor_setGain();

// closing
void thor_closeRessources();

// error handling
void thor_errorHandling(thor_ERROR error);

#endif // THORLABS_H
