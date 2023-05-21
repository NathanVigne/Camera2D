#ifndef ICAMERA_H
#define ICAMERA_H

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <string>
#include <vector>

struct CamNamesIDs
{
    std::vector<std::string> names;
    std::vector<std::string> ids;
};

enum TRIGGER { CONTINUOUS, SINGLE_SHOT, TRIGGER_END };

/// \brief Camera interface
class ICamera : public QObject
{
    Q_OBJECT
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

    double getMinGain() { return min_gain; };
    double getMaxGain() { return max_gain; };
    long long getMinExposure() { return min_exposure; };
    long long getMaxExposure() { return max_exposure; };
    int getSensorHeigth() { return sensorHeight_px; };
    int getSensorWidth() { return sensorWidth_px; };

    QMutex *m_mutex;
    unsigned short *temp_image_buffer = nullptr;

signals:
    void frameReady();

protected:
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
    unsigned char *temp_metadata_buffer = nullptr;
};

#endif // ICAMERA_H
