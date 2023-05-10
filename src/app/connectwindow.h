#ifndef CONNECTWINDOW_H
#define CONNECTWINDOW_H

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

class ConnectWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectWindow(QWidget *parent = nullptr);
    ~ConnectWindow();

public slots:
    void cameraFound(QStringList *cam_name);
    void cameraSelect();

signals:
    void cameraConnect(QString, int);

private:
    QPushButton *boutton_connect;
    QComboBox *cam_name_display;
    QVBoxLayout *layout;
    QLabel *label;
};

#endif // CONNECTWINDOW_H
