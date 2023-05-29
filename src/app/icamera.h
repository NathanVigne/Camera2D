#ifndef ICAMERA_H
#define ICAMERA_H

#include <functional>
#include <mutex>
#include <string>
#include <vector>

struct CamNamesIDs
{
    std::vector<std::string> names;
    std::vector<std::string> ids;
};

enum TRIGGER { CONTINUOUS, SINGLE_SHOT, TRIGGER_END };

enum BUFFTYPE { U8, U16, BUFF_END };

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
    virtual long long GetExposure() = 0;
    virtual void SetGain(const double gain_db) = 0;

    // Camera managment
    virtual int Connect(std::string ID) = 0;
    virtual void Disconnect() = 0;
    virtual CamNamesIDs SearchCam() = 0;
    virtual void Initialize() = 0;

    // Setter and Getter
    void setMutex(std::mutex *mut) { m_mutex = mut; };
    std::mutex *getMutex() { return m_mutex; };
    double getMinGain() { return min_gain; };
    double getMaxGain() { return max_gain; };
    long long getMinExposure() { return min_exposure; };
    long long getMaxExposure() { return max_exposure; };
    int getSensorHeigth() { return sensorHeight_px; };
    int getSensorWidth() { return sensorWidth_px; };
    int getBitDepth() { return bit_depth; };
    BUFFTYPE getBuffType() { return buff_type; };

    virtual void setFrameReadyCallback(std::function<void()> frameReadyCallback) = 0;
    virtual void setDisconnectCbck(std::function<void()> disconnectCbck) = 0;
    virtual void setConnectCbck(std::function<void()> connectCbck) = 0;

    // TO DO : check usefullnes ?
    unsigned short *temp_image_buffer = nullptr;

protected:
    // callBack = emit signal when frame ready
    std::function<void()> m_frameReadyCallback;
    std::function<void()> m_disconnectCbck;
    std::function<void()> m_connectCbck;

    bool isConnected = false;
    void *handle = nullptr;

    double min_gain = 0;
    double max_gain = 0;
    long long min_exposure = 0;
    long long max_exposure = 0;

    int sensorHeight_px = 0;
    int sensorWidth_px = 0;
    double pixelWidth = 0;
    double pixelHeight = 0;
    int bit_depth = 0;
    BUFFTYPE buff_type = BUFF_END;
    std::mutex *m_mutex;

    unsigned char *temp_metadata_buffer = nullptr;
};

#endif // ICAMERA_H
