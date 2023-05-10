#ifndef CONNECTWINDOW_H
#define CONNECTWINDOW_H

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>
#include <thorlabs.h>

class ConnectWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectWindow(QWidget *parent = nullptr);
    ~ConnectWindow();

public slots:
    void cameraFound(CameraNameId *names_ids);
    void connectClicked();
    void refrechClicked();

signals:
    void cameraConnect(QString, int);
    void cameraRefresh();

private:
    QPushButton *boutton_connect;
    QPushButton *boutton_refresh;
    QComboBox *cam_name_display;
    QVBoxLayout *v_layout;
    QHBoxLayout *h_layout;
    QLabel *label;

    QThread thread;
    // TO DO : remove when cam loader is done
    CameraNameId name_id;
};

#endif // CONNECTWINDOW_H
