#ifndef ICAMERA_H
#define ICAMERA_H

#include <string>
#include <vector>

struct CamNamesIDs
{
    std::vector<std::string> names;
    std::vector<std::string> ids;
};

/// \brief Camera interface
class ICamera
{
public:
    virtual ~ICamera(){};

    // Camera Control
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void SingleShot() = 0;
    virtual void SetExposure() = 0;
    virtual void SetGain() = 0;

    // Camera managment
    virtual int Connect(std::string ID) = 0;
    virtual void Disconnect() = 0;
    virtual CamNamesIDs SearchCam() = 0;

protected:
    bool isConnected = false;
    void *handle = nullptr;
};

#endif // ICAMERA_H
