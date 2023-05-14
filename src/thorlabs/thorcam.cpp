#include "thorcam.h"

/*! \fn ThorCam::~ThorCam()
    
    Destructor of the ThroCam class.
    We close all opened ressources

*/
ThorCam::~ThorCam()
{
    CloseRessources();
    handle = 0;
}

/*! \fn void ThorCam::Start()
    
    TO DO

*/
void ThorCam::Start() {}

/*! \fn void ThorCam::Stop()
    
    TO DO

*/
void ThorCam::Stop() {}

/*! \fn void ThorCam::SingleShot()
    
    TO DO

*/
void ThorCam::SingleShot() {}

/*! \fn void ThorCam::SetExposure()
    
    TO DO

*/
void ThorCam::SetExposure() {}

/*! \fn void ThorCam::SetGain()
    
    TO DO

*/
void ThorCam::SetGain() {}

/*! \fn int ThorCam::Connect(std::string ID)
    
    ThorCam implementation of the connect function.
    From a string ID we connect to the thorlabs camera
    and setting the camera handle.
    Errors are logged and the function return 0 is succesfull
    or -1 ortherwise. 

*/
int ThorCam::Connect(std::string ID)
{
    if (!isConnected) {
        // Connect to the camera (get a handle to it).
        int result = tl_camera_open_camera((char *) ID.c_str(), &handle);
        if (result != 0) {
            // deal with error "Failed to discover camera"
            std::cerr << "Thorlab :: Failed to Open camera : " << tl_camera_get_last_error()
                      << std::endl;
            isConnected = false;
            return -1;
        }
        isConnected = true;
        return 0;
    }
    return 1;
}

/*! \fn int ThorCam::Disconnect(std::string ID)
    
    ThorCam implementation of the disconnect function.
    This function disconnect the openned camera.
    Errors are logged and the function return 0 is succesfull
    or -1 ortherwise. 

*/
void ThorCam::Disconnect()
{
    if (isConnected) {
        int result = tl_camera_close_camera(handle);
        if (result != 0) {
            //thor_errorHandling(thor_ERROR::ERROR_DISCONNECT_CAM);
            std::cerr << "Thorlab :: Failed to disconnect camera : " << tl_camera_get_last_error()
                      << std::endl;
            handle = 0;
        }
        isConnected = false;
    }
}

/*! \fn CamNamesIDs ThorCam::SearchCam()
    
    ThorCam implementation of the SearchCam function.
    This function open ressources if not already done. then look
    for connected cameras. If camera are found it returns the name/ID
    for each camera. Otherwise it return an empty struct.
    Errors are logged.

*/
CamNamesIDs ThorCam::SearchCam()
{
    CamNamesIDs name_id;

    if (!isDLLOpen || !isSDKOpen) {
        InitializedDLL();
    }

    // Discover cameras.
    char buffer[1024];
    buffer[0] = 0;
    int result = tl_camera_discover_available_cameras(buffer, sizeof(buffer));
    if (result != 0) {
        // deal with error "Failed to discover camera"
        std::cerr << "Thorlab :: Failed to discover camera : " << tl_camera_get_last_error()
                  << std::endl;
        CloseRessources(); // is it obligated ?
        return name_id;
    }
    std::string camera_ids = buffer;

    // Check for no camera
    if (camera_ids.empty()) {
        // need to send a signal or something !
        return name_id;
    }

    // Get all camera IDs and Names
    // different IDs are separated by white space
    std::vector<std::string> camera_ids_list;
    std::vector<std::string> camera_ids_list_temp;
    std::vector<std::string> camera_names_list;
    size_t start = 0, end = 0;
    while ((end = camera_ids.find(' ', start)) != std::string::npos) {
        camera_ids_list_temp.push_back(camera_ids.substr(start, end - start));
        start = end + 1;
    }
    camera_ids_list_temp.push_back(camera_ids.substr(start));

    // Loop on all cameras IDs to get the names
    for (const auto &item : camera_ids_list_temp) {
        // Connect to the camera (get a handle to it).
        result = Connect(item);
        if (result != 0) {
            continue;
        } else {
            // Get Name
            result = tl_camera_get_name(handle, buffer, sizeof(buffer));
            if (result != 0) {
                std::cerr << "Thorlab :: Failed to Get camera name : " << tl_camera_get_last_error()
                          << std::endl;
                //camera_names_list.push_back("Cam Error : Name");
                continue;
            } else {
                std::string name = buffer;
                camera_names_list.push_back("Thorlabs : " + name);
                camera_ids_list.push_back(item);
            }

            // disconnect
            Disconnect();
        }
    }

    name_id.ids = camera_ids_list;
    name_id.names = camera_names_list;

    return name_id;
}

/*! \fn int ThorCam::InitializedDLL()
    
    ThorCam specific implementation to open all ressources (DLL and SDK).
    Errors are logged and the function return 0 is succesfull or negative value.

*/
int ThorCam::InitializedDLL()
{
    // Initializes camera dll
    int result = tl_camera_sdk_dll_initialize();
    if (result != 0) {
        // deal with error "Failed to initialized"
        std::cerr << "Thorlab :: Failed to Initialized DLL" << std::endl;
        isDLLOpen = false;
        CloseRessources();
        return -1;
    }
    isDLLOpen = true;

    // Open the camera SDK
    result = tl_camera_open_sdk();
    if (result != 0) {
        // deal with error "Failed to open SDK"
        std::cerr << "Thorlab :: Failed to open SDK" << std::endl;
        isSDKOpen = false;
        CloseRessources();
        return -2;
    }
    isSDKOpen = true;
    return 0;
}

/*! \fn int ThorCam::CloseRessources()
    
    ThorCam specific implementation to close all ressources.
    Errors are logged and the function return 0 is succesfull.

*/
int ThorCam::CloseRessources()
{
    if (isDLLOpen) {
        if (isSDKOpen) {
            if (isConnected) {
                int result = tl_camera_close_camera(handle);
                if (result != 0) {
                    //thor_errorHandling(thor_ERROR::ERROR_DISCONNECT_CAM);
                    std::cerr << "Thorlab :: Failed to disconnect camera : "
                              << tl_camera_get_last_error() << std::endl;
                    handle = 0;
                }
                isConnected = false;
            }
            tl_camera_close_sdk();
        }
        tl_camera_sdk_dll_terminate();
    }
    return 0;
}
