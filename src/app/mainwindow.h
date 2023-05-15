#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGraphicsView>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpacerItem>
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
    void debugConnect(ICamera *camera);

private:
    //Ui::MainWindow *ui;
    void uiSetUp();

private:
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
    QGraphicsView *mainDisplay;

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
