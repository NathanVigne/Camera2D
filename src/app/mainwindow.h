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
#include "fit.h"
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
    void slot_Zoom(double newZoom);
    void slot_ResetZoom();

    // Gain slot
    void slot_Gain(double newGain);

    // Exposure slot
    void slot_Exposure(int newExposure);

    // Color slots
    void slot_Color(bool check);

    void receivedLabelX(std::mutex *mutex, double *params);
    void receivedLabelY(std::mutex *mutex, double *params);

private:
    void uiSetUp();
    void sendFrameReady();
    void sendDisconnect();
    void sendReConnect();
    void displayDisconnect();
    void displayReConnect();
    void updateText();

private:
    ICamera *cam = nullptr;
    bool isRunning = false;
    std::mutex m_mutexSave;
    std::mutex m_mutexDisplay;
    MemoryManager *mem;
    CAMERATYPE cam_type;
    QMessageBox *msg; // do not keep here ??

    double wx_d;
    double wy_d;
    double wx_f;
    double wy_f;
    double zoom_ = 1.0;
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
    QSpacerItem *sZoom1;
    QSpacerItem *sZoom2;
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
    QLabel *labelzoom;

    // Layouts
    QHBoxLayout *lControls;
    QVBoxLayout *lColors;
    QVBoxLayout *lGain;
    QVBoxLayout *lExposure;
    QVBoxLayout *rightLayout;

    // ----------------------------------------
    // Main display
    // ----------------------------------------
    GLDisplay *mainDisplay;

    // ----------------------------------------
    // secondary display
    // ----------------------------------------
    MyChart *xcutDisplay;
    Fit *xFit;
    MyChart *ycutDisplay;
    Fit *yFit;

    // Group Boxes
    QGroupBox *gbSecondaryDisplay;

    // Check Boxes
    QCheckBox *cbCentrage;
    QDoubleSpinBox *dsbZoom;

    // Label
    QLabel *labelInfo;

    // Layout
    QHBoxLayout *lCheckBoxes;
    QVBoxLayout *lSecondaryDisplay;
};
#endif // MAINWINDOW_H
