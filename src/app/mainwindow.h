#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpacerItem>
#include "cameramanager.h"
#include "exportwindow.h"
#include "gldisplay.h"
#include "icamera.h"
#include "memorymanager.h"
#include "mychart.h"
#include <iostream>
#include <mutex>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void frameReady();
    void camDisconnect();
    void camReConnect();

public slots:

    // Drawing slots
    void callBackDraw();
    void slot_CameraOpen(ICamera *camera, CAMERATYPE type);

    // Push Button slots
    void slot_Start();
    void slot_Stop();
    void slot_SingleShot();
    void slot_Export();
    void slot_Quit();
    void slot_ResetZoom();

    // Gain slot
    void slot_Gain(double newGain);

    // Exposure slot
    void slot_Exposure(int newExposure);

    // Color slots
    void slot_Color(bool check);

    // Check Box slots
    // TO DO

private:
    void uiSetUp();
    void sendFrameReady();
    void sendDisconnect();
    void sendReConnect();
    void displayDisconnect();
    void displayReConnect();

private:
    ICamera *cam = nullptr;
    bool isRunning = false;
    std::mutex m_mutex;
    MemoryManager *mem;
    CAMERATYPE cam_type;
    QMessageBox *msg;

    // ----------------------------------------
    // Central widget
    // ----------------------------------------
    QWidget *window;
    QGridLayout *mainLayout;

    // ----------------------------------------
    // Push Buttons properties
    // ----------------------------------------
    QPushButton *bStart;
    QPushButton *bStop;
    QPushButton *bSingleShot;
    QPushButton *bExport;
    QPushButton *bQuit;
    QPushButton *bResetZoom;

    // Layouts + spacer
    QSpacerItem *sButtons;
    QHBoxLayout *lButtons;

    // ----------------------------------------
    // Camera Gain/Exposure/Color controls properties
    // ----------------------------------------
    // Controls
    QSlider *sliderExposure;
    QDoubleSpinBox *dsbGain;
    QRadioButton *rbMonochrome;
    QRadioButton *rbSatMonochrome;
    QRadioButton *rbSatColor;

    // Boxes
    QGroupBox *gbSlider;
    QGroupBox *gbGain;
    QGroupBox *gbColor;

    // Labels
    QLabel *labelExposure;

    // Layouts
    QHBoxLayout *lControls;
    QVBoxLayout *lColors;
    QVBoxLayout *lGain;
    QVBoxLayout *lExposure;

    // ----------------------------------------
    // Main display
    // ----------------------------------------
    GLDisplay *mainDisplay;

    // ----------------------------------------
    // secondary display
    // ----------------------------------------
    MyChart *xcutDisplay;
    MyChart *ycutDisplay;

    // Group Boxes
    QGroupBox *gbSecondaryDisplay;

    // Check Boxes
    QCheckBox *cbCentrage;
    QCheckBox *cbEnergy;

    // Label
    QLabel *labelInfo;

    // Layout
    QHBoxLayout *lCheckBoxes;
    QVBoxLayout *lSecondaryDisplay;
};
#endif // MAINWINDOW_H
