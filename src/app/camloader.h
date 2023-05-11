#ifndef CAMLOADER_H
#define CAMLOADER_H

#include <QString>
#include <QThread>
#include <thorlabs.h>

class CamLoader : public QObject
{
    Q_OBJECT;

public:
    CamLoader(QObject *parent = NULL);
    ~CamLoader();

public slots:
    // Slot for receiving info from ConnectWindow
    // used to start new thread !!
    void slot_StartScan();
    void slot_StartConnect(int id);

private slots:
    // Slot to do some private work in a thread
    void slot_scanForCamera();
    void slot_connectToCamera(int id);

signals:
    // Signal emitted to launch the private slot in
    // a new thread
    void signal_startScan();
    void signal_startConnect(int);

    // Signal to send the ConnectWindow when
    // the private work is done
    void signal_endOfCamScan(CameraNameId *);
    void signal_endOfCamConnect(void *);
    void signal_failledCamConnect();

private:
    CameraNameId names_ids;
    QThread thread;

    void *open_cam_handle;
};

#endif // CAMLOADER_H
