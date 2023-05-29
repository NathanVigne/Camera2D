#include "thorcam.h"
#include <cstring>

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
    
    Thorlabs implementation of the public Interface function to
    start continuous aquisition. If camera already running does nothing
    Error are logged;

*/
void ThorCam::Start()
{
    if (isRunning) {
        return;
    }

    SetTrigger(CONTINUOUS);
    // Arm the camera.
    // if Hardware Triggering, make sure to set the operation mode before arming the camera.
    int result = tl_camera_arm(handle, 2);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Arm camera : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }

    // SOFTWARE TRIGGER
    result = tl_camera_issue_software_trigger(handle);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Issue software trigger : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }
    isRunning = true;
}

/*! \fn void ThorCam::Stop()
    
    Thorlabs implementation of the public Interface function to
    stop aquisition. If camera already running it first
    stop aquisition then start a single shot event !

*/
void ThorCam::Stop()
{
    // Stop the camera.
    int result = tl_camera_disarm(handle);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Disarm camera : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }
    isRunning = false;
    isFirstFrameFinished = false;
}

/*! \fn void ThorCam::SingleShot()
    
    Thorlabs implementation of the public Interface function to
    start single shot aquisition. If camera already running it first
    stop aquisition then start a single shot event !

*/
void ThorCam::SingleShot()
{
    if (isRunning) {
        Stop();
    }

    SetTrigger(SINGLE_SHOT);
    // Arm the camera.
    // if Hardware Triggering, make sure to set the operation mode before arming the camera.
    int result = tl_camera_arm(handle, 2);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Arm camera : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }

    // SOFTWARE TRIGGER
    result = tl_camera_issue_software_trigger(handle);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Issue software trigger : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }

    // wait for end of aquisition (need to think of better way with trhead !!)
    isRunning = true;
    while (!isFirstFrameFinished) {
    };
    Stop();
}

/*! \fn void ThorCam::SetExposure(long long exposure_us)
    
    Thorlabs implementation of the public Interface function to
    set the camera exposure. the exposure must be in µs
    Error are logged;

*/
void ThorCam::SetExposure(const long long exposure_us)
{
    // Set the exposure
    int result = tl_camera_set_exposure_time(handle, exposure_us);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Set Exposure : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }
}

long long ThorCam::GetExposure()
{
    long long exp = 0;
    int result = tl_camera_get_exposure_time(handle, &exp);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to get Exposure : " << tl_camera_get_last_error()
                  << std::endl;
        return exp;
    }
    return exp;
}

/*! \fn void ThorCam::SetGain(const double gain_db)
    
    Thorlabs implementation of the public Interface function to
    set the camera gain. the gain must be in dB
    Error are logged;

*/
void ThorCam::SetGain(const double gain_db)
{
    if (max_gain > 0) {
        // this camera supports gain, set it to gain_db
        int gain_index;
        int result = tl_camera_convert_decibels_to_gain(handle, gain_db, &gain_index);
        if (result != 0) {
            std::cerr << "Thorlab :: Failed to Convert Gain to int : " << tl_camera_get_last_error()
                      << std::endl;
            return;
        }
        result = tl_camera_set_gain(handle, gain_index);
        if (result != 0) {
            std::cerr << "Thorlab :: Failed to set Gain : " << tl_camera_get_last_error()
                      << std::endl;
            return;
        }
        return;
    }
    std::clog << "Thorlab :: Max gain too low : " << std::endl;
}

/*! \fn int ThorCam::Connect(std::string ID)
    
    ThorCam implementation of the connect function.
    From a string ID we connect to the thorlabs camera
    and setting the camera handle.
    Errors are logged and the function return 0 is succesfull
    or -1 ortherwise. 

*/
int ThorCam::Connect(std::string ID)
{
    if (!isDLLOpen || !isSDKOpen) {
        InitializedDLL();
    }

    if (!isDiscover) {
        char buffer[256];
        int result = tl_camera_discover_available_cameras(buffer, sizeof(buffer));
        if (result != 0) {
            // deal with error "Failed to discover camera"
            std::cerr << "Thorlab :: Failed to discover camera : " << tl_camera_get_last_error()
                      << std::endl;
            isDiscover = false;
            CloseRessources(); // is it obligated ?
            return -2;
        }
        isDiscover = true;
    }

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
        isDiscover = false;
        CloseRessources(); // is it obligated ?
        return name_id;
    }
    isDiscover = true;
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

