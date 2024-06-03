#ifndef ALLIEDVISION_H
#define ALLIEDVISION_H

#include "../app/icamera.h"
#include <iostream>


class AlliedVision : public ICamera
{
public:
    ~AlliedVision() override;

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

};

#endif // ALLIEDVISION_H
