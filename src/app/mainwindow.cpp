#include "mainwindow.h"
//#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
//, ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    setWindowIcon(QIcon(":/icon/cam_ico.png"));
    uiSetUp();
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::debugConnect(ICamera *camera)
{
    std::clog << "ICamera pointer : " << camera << std::endl;
    this->show();
}

/*!
 * \brief MainWindow::uiSetUp
 * function to setup the ui of the main window
 */
void MainWindow::uiSetUp()
{
    // ----------------------------------------
    // Push Buttons Set Up
    // ----------------------------------------
    bStart = new QPushButton(this);
    bStart->setText("Start");
    bStop = new QPushButton(this);
    bStop->setText("Stop");
    bSingleShot = new QPushButton(this);
    bSingleShot->setText("Single shot");
    bExport = new QPushButton(this);
    bExport->setText("Export");
    bQuit = new QPushButton(this);
    bQuit->setText("Quit");

    sButtons = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

    lButtons = new QHBoxLayout();
    lButtons->addWidget(bStart, 0, Qt::AlignLeft);
    lButtons->addWidget(bStop, 0, Qt::AlignLeft);
    lButtons->addWidget(bSingleShot, 0, Qt::AlignLeft);
    lButtons->addItem(sButtons);
    lButtons->addWidget(bExport, 0, Qt::AlignRight);
    lButtons->addWidget(bQuit, 0, Qt::AlignRight);

    // ----------------------------------------
    // Camera Gain/Exposure/Color controls properties
    // ----------------------------------------
    // Boxes
    gbSlider = new QGroupBox("Exopsure time");
    gbGain = new QGroupBox("Gain (dB)");
    gbColor = new QGroupBox("Colors");

    // Controls
    sliderExposure = new QSlider(Qt::Horizontal, this);
    dsbGain = new QDoubleSpinBox(this);
    rbMonochrome = new QRadioButton("Gray Level", this);
    rbSatMonochrome = new QRadioButton("Gray Level, Red Saturated", this);
    rbSatColor = new QRadioButton("Color : Thermal", this);

    // Labels
    labelExposure = new QLabel("0,00 ms", this);

    // Layouts
    lColors = new QVBoxLayout();
    lColors->addWidget(rbMonochrome);
    lColors->addWidget(rbSatMonochrome);
    lColors->addWidget(rbSatColor);
    gbColor->setLayout(lColors);

    lGain = new QVBoxLayout();
    lGain->addWidget(dsbGain);
    gbGain->setLayout(lGain);

    lExposure = new QVBoxLayout();
    lExposure->addWidget(sliderExposure);
    lExposure->addWidget(labelExposure);
    gbSlider->setLayout(lExposure);

    lControls = new QHBoxLayout();
    lControls->addWidget(gbSlider);
    lControls->addWidget(gbGain);
    lControls->addWidget(gbColor);

    // ----------------------------------------
    // secondary display
    // ----------------------------------------
    xcutDisplay = new QGraphicsView(this);
    ycutDisplay = new QGraphicsView(this);

    // Group Boxes
    gbSecondaryDisplay = new QGroupBox("X/Y profiles and Analysis");

    // Check Boxes
    cbCentrage = new QCheckBox("Automatic centering", this);
    cbEnergy = new QCheckBox("Circle 86.5 %", this);

    // Label
    labelInfo = new QLabel("Info");

    lCheckBoxes = new QHBoxLayout();
    lCheckBoxes->addWidget(cbCentrage);
    lCheckBoxes->addWidget(cbEnergy);
    lSecondaryDisplay = new QVBoxLayout;
    lSecondaryDisplay->addLayout(lCheckBoxes);
    lSecondaryDisplay->addWidget(xcutDisplay);
    lSecondaryDisplay->addWidget(ycutDisplay);
    lSecondaryDisplay->addWidget(labelInfo);

    gbSecondaryDisplay->setLayout(lSecondaryDisplay);

    // ----------------------------------------
    // Main display
    // ----------------------------------------
    mainDisplay = new QGraphicsView(this);

    // ----------------------------------------
    // Central Widget set-up
    // ----------------------------------------
    mainLayout = new QGridLayout();
    mainLayout->addLayout(lControls, 1, 0);
    mainLayout->addLayout(lButtons, 1, 1);
    mainLayout->addWidget(mainDisplay, 0, 0);
    mainLayout->addWidget(gbSecondaryDisplay, 0, 1);

    window = new QWidget();
    window->setLayout(mainLayout);
    setCentralWidget(window);
}
