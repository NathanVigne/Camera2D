#include "mainwindow.h"

/*!
 * \brief MainWindow::MainWindow
 * \param QWidget *parent
 * 
 * Mainwindow constructor. Setup the UI / Icon / Title ...
 * Manage the connection for all camera cotrols
 * 
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Instanciate the memory managment
    mem = new MemoryManager(&m_mutexSave, &m_mutexDisplay);
    msg = new QMessageBox();

    setWindowIcon(QIcon(":/icon/cam_ico.png"));
    uiSetUp();
    resize(1200, 600);

    // Connect Slot to cam !
    // PushButtons connection
    connect(bStart, &QPushButton::clicked, this, &MainWindow::slot_Start);
    connect(bStop, &QPushButton::clicked, this, &MainWindow::slot_Stop);
    connect(bSingleShot, &QPushButton::clicked, this, &MainWindow::slot_SingleShot);
    connect(bExport, &QPushButton::clicked, this, &MainWindow::slot_Export);
    connect(bQuit, &QPushButton::clicked, this, &MainWindow::slot_Quit);
    connect(bResetZoom, &QPushButton::clicked, this, &MainWindow::slot_ResetZoom);

    // Gain & Zoom connection
    connect(dsbGain, &QDoubleSpinBox::valueChanged, this, &MainWindow::slot_Gain);
    connect(dsbZoom, &QDoubleSpinBox::valueChanged, this, &MainWindow::slot_Zoom);

    // Exposure connection
    connect(sliderExposure, &QSlider::valueChanged, this, &MainWindow::slot_Exposure);

    //Color connection
    connect(rbMonochrome, &QRadioButton::clicked, this, &MainWindow::slot_Color);
    connect(rbSatMonochrome, &QRadioButton::clicked, this, &MainWindow::slot_Color);
    connect(rbSatColor, &QRadioButton::clicked, this, &MainWindow::slot_Color);

    connect(this, &MainWindow::frameReady, this, &MainWindow::callBackDraw, Qt::QueuedConnection);
    connect(this,
            &MainWindow::camDisconnect,
            this,
            &MainWindow::displayDisconnect,
            Qt::QueuedConnection);
    connect(this,
            &MainWindow::camReConnect,
            this,
            &MainWindow::displayReConnect,
            Qt::QueuedConnection);

    std::clog << "MainWindow :: Constructor. Thread : " << QThread::currentThreadId() << std::endl;
}

/*!
 * \brief MainWindow::~MainWindow
 * 
 * MainWindow destructor, nothing to do ! (Sure ??)
 * 
 */
MainWindow::~MainWindow()
{
    delete mem; // for safety since all buffer are manage here
    delete msg; // Not set-up in UiSetUp

    if (cam) {
        delete xFit;
        delete yFit;
    }
    std::clog << "Destroy MainWindow " << std::endl;
}

/*!
 * \brief MainWindow::callBackDraw
 * 
 * CallBack function called to draw on the GLDisplay !
 * 
 * And to get vector for 1D display
 * 
 * TO DO : move the 1D display to it's own thread or own
 * draw routine ?? (seems ok for now)
 * 
 */
void MainWindow::callBackDraw()
{
    mainDisplay->setTexture(mem->display(), &m_mutexDisplay);
    mainDisplay->update();

    // Get 1D cuts TO Test ???
    // Move to a memory manager ?
    int x = mainDisplay->getCroix().x();
    int y = mainDisplay->getCroix().y();
    xcutDisplay->myUpdate(y, &m_mutexDisplay);
    ycutDisplay->myUpdate(x, &m_mutexDisplay);

    // TO DO : with new fit
    // xFit->offsetUpdate(x);
    // yFit->offsetUpdate(y);
}

/*!
 * \brief MainWindow::slot_CameraOpen
 * \param ICamer *camera 
 * \param CAMERATYPE type
 * 
 * Public slot called when we connect to a camera in the connect window.
 * 
 * This function also set the camera control for the specified camera
 * 
 */