/*! \fn void ThorCam::Initialize()
    
    ThorCam implementation of the camera initialization.
    This function must prepare the camera for aquisition
    by setting the callback function, getting relevant information such as
    min/max gain, min/max exposure, ...
    Errors are logged.

*/
void ThorCam::Initialize()
{
    // Set-up the call-back function
    SetConnectCallback((void *) this);
    SetDisconnectCallback((void *) this);
    SetFrameAvailableCallback((void *) this);

    //Load Sensor info
    LoadInfo();

    // initialize buffer for the Frame callBack
    temp_image_buffer = (unsigned short *) malloc(sizeof(unsigned short) * sensorWidth_px
                                                  * sensorHeight_px);
    temp_metadata_buffer = (unsigned char *) malloc(sizeof(unsigned char) * 7 * 8);
}

/*!
 * \brief ThorCam::setFrameReadyCallback
 * \param frameReadyCallback
 * 
 * Setting the callback for when frame is ready.
 * the callback function must be set in mainWindow or mainDisplay
 * 
 */
void ThorCam::setFrameReadyCallback(std::function<void()> frameReadyCallback)
{
    m_frameReadyCallback = frameReadyCallback;
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

/*! \fn void ThorCam::LoadGain()
    
    ThorCam specific implementation to load min/ max gain.
    The Gain is in dB type double!
    Errors are logged.

*/
void ThorCam::LoadGain()
{
    int minGain_int = 0;
    int maxGain_int = 0;
    int result = tl_camera_get_gain_range(handle, &minGain_int, &maxGain_int);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to load Gain : " << tl_camera_get_last_error() << std::endl;
        return;
    }
    min_gain = (double) minGain_int / 10.0;
    max_gain = (double) maxGain_int / 10.0;
}

/*! \fn void ThorCam::LoadExposure()
    
    ThorCam specific implementation to load min/max exposure.
    The exposure is in µs (type long long)
    Errors are logged.

*/
void ThorCam::LoadExposure()
{
    long long min_exposure_temp;
    long long max_exposure_temp;
    int result = tl_camera_get_exposure_time_range(handle, &min_exposure_temp, &max_exposure_temp);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to load Exposure : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }
    min_exposure = min_exposure_temp;
    max_exposure = max_exposure_temp;
}

/*! \fn void ThorCam::LoadSensorInfo()
    
    ThorCam specific implementation to load Sensor info.
    Errors are logged.

*/
void ThorCam::LoadSensorInfo()
{
    int result = tl_camera_get_sensor_height(handle, &sensorHeight_px);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to load sensor Height (px) : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }

    result = tl_camera_get_sensor_width(handle, &sensorWidth_px);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to load sensor Width (px) : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }

    result = tl_camera_get_sensor_pixel_width(handle, &pixelWidth);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to load pixel Width (um) : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }

    result = tl_camera_get_sensor_pixel_height(handle, &pixelHeight);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to load pixel Height (um) : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }
}

/*!
 * \brief ThorCam::LoadBitDepth
 * 
 * Get info about bitDepth of the buffer
 * 
 */
void ThorCam::LoadBitDepth()
{
    int result = tl_camera_get_bit_depth(handle, &bit_depth);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to load bit depth : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }
}

/*!
 * \brief ThorCam::LoadBufferType
 * 
 * Get Info about buffer type
 * 
 */
void ThorCam::LoadBufferType()
{
    buff_type = U16;
}

/*! \fn void ThorCam::LoadInfo()
    
    ThorCam specific implementation to load all relevant camera infos.
    Errors are logged.

*/
void ThorCam::LoadInfo()
{
    LoadGain();
    LoadExposure();
    LoadSensorInfo();
    LoadBitDepth();
    LoadBufferType();
}

