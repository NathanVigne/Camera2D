#ifndef MIGHTEX_H
#define MIGHTEX_H

#include <windows.h> // Needed to define the HWMD type of the dll function !!

#include "../app/icamera.h"
#include "MT_USBCamera_SDK.h"
#include <iostream>

class Mightex : public ICamera
{
public:
    ~Mightex() override;

    // Camsera Control
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

    // callback
    void setFrameReadyCallback(std::function<void()> frameReadyCallback) override;
    void setDisconnectCbck(std::function<void()> disconnectCbck) override;
    void setConnectCbck(std::function<void()> connectCbck) override;

private:
    bool isDLLOpen = false;
    bool isEngineOn = false;
    bool isRunning = false;
    std::string myID;

    DEV_HANDLE m_camHandle = -1;
    TImageControl camSettings;

    int numbOfCam = 0;
    int InitializedDLL();
    int loadCamSettings();
    void setUnAdjustedFrame();

    // Static callback funtions
    static Mightex *ctx;
    static void CameraUSBEventCallback(int fault_type);
    static void FrameAvailableCallback(
        int FrameType, int Row, int Col, TImageAttachData *Attributes, unsigned char *BytePtr);
};

#endif // MIGHTEX_H
