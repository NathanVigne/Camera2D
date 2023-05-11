#ifndef CONNECTWINDOW_H
#define CONNECTWINDOW_H

#include <QComboBox>
#include <QDebug>
#include <QEvent>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <thorlabs.h>

class ConnectWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectWindow(QWidget *parent = nullptr);
    ~ConnectWindow();

    // public function
    void refresh();

public slots:
    // slot for receiving info from cam loader
    void slot_cameraFound(CameraNameId *names_ids);
    void slot_cameraConnect(void *camera_handle);
    void slot_failledCamConnect();

private slots:
    // slot when button pressed
    void slot_connectClicked();
    void slot_refrechClicked();

signals:
    // signal to send cam loader when button pressed
    void signal_Connect(int);
    void signal_Refresh();

    // signal to send when a camera is connected
    void signal_cameraConnected(void *);

private:
    QPushButton *boutton_connect;
    QPushButton *boutton_refresh;
    QComboBox *cam_name_display;
    QVBoxLayout *v_layout;
    QHBoxLayout *h_layout;
    QLabel *label;
};

#endif // CONNECTWINDOW_H