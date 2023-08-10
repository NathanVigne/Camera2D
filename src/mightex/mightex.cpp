#include "mightex.h"

Mightex *Mightex::ctx;

Mightex::~Mightex()
{
    if (isDLLOpen) {
        if (isConnected) {
            Disconnect();
        }
        MTUSB_UnInitDevice();
        isDLLOpen = false;
    }
}

void Mightex::Start()
{
    if (isRunning) {
        return;
    }

    // SetTrigger Continuous
    int result = MTUSB_StartFrameGrab(m_camHandle, 0x8888);
    if (result != 1) {
        std::cerr << "Mightex :: Start :: Failed to Start (invalid camHandle) or engine not started"
                  << std::endl;
        return;
    }
    result = MTUSB_InstallFrameHooker(m_camHandle, true, 0, FrameAvailableCallback);
    if (result != 1) {
        std::cerr << "Mightex :: Start :: Failed to install frame hooker (invalid camHandle) or "
                     "engine not started"
                  << std::endl;
        return;
    }
    isRunning = true;
}

void Mightex::Stop()
{
    int result = MTUSB_StopFrameGrab(m_camHandle);
    if (result != 1) {
        std::cerr << "Mightex :: Stop :: Failed to Stop (invalid camHandle) or engine not started"
                  << std::endl;
        return;
    }
    isRunning = false;
}

void Mightex::SingleShot()
{
    if (isRunning) {
        Stop();
    }

    // SetTrigger One frame
    int result = MTUSB_StartFrameGrab(m_camHandle, 0x1);
    if (result != 1) {
        std::cerr
            << "Mightex :: SingleShot :: Failed to Start (invalid camHandle) or engine not started"
            << std::endl;
        return;
    }
    result = MTUSB_InstallFrameHooker(m_camHandle, true, 0, FrameAvailableCallback);
    if (result != 1) {
        std::cerr
            << "Mightex :: SingleShot :: Failed to install frame hooker (invalid camHandle) or "
               "engine not started"
            << std::endl;
        return;
    }
}

void Mightex::SetExposure(const long long exposure_us)
{
    // check for
    int maxExpTimeInd = 2;
    int exp = (int) exposure_us;

    if (exposure_us <= 750 * 1000) {
        maxExpTimeInd = 3;
    }
    if (exposure_us < 100 * 1000) {
        maxExpTimeInd = 2;
    }
    if (exposure_us < 10 * 1000) {
        maxExpTimeInd = 1;
    }
    if (exposure_us < 5 * 1000) {
        maxExpTimeInd = 0;
    }
    camSettings.MaxExposureTimeIndex = maxExpTimeInd;
    camSettings.ExposureTime = (int) exp;

    int result = MTUSB_SetExposureTime(m_camHandle, &camSettings);
    if (result != 1) {
        std::cerr << "Mightex :: SetExposure :: failed to set exposure (invalied cam handle)"
                  << std::endl;
        return;
    }
}

long long Mightex::GetExposure()
{
    loadCamSettings();
    long long exp = (long long) camSettings.ExposureTime;
    return exp;
}

void Mightex::SetGain(const double gain_db)
{
    // Chack for max gain ?
    int gain = (int) gain_db * 8;
    if (gain <= 128 && gain >= 0) {
        // convert to correct value and type ?
        camSettings.BlueGain = gain;
        camSettings.GreenGain = gain;
        camSettings.RedGain = gain;
    } else {
        std::cerr << "Mightex :: SetGain :: gain outside range gain_dg*8 : 0-128" << std::endl;
        return;
    }

    int result = MTUSB_SetGain(m_camHandle, &camSettings);
    if (result != 1) {
        std::cerr << "Mightex :: SetGain :: failed to set gain (invalied cam handle)" << std::endl;
        return;
    }
}

int Mightex::Connect(std::string ID)
{
    if (!isDLLOpen) {
        InitializedDLL();
    }

    int camID = stoi(ID);
    if (camID >= numbOfCam || camID < 0) {
        std::cerr << "Mightex :: Connect :: camID out of bound " << std::endl;
        isConnected = false;
        return -1;
    }

    if (!isConnected) {
        m_camHandle = MTUSB_OpenDevice(camID);
        if (m_camHandle == 0xFFFFFFFF) {
            std::cerr << "Mightex :: Connect :: Try to connect while cam engine started "
                      << std::endl;
            isConnected = false;
            return -1;
        }
        if (m_camHandle == -1) {
            std::cerr << "Mightex :: Connect :: Failled to connect camera " << std::endl;
            isConnected = false;
            return -1;
        }
        myID = ID;
        isConnected = true;
        return 0;
    }
    return 1;
}

void Mightex::Disconnect()
{
    if (isConnected) {
        if (isEngineOn) {
            MTUSB_StopCameraEngine(m_camHandle);
            isEngineOn = false;
        }
        m_camHandle = -1;
        isConnected = false;
    }
}

CamNamesIDs Mightex::SearchCam()
{
    CamNamesIDs name_id;

    if (!isDLLOpen) {
        InitializedDLL();
    }

    std::vector<std::string> camera_names_list;
    std::vector<std::string> camera_ids_list;
    char bufferA[16];
    char bufferB[16];

    int result;

    // Loop over all connected camera
    for (int i = 0; i < numbOfCam; ++i) {
        std::string ID = std::to_string(i);
        result = Connect(ID);
        if (result != 0) {
            continue;
        } else {
            // Get Name + S/N
            result = MTUSB_GetModuleNo(m_camHandle, bufferA);
            if (result != 1) {
                std::cerr << "Mightex :: SearchCam :: Failed to Get Module N0 : " << std::endl;
                continue;
            } else {
                // Get S/N or more likelly previous ID
                result = MTUSB_GetSerialNo(m_camHandle, bufferB);
                if (result != 1) {
                    std::cerr << "Mightex :: SearchCam :: Failed to Get S/N : " << std::endl;
                    continue;
                } else {
                    std::string name = bufferA;
                    std::string SN = bufferB;
                    camera_names_list.push_back("Mightex : " + name + " / SN : " + SN);
                    camera_ids_list.push_back(ID);
                }
            }

            // Disconnect
            Disconnect();
        }
    }

    name_id.ids = camera_ids_list;
    name_id.names = camera_names_list;

    return name_id;
}