void MainWindow::slot_CameraOpen(ICamera *camera, CAMERATYPE type)
{
    // get came handle and type
    cam = camera;
    std::clog << "MainWindow :: CameraOpen. Thread : " << QThread::currentThreadId() << std::endl;
    cam_type = type;

    // set up & initiialize cam
    // Set cam mutex
    cam->setMutex(&m_mutexSave);
    // Set memory managment
    cam->setMemory(mem);
    // connect callback for drawing
    cam->setFrameReadyCallback([this]() { this->sendFrameReady(); });
    cam->setDisconnectCbck([this]() { this->sendDisconnect(); });
    cam->setConnectCbck([this]() { this->sendReConnect(); });
    cam->setWinHandle((HWND) this->winId());
    cam->Initialize();

    // initialize chart
    xcutDisplay->setMaxY(pow(2, cam->getBitDepth()));
    xcutDisplay->setMem(mem);
    xcutDisplay->setSize(cam->getSensorWidth());
    connect(xcutDisplay,
            &MyChart::receivedFit,
            this,
            &MainWindow::receivedLabelX,
            Qt::QueuedConnection);

    xFit = new Fit(cam->getSensorWidth(), 4, 201, 0, xcutDisplay);
    connect(xFit, &Fit::fitEND, xcutDisplay, &MyChart::copyFitData, Qt::QueuedConnection);

    ycutDisplay->setMaxY(pow(2, cam->getBitDepth()));
    ycutDisplay->setMem(mem);
    ycutDisplay->setSize(cam->getSensorHeigth());
    connect(ycutDisplay,
            &MyChart::receivedFit,
            this,
            &MainWindow::receivedLabelY,
            Qt::QueuedConnection);

    yFit = new Fit(cam->getSensorWidth(), 4, 201, 1, ycutDisplay);
    connect(yFit, &Fit::fitEND, ycutDisplay, &MyChart::copyFitData, Qt::QueuedConnection);

    // Inittialize UI and cam controls
    mainDisplay->setBit_depth(cam->getBitDepth());
    mainDisplay->setBuff_type(cam->getBuffType());

    // Set Gain UI
    dsbGain->setMaximum(cam->getMaxGain());
    dsbGain->setMinimum(cam->getMinGain());
    dsbGain->setValue(cam->getMinGain());
    cam->SetGain(cam->getMinGain());

    // Set exposure UI
    // in µs
    int min, max;
    //    if (cam_type == CAMERATYPE::THORLABS) {
    min = int(double(log10(cam->getMinExposure()) * 1000.0));
    max = int(double(log10(cam->getMaxExposure()) * 1000.0));
    //    } else {
    //        min = (int) cam->getMinExposure();
    //        max = (int) cam->getMaxExposure();
    //    }

    sliderExposure->setMinimum(min);
    sliderExposure->setMaximum(max);
    sliderExposure->setValue((int) cam->getMinExposure());
    cam->SetExposure(cam->getMinExposure());

    // Show display
    rbSatColor->setChecked(true);
    mainDisplay->setColorChoice(ColorChoice::COLOR_SAT);
    mainDisplay->setTexHeigth(cam->getSensorHeigth());
    mainDisplay->setTexWidth(cam->getSensorWidth());
    this->show();
}

/*!
 * \brief MainWindow::slot_Start
 * 
 * Public slot called when the start button is push. Call the start function
 * of the ICamera !
 * 
 */
void MainWindow::slot_Start()
{
    std::clog << "MW : Start Clicked" << std::endl;
    cam->Start();
    xFit->startLoop(100);
    yFit->startLoop(100);
    bStart->setDisabled(true);
    isRunning = true;
}

/*!
 * \brief MainWindow::slot_Stop
 * 
 * Public slot called when the stop button is push. Call the stop function
 * of the ICamera !
 */
