#ifndef THORLABS_H
#define THORLABS_H
#include <QDebug>

/*!
    Header file with wrapper funtion to use a thorlabs camera 
*/
enum thor_ERROR {
    ERROR_INIT_DLL,
    ERROR_CONNECT_SDK,
    ERROR_SEARCH_CAM,
    ERROR_CONNECT_CAM,
    ERROR_DISCONNECT_CAM

};

struct CameraNameId
{
    QStringList camera_Name;
    QStringList camera_ID;
};

// Initialisation
void thor_intializeDLLs();
CameraNameId thor_searchCamera();
void *thor_connectCamera(int id);

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