/*! \fn void ThorCam::SetTrigger(TRIGGER trig) {}
    
    ThorCam specific implementation to to configure the Trigger mode
    (single vs vontinuous).
    Errors are logged.

*/
void ThorCam::SetTrigger(TRIGGER trig)
{
    int result;
    switch (trig) {
    case CONTINUOUS:
        // Configure camera for continuous acquisition by setting the number of frames to 0.
        result = tl_camera_set_frames_per_trigger_zero_for_unlimited(handle, 0);
        break;
    case SINGLE_SHOT:
        // Configure camera for single shot acquisition by setting the number of frames to 1.
        result = tl_camera_set_frames_per_trigger_zero_for_unlimited(handle, 1);
        break;
    case TRIGGER_END:
        result = 0;
        break;
    }
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Set the trigger mode : " << tl_camera_get_last_error()
                  << std::endl;
        return;
    }
}

/*! \fn void ThorCam::SetConnectCallback(void *context)
    
    ThorCam specific implementation to set the connect callback.
    This is used to register for run time camera connect events.
    a user context can be passed to be access in the callback function.
    Errors are logged.

*/
void ThorCam::SetConnectCallback(void *context)
{
    int result = tl_camera_set_camera_connect_callback(CameraConnectCallback, context);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Set the Connect Callback : "
                  << tl_camera_get_last_error() << std::endl;
        return;
    }
}

/*! \fn void ThorCam::SetDisconnectCallback(void *context)
    
    ThorCam specific implementation to set the disconnect callback.
    This is used to register for run time camera disconnect events.
    a user context can be passed to be access in the callback function.
    Errors are logged.

*/
void ThorCam::SetDisconnectCallback(void *context)
{
    int result = tl_camera_set_camera_disconnect_callback(CameraDisconnectCallback, context);
    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Set the disconnect Callback : "
                  << tl_camera_get_last_error() << std::endl;
        return;
    }
}

/*! \fn void ThorCam::SetFrameAvailableCallback(TRIGGER trig, void *context)
    
    ThorCam specific implementation to set the Frame Available callback.
    A user context can be passed to be access in the callback function.
    Errors are logged.

*/
void ThorCam::SetFrameAvailableCallback(void *context)
{
    // Set the frame available callback
    int result = tl_camera_set_frame_available_callback(handle, FrameAvailableCallback, context);

    if (result != 0) {
        std::cerr << "Thorlab :: Failed to Set the Frame Available Callback : "
                  << tl_camera_get_last_error() << std::endl;
        return;
    }
}

/*! \fn void ThorCam::CameraConnectCallback(char *cameraSerialNumber,
                                    TL_CAMERA_USB_PORT_TYPE usb_bus_speed,
                                    void *context)
    
    ThorCam specific implementation of the Connect callback.

*/
void ThorCam::CameraConnectCallback(char *cameraSerialNumber,
                                    TL_CAMERA_USB_PORT_TYPE usb_bus_speed,
                                    void *context)
{
    std::clog << "Thorlab :: ConnectCallback called !" << std::endl;
}

/*! \fn void ThorCam::CameraDisconnectCallback(char *cameraSerialNumber, void *context)

    ThorCam specific implementation of the Disconnect callback.

*/
void ThorCam::CameraDisconnectCallback(char *cameraSerialNumber, void *context)
{
    std::clog << "Thorlab :: DisconnectCallback called !" << std::endl;
}

/*! \fn void ThorCam::FrameAvailabelCallback(void *sender,
                                     unsigned short *image_buffer,
                                     int frame_count,
                                     unsigned char *metadata,
                                     int metadata_size_in_bytes,
                                     void *context)

    ThorCam specific implementation of the FrameAvailabelCallback callback.

*/
void ThorCam::FrameAvailableCallback(void *sender,
                                     unsigned short *image_buffer,
                                     int frame_count,
                                     unsigned char *metadata,
                                     int metadata_size_in_bytes,
                                     void *context)
{
    std::clog << "Thorlab :: FrameAvailabelCallback called !" << std::endl;
    ThorCam *ctx = (ThorCam *) context;
    std::scoped_lock lock{*(ctx->m_mutex)};

    memcpy(ctx->temp_image_buffer,
           image_buffer,
           (sizeof(unsigned short) * ctx->sensorHeight_px * ctx->sensorWidth_px));
    ctx->isFirstFrameFinished = true;
    ctx->m_frameReadyCallback();
    //emit ctx->frameReady();
}