void MainWindow::slot_Stop()
{
    std::clog << "MW : Stop Clicked" << std::endl;
    cam->Stop();
    xFit->stop();
    yFit->stop();
    bStart->setDisabled(false);
    isRunning = false;
}

/*!
 * \brief MainWindow::slot_SingleShot
 *
 * Public slot called when the SingleShot button is push. Call the SingleShot function
 * of the ICamera !
 * 
 */
void MainWindow::slot_SingleShot()
{
    std::clog << "MW : Single Shot Clicked" << std::endl;
    xFit->startsingle();
    yFit->startsingle();
    cam->SingleShot();
    bStart->setDisabled(false);
    isRunning = false;
}

/*!
 * \brief MainWindow::slot_Export
 * 
 * Public slot called when the Export button is push.
 *
 */
void MainWindow::slot_Export()
{
    std::clog << "MW : Export Clicked" << std::endl;
    exportWindow *exW = new exportWindow;
    exW->setButtonExport(bExport);
    exW->setIsRunning(isRunning);
    exW->setColChoice(mainDisplay->getColorChoice());
    exW->setCam(cam);
    exW->setXCut(xcutDisplay);
    exW->setYCut(ycutDisplay);
    exW->setMutex(&m_mutexDisplay);
    exW->setMem(mem);
    exW->show();
    cam->Stop();
}

/*!
 * \brief MainWindow::slot_Quit
 * 
 * Public slot called when the Quit button is push. Close the window.
 * If it is the last window then the application will quit and all cleanup
 * should be done by all classes.
 * 
 */
void MainWindow::slot_Quit()
{
    std::clog << "MW : Quit Clicked" << std::endl;
    close();
}

/*!
 * \brief MainWindow::slot_Zoom
 * \param double newZoom
 *
 * Public slot called when the zoom spin box is changed. Update the internal variable zoom
 *
 */
void MainWindow::slot_Zoom(double newZoom)
{
    zoom_ = newZoom;
}

/*!
 * \brief MainWindow::slot_ResetZoom
 * 
 * Public slot called whenthe reset zoom button is push. Call the reset zoom
 * function of the mainDisplay
 * 
 */
void MainWindow::slot_ResetZoom()
{
    std::clog << "MW : ResetZoom Clicked" << std::endl;
    mainDisplay->resetZoom();
}

/*!
 * \brief MainWindow::slot_Gain
 * \param double newGain
 * 
 * Public slot called when the gain in changed in the doublespinbox.
 * Set the newGain to the ICamera.
 * 
 */
void MainWindow::slot_Gain(double newGain)
{
    //std::clog << "New Gain : " << newGain << std::endl;
    cam->SetGain(newGain);
}

/*!
 * \brief MainWindow::slot_Exposure
 * \param int newExposure
 * 
 * Public slot called when the exposure is change on the horizontal slider.
 * Set the new exposure to the camera ! Also ask the camera what it's new 
 * exposure to display the value.
 * 
 */
void MainWindow::slot_Exposure(int newExposure)
{
    float value;
    value = std::pow(10, float(newExposure) / 1000.0);

    cam->SetExposure((long long) value);

    //std::clog << "New Exposure : " << value << std::endl;
    long long camVal = cam->GetExposure();
    //std::clog << "New Exposure : " << value << "Cam Exposure : " << camVal << std::endl;

    QString str = QStringLiteral("%1 ms").arg((float) camVal / 1000.0);
    labelExposure->setText(str);
}

/*!
 * \brief MainWindow::slot_Color
 * \param bool check
 * 
 * Public slot called when a radio button is toggled
 * 
 */
void MainWindow::slot_Color(bool check)
{
    if (rbMonochrome->isChecked()) {
        std::clog << "MW : ColorMonochrome" << std::endl;
        mainDisplay->setColorChoice(ColorChoice::MONO);
    } else if (rbSatMonochrome->isChecked()) {
        std::clog << "MW ! ColorMonochrome Sat" << std::endl;
        mainDisplay->setColorChoice(ColorChoice::MONO_SAT);
    } else if (rbSatColor->isChecked()) {
        std::clog << "MW : Color Color" << std::endl;
        mainDisplay->setColorChoice(ColorChoice::COLOR_SAT);
    }
}

