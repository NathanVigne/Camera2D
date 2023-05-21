#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/icon/cam_ico.png"));
    uiSetUp();

    // Connect Slot to cam !
    // TO DO caonnection with Color and chack boxes !
    // PushButtons connection
    connect(bStart, &QPushButton::clicked, this, &MainWindow::slot_Start);
    connect(bStop, &QPushButton::clicked, this, &MainWindow::slot_Stop);
    connect(bSingleShot, &QPushButton::clicked, this, &MainWindow::slot_SingleShot);
    connect(bExport, &QPushButton::clicked, this, &MainWindow::slot_Export);
    connect(bQuit, &QPushButton::clicked, this, &MainWindow::slot_Quit);
    connect(bResetZoom, &QPushButton::clicked, this, &MainWindow::slot_ResetZoom);

    // Gain connection
    connect(dsbGain, &QDoubleSpinBox::valueChanged, this, &MainWindow::slot_Gain);

    // Exposure connection
    connect(sliderExposure, &QSlider::valueChanged, this, &MainWindow::slot_Exposure);

    //Color connection
    connect(rbMonochrome, &QRadioButton::toggled, this, &MainWindow::slot_Color);
    connect(rbSatMonochrome, &QRadioButton::toggled, this, &MainWindow::slot_Color);
    connect(rbSatColor, &QRadioButton::toggled, this, &MainWindow::slot_Color);
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::callBackDraw()
{
    mainDisplay->setTexture(cam->temp_image_buffer, &m_mutex);
    mainDisplay->update();
}

void MainWindow::slot_CameraOpen(ICamera *camera, CAMERATYPE type)
{
    cam = camera;
    cam_type = type;
    std::clog << "ICamera pointer : " << cam << std::endl;

    dsbGain->setMaximum(cam->getMaxGain());
    dsbGain->setMinimum(cam->getMinGain());
    dsbGain->setValue(cam->getMinGain());
    cam->SetGain(cam->getMinGain());

    // in µs
    int min, max;
    if (cam_type == CAMERATYPE::THORLABS) {
        min = int(double(log10(cam->getMinExposure()) * 1000.0));
        max = int(double(log10(cam->getMaxExposure()) * 1000.0));
    } else {
        min = (int) cam->getMinExposure();
        max = (int) cam->getMaxExposure();
    }

    sliderExposure->setMinimum(min);
    sliderExposure->setMaximum(max);
    sliderExposure->setValue((int) cam->getMinExposure());
    cam->SetExposure(cam->getMinExposure());
    cam->m_mutex = &m_mutex;

    connect(cam, &ICamera::frameReady, this, &MainWindow::callBackDraw);
    mainDisplay->texHeigth = cam->getSensorHeigth();
    mainDisplay->texWidth = cam->getSensorWidth();
    this->show();
}

void MainWindow::slot_Start()
{
    std::clog << "Start Clicked" << std::endl;
    cam->Start();
}

void MainWindow::slot_Stop()
{
    std::clog << "Stop Clicked" << std::endl;
    cam->Stop();
}

void MainWindow::slot_SingleShot()
{
    std::clog << "Single Shot Clicked" << std::endl;
    cam->SingleShot();
}

void MainWindow::slot_Export()
{
    std::clog << "Export Clicked" << std::endl;
}

void MainWindow::slot_Quit()
{
    std::clog << "Quit Clicked" << std::endl;
    close();
}

void MainWindow::slot_ResetZoom()
{
    std::clog << "ResetZoom Clicked" << std::endl;
    mainDisplay->resetZoom();
}

void MainWindow::slot_Gain(double newGain)
{
    std::clog << "New Gain : " << newGain << std::endl;
    cam->SetGain(newGain);
}

void MainWindow::slot_Exposure(int newExposure)
{
    float value;
    if (cam_type == CAMERATYPE::THORLABS) {
        value = std::pow(10, float(newExposure) / 1000.0);
    } else {
        value = float(newExposure);
    }

    cam->SetExposure((long long) value);

    std::clog << "New Exposure : " << value << std::endl;
    long long camVal = cam->GetExposure();
    std::clog << "New Exposure : " << value << "Cam Exposure : " << camVal << std::endl;

    QString str = QStringLiteral("%1 ms").arg((float) camVal / 1000.0);
    labelExposure->setText(str);
}

void MainWindow::slot_Color(bool check)
{
    if (rbMonochrome->isChecked()) {
        std::clog << "ColorMonochrome" << std::endl;
    } else if (rbSatMonochrome->isChecked()) {
        std::clog << "ColorMonochrome Sat" << std::endl;
    } else if (rbSatColor->isChecked()) {
        std::clog << "Color Color" << std::endl;
    }
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
    rbMonochrome->setChecked(true);

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

    // Check Boxes + Pusbbuton
    cbCentrage = new QCheckBox("Automatic centering", this);
    cbEnergy = new QCheckBox("Circle 86.5 %", this);
    bResetZoom = new QPushButton(this);
    bResetZoom->setText("Reset Zoom");

    // Label
    labelInfo = new QLabel("Info");

    lCheckBoxes = new QHBoxLayout();
    lCheckBoxes->addWidget(cbCentrage);
    lCheckBoxes->addWidget(cbEnergy);
    lCheckBoxes->addWidget(bResetZoom);
    lSecondaryDisplay = new QVBoxLayout;
    lSecondaryDisplay->addLayout(lCheckBoxes);
    lSecondaryDisplay->addWidget(xcutDisplay);
    lSecondaryDisplay->addWidget(ycutDisplay);
    lSecondaryDisplay->addWidget(labelInfo);

    gbSecondaryDisplay->setLayout(lSecondaryDisplay);

    // ----------------------------------------
    // Main display
    // ----------------------------------------
    mainDisplay = new GLDisplay(this);

    // ----------------------------------------
    // Central Widget set-up
    // ----------------------------------------
    mainLayout = new QGridLayout();
    mainLayout->addLayout(lControls, 1, 0);
    mainLayout->addLayout(lButtons, 1, 1);
    mainLayout->addWidget(mainDisplay, 0, 0);
    mainLayout->addWidget(gbSecondaryDisplay, 0, 1);
    mainLayout->setColumnStretch(0, 3);
    mainLayout->setColumnStretch(1, 2);
    mainLayout->setColumnMinimumWidth(0, 600);
    mainLayout->setRowMinimumHeight(0, 400);
    mainLayout->setRowStretch(0, 1);
    mainLayout->setRowStretch(1, 0);

    window = new QWidget();
    window->setLayout(mainLayout);
    setCentralWidget(window);
}
