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
    void SetExposure() override;
    void SetGain() override;

    // Connect / Disconect
    int Connect(std::string ID) override;
    void Disconnect() override;
    CamNamesIDs SearchCam() override;

private:
    // Specific Thorlabs mehtods and properties
    int InitializedDLL();
    int CloseRessources();

    bool isDLLOpen = false;
    bool isSDKOpen = false;
};

#endif // THORCAM_H