/*!
 * \brief MainWindow::receivedLabelX
 * \param mutex
 * \param params
 * 
 * Slot called to update the fit label
 * Called when fit is finised
 * 
 */
void MainWindow::receivedLabelX(std::mutex *mutex, double *params)
{
    std::scoped_lock lock(*mutex);
    wx_d = params[2];
    wx_f = params[2] * cam->getPixelWidth() / zoom_;
    updateText();
}

void MainWindow::receivedLabelY(std::mutex *mutex, double *params)
{
    std::scoped_lock lock(*mutex);
    wy_d = params[2];
    wy_f = params[2] * cam->getPixelHeight() / zoom_;
    updateText();
}

/*!
 * \brief MainWindow::uiSetUp
 * 
 * function to set-up all the widget of the mainWindow
 * 
 */
void MainWindow::uiSetUp()
{
    // ----------------------------------------
    // Push Buttons Set Up
    // ----------------------------------------
    bStart = new QPushButton();
    bStart->setText("Start");
    bStart->setFixedHeight(30);

    bStop = new QPushButton();
    bStop->setText("Stop");
    bStop->setFixedHeight(30);

    bSingleShot = new QPushButton();
    bSingleShot->setText("Single shot");
    bSingleShot->setFixedHeight(30);

    bExport = new QPushButton();
    bExport->setText("Export");
    bExport->setFixedHeight(30);

    bQuit = new QPushButton();
    bQuit->setText("Quit");
    bQuit->setFixedHeight(30);

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
    sliderExposure = new QSlider(Qt::Horizontal);
    dsbGain = new QDoubleSpinBox();
    rbMonochrome = new QRadioButton("Gray Level");
    rbSatMonochrome = new QRadioButton("Gray Level, Red Saturated");
    rbSatColor = new QRadioButton("Color : Thermal");

    // Labels
    labelExposure = new QLabel("0,00 ms");

    // Layouts
    lColors = new QVBoxLayout();
    lColors->addWidget(rbMonochrome);
    lColors->addWidget(rbSatMonochrome);
    lColors->addWidget(rbSatColor);
    lColors->setSpacing(0);
    lColors->setContentsMargins(10, 0, 10, 0);
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
    //xcutDisplay = new QGraphicsView(this);
    //ycutDisplay = new QGraphicsView(this);
    xcutDisplay = new MyChart(0, QColor(0, 0, 255));
    ycutDisplay = new MyChart(1, QColor(255, 0, 0));

    // Group Boxes
    gbSecondaryDisplay = new QGroupBox("X/Y profiles and Analysis");

    // Check Boxes + Pusbbuton
    dsbZoom = new QDoubleSpinBox();
    dsbZoom->setRange(0, 1000);
    dsbZoom->setSingleStep(0.5);
    dsbZoom->setValue(zoom_);
    labelzoom = new QLabel("Zoom for Waist (µm) Fit");
    sZoom1 = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    sZoom2 = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    bResetZoom = new QPushButton();
    bResetZoom->setText("Reset Display Zoom");

    // Label
    labelInfo = new QLabel("Fit Information");
    QFont font = labelInfo->font();
    font.setPointSize(12);
    font.setBold(true);
    labelInfo->setFont(font);
    labelInfo->setMinimumHeight(80);
    labelInfo->setAlignment(Qt::AlignCenter);
    labelInfo->clear();
    labelInfo->setText(
        "\t\tX\t|\tY\nWaist (px) : \t xxx \t|\t xxx \nWaist (µm) : \t xxx \t|\t xxx");

    lCheckBoxes = new QHBoxLayout();
    lCheckBoxes->addItem(sZoom1);
    lCheckBoxes->addWidget(labelzoom);
    lCheckBoxes->addWidget(dsbZoom);
    lCheckBoxes->addItem(sZoom2);
    lCheckBoxes->addWidget(bResetZoom);
    lSecondaryDisplay = new QVBoxLayout();
    lSecondaryDisplay->addLayout(lCheckBoxes);
    lSecondaryDisplay->addWidget(xcutDisplay);
    lSecondaryDisplay->addWidget(ycutDisplay);
    lSecondaryDisplay->addWidget(labelInfo);
    lSecondaryDisplay->setContentsMargins(5, 0, 5, 0);

    gbSecondaryDisplay->setLayout(lSecondaryDisplay);

    // ----------------------------------------
    // Main display
    // ----------------------------------------
    mainDisplay = new GLDisplay();

    // ----------------------------------------
    // Right Widget set-up
    // ----------------------------------------
    rightLayout = new QVBoxLayout();
    rightLayout->addWidget(gbSecondaryDisplay);
    rightLayout->addLayout(lButtons);

    // ----------------------------------------
    // Central Widget set-up
    // ----------------------------------------
    mainLayout = new QGridLayout();
    mainLayout->addWidget(mainDisplay, 0, 0);
    mainLayout->addLayout(lControls, 1, 0);
    mainLayout->addLayout(rightLayout, 0, 1, 2, 1);
    mainLayout->setColumnStretch(0, 5);
    mainLayout->setColumnStretch(1, 4);
    mainLayout->setColumnMinimumWidth(0, 600);
    mainLayout->setRowMinimumHeight(0, 400);
    mainLayout->setRowStretch(0, 1);
    mainLayout->setRowStretch(1, 0);

    window = new QWidget();
    window->setLayout(mainLayout);
    setCentralWidget(window);
}

