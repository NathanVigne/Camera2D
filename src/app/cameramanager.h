#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QThread>
#include "icamera.h"
#include "thorcam.h"

enum CAMERATYPE { THORLABS, CAMTYPE_LAST, MIGHTEX };

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
    // used to start new thread !!
    void slot_StartScan();
    void slot_StartConnect(int id);

private slots:
    // Slot to do some private work in a thread
    void DiscoverCameras();
    void CameraConnect(int id);

signals:
    // Signal emitted to launch the private slot in
    // a new thread
    void signal_StartScan();
    void signal_StartConnect(int);

    // Signal to send the ConnectWindow when
    // the private work is done
    void signal_EndOfCamScan(CamList *);
    void signal_EndOfCamConnect(ICamera *, CAMERATYPE);
    void signal_FailledCamConnect();

private:
    CamList camera_list;

    ICamera *camera;
    bool isCamera = false;

    QThread thread;
};

#endif // CAMERAMANAGER_H