void Mightex::Initialize()
{
    // Start Cam engine
    int result = MTUSB_StartCameraEngine(NULL, m_camHandle);
    if (result != 1) {
        std::cerr << "Mightex :: Initialize :: (invalid camHandle) or engine not started"
                  << std::endl;
        return;
    }
    isEngineOn = true;

    // Load camSettings
    loadCamSettings();

    // initialze for un-adujsted frame
    // Not usefull for rawData
    setUnAdjustedFrame();

    // Set all ICamera properties
    min_gain = 1;
    max_gain = 128 / 8;
    min_exposure = 0.05 * 1000;
    max_exposure = 750 * 1000;

    switch (camSettings.Resolution) {
    case 0:
        sensorHeight_px = 32;
        sensorWidth_px = 32;
        break;
    case 1:
        sensorHeight_px = 64;
        sensorWidth_px = 64;
        break;
    case 2:
        sensorHeight_px = 120;
        sensorWidth_px = 160;
        break;
    case 3:
        sensorHeight_px = 240;
        sensorWidth_px = 320;
        break;
    case 4:
        sensorHeight_px = 480;
        sensorWidth_px = 640;
        break;
    case 5:
        sensorHeight_px = 600;
        sensorWidth_px = 800;
        break;
    case 6:
        sensorHeight_px = 768;
        sensorWidth_px = 1024;
        break;
    case 7:
        sensorHeight_px = 1024;
        sensorWidth_px = 1280;
        break;
    case 8:
        sensorHeight_px = 1200;
        sensorWidth_px = 1600;
        break;
    case 9:
        sensorHeight_px = 1536;
        sensorWidth_px = 2048;
        break;
    }

    pixelHeight = 5.2;
    pixelWidth = 5.2;
    bit_depth = 8;
    buff_type = U8;

    // initialize buffer for the Frame callBack
    m_mem->allocateMem(sensorWidth_px, sensorHeight_px, buff_type);

    // instal P&P
    ctx = this;
    MTUSB_InstallDeviceHooker(CameraUSBEventCallback);

    // The frame callback must be install each time we
    // frame grabbing
}

void Mightex::setFrameReadyCallback(std::function<void()> frameReadyCallback)
{
    m_frameReadyCallback = frameReadyCallback;
}

void Mightex::setDisconnectCbck(std::function<void()> disconnectCbck)
{
    m_disconnectCbck = disconnectCbck;
}

void Mightex::setConnectCbck(std::function<void()> connectCbck)
{
    m_connectCbck = connectCbck;
}

int Mightex::InitializedDLL()
{
    numbOfCam = MTUSB_InitDevice();
    isDLLOpen = true;
    return 0;
}

int Mightex::loadCamSettings()
{
    int result = MTUSB_GetFrameSetting(m_camHandle, &camSettings);
    if (result != 1) {
        std::cerr << "Mightex :: loadCamSettings :: (invalid camHandle) or engine not started"
                  << std::endl;
        return -1;
    }
    return 0;
}

void Mightex::setUnAdjustedFrame()
{
    camSettings.Gamma = 10;
    camSettings.Bright = 50;
    camSettings.SharpLevel = 0;
    int result = MTUSB_SetGammaValue(m_camHandle, &camSettings);
    if (result != 1) {
        std::cerr << "Mightex :: setUnAdjustedFrame :: failed to set Gamma parameters (invalied "
                     "cam handle)"
                  << std::endl;
        return;
    }

    camSettings.BinMode = 1;
    camSettings.Resolution = 6;
    result = MTUSB_SetResolution(m_camHandle, &camSettings);
    if (result != 1) {
        std::cerr << "Mightex :: setUnAdjustedFrame :: failed to set Frame parameters (invalied "
                     "cam handle)"
                  << std::endl;
        return;
    }
}

// TO DO a lot of testing
// Seems to not work (seems to need to be in frameGrab ???)
// Maybe do cleanup and Re-Launch connect window !
void Mightex::CameraUSBEventCallback(int fault_type)
{
    switch (fault_type) {
    case 0: // A camera is removes
        std::clog << "Cam disconnected" << std::endl;
        ctx->m_disconnectCbck();
        ctx->Disconnect();
        break;
    case 1: // A camera is connected
        std::clog << "Cam connected" << std::endl;
        ctx->m_connectCbck();
        ctx->Connect(ctx->myID);
        break;
    case 2: // A USB error as occured
        std::clog << "USB error" << std::endl;
        ctx->m_disconnectCbck();
        ctx->Disconnect();
        break;
    }
}

void Mightex::FrameAvailableCallback(
    int FrameType, int Row, int Col, TImageAttachData *Attributes, unsigned char *BytePtr)
{
    std::scoped_lock lock{*(ctx->m_mutex)};
    memcpy(ctx->m_mem->save(), BytePtr, (sizeof(unsigned char) * Row * Col));
    ctx->m_mem->swap();
    ctx->m_frameReadyCallback();
}