/*!
 * \brief MainWindow::sendFrameReady
 * 
 * slot colled when frame is ready
 * emit the frameready signal to draw frame
 * 
 */
void MainWindow::sendFrameReady()
{
    emit frameReady();
}

/*!
 * \brief MainWindow::sendDisconnect
 * 
 * slot called when cam is disconnected
 * emit camdisco signal
 * 
 */
void MainWindow::sendDisconnect()
{
    std::clog << "MainWindow :: cam disconnected" << std::endl;
    emit camDisconnect();
}

/*!
 * \brief MainWindow::sendReConnect
 * 
 * slot called when cam is reconnected
 * emit camReco signal
 * 
 */
void MainWindow::sendReConnect()
{
    std::clog << "MainWindow :: cam reconnected" << std::endl;
    emit camReConnect();
}

/*!
 * \brief MainWindow::displayDisconnect
 * 
 * slot called when cam is disconnected
 * open msgbox !
 * 
 */
void MainWindow::displayDisconnect()
{
    msg->setWindowIcon(QIcon(":/icon/cam_ico.png"));
    msg->setIcon(QMessageBox::Critical);
    msg->setText("Camera is disconnect !\n Please reconnect the camera.");
    msg->setWindowTitle("Error");
    msg->exec();
}

/*!
 * \brief MainWindow::displayReConnect
 * 
 * slot called when cam is reconnected
 * reload the cam handler & close msgbox ?
 * 
 */
void MainWindow::displayReConnect()
{
    std::clog << "MainWindow :: cam reconnected TO DO stuff ?" << std::endl;
    slot_Exposure(sliderExposure->value());
    slot_Gain(dsbGain->value());
    if (isRunning) {
        cam->Start();
    }
    msg->close();
}

/*!
 * \brief MainWindow::updateText
 * 
 * function to update text label from the fit data
 * 
 */
void MainWindow::updateText()
{
    char buffer[100];
    sprintf(buffer,
            "\t\tX\t|\tY\nWaist (px) : \t %.1f \t|\t %.1f \nWaist (µm) : \t %.1f \t|\t %.1f",
            wx_d,
            wy_d,
            wx_f,
            wy_f);
    labelInfo->setText(buffer);
}
