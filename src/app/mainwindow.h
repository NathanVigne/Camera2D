#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGraphicsView>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMutex>
#include <QMutexLocker>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpacerItem>
#include "gldisplay.h"
#include "icamera.h"
#include <iostream>

//QT_BEGIN_NAMESPACE
//namespace Ui { class MainWindow; }
//QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void callBackDraw();

    void slot_CameraOpen(ICamera *camera);

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
    // TO DO

    // Check Box slots
    // TO DO

private:
    void uiSetUp();

private:
    ICamera *cam = nullptr;
    QMutex m_mutex;

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
    QGraphicsView *xcutDisplay;
    QGraphicsView *ycutDisplay;

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
