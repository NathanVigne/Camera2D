#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QThread>
#include "icamera.h"
#include "mightex.h"
#include "thorcam.h"

enum CAMERATYPE { THORLABS, MIGHTEX, CAMTYPE_LAST };

struct CamList
{
    std::vector<std::string> name;
    std::vector<std::string> id;
    std::vector<CAMERATYPE> type;
};

class CameraManager : public QObject
{
    Q_OBJECT;

public:
    CameraManager(QObject *parent = nullptr);
    ~CameraManager();

public slots:
    // Slot for receiving info from ConnectWindow
    void DiscoverCameras();
    void CameraConnect(int id);

signals:
    // Signal to send the ConnectWindow when
    // the work is done
    void signal_EndOfCamScan(CamList *);
    void signal_EndOfCamConnect(ICamera *, CAMERATYPE);
    void signal_FailledCamConnect();

private:
    CamList camera_list;
    ICamera *camera;
    bool isCamera = false;
};

#endif // CAMERAMANAGER_H
