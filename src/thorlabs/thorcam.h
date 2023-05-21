#ifndef THORCAM_H
#define THORCAM_H

#include <iostream>

#include "../app/icamera.h"
#include "tl_camera_sdk.h"
#include "tl_camera_sdk_load.h"

class ThorCam : public ICamera
{
public:
    ~ThorCam() override;

    // Camera Control
    void Start() override;
    void Stop() override;
    void SingleShot() override;
    void SetExposure(const long long exposure_us) override;
    long long GetExposure() override;
    void SetGain(const double gain_db) override;

    // Connect / Disconect
    int Connect(std::string ID) override;
    void Disconnect() override;
    CamNamesIDs SearchCam() override;
    void Initialize() override;

private:
    // Specific Thorlabs mehtods and properties
    bool isDLLOpen = false;
    bool isSDKOpen = false;
    bool isRunning = false;
    bool isDiscover = false;
    bool isFirstFrameFinished = false;

    int InitializedDLL();
    int CloseRessources();

    void LoadGain();
    void LoadExposure();
    void LoadSensorInfo();
    void LoadInfo();

    void SetTrigger(TRIGGER trig);
    void SetConnectCallback(void *context);
    void SetDisconnectCallback(void *context);
    void SetFrameAvailableCallback(void *context);

    // Static callback funtions
    static void CameraConnectCallback(char *cameraSerialNumber,
                                      TL_CAMERA_USB_PORT_TYPE usb_bus_speed,
                                      void *context);
    static void CameraDisconnectCallback(char *cameraSerialNumber, void *context);
    static void FrameAvailableCallback(void *sender,
                                       unsigned short *image_buffer,
                                       int frame_count,
                                       unsigned char *metadata,
                                       int metadata_size_in_bytes,
                                       void *context);
};

#endif // THORCAM_H
