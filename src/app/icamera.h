#ifndef ICAMERA_H
#define ICAMERA_H

#include <string>
#include <vector>

struct CamNamesIDs
{
    std::vector<std::string> names;
    std::vector<std::string> ids;
};

enum TRIGGER { CONTINUOUS, SINGLE_SHOT, TRIGGER_END };

/// \brief Camera interface
class ICamera
{
public:
    virtual ~ICamera() {}

    // Camera Control
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void SingleShot() = 0;
    virtual void SetExposure(const long long exposure_us) = 0;
    virtual void SetGain(const double gain_db) = 0;

    // Camera managment
    virtual int Connect(std::string ID) = 0;
    virtual void Disconnect() = 0;
    virtual CamNamesIDs SearchCam() = 0;
    virtual void Initialize() = 0;

protected:
    bool isConnected = false;
    void *handle = nullptr;

    double min_gain;
    double max_gain;
    long long min_exposure;
    long long max_exposure;

    int sensorHeight_px;
    int sensorWidth_px;
    double pixelWidth;
    double pixelHeight;

    unsigned short *temp_image_buffer;
    unsigned char *temp_metadata_buffer;
};

#endif // ICAMERA_H
