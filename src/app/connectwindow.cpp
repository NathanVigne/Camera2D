#include "connectwindow.h"
#include <iostream>

/*!
    \fn ConnectWindow::ConnectWindow(QWidget *parent) : QWidget{parent}
    
    Constructor for the connect Window, hérite de QWidget.
    Set-up the widgets layout and connect relevent signals !
*/
ConnectWindow::ConnectWindow(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(200, 150);
    setWindowTitle(" ");
    setWindowIcon(QIcon(":/icon/cam_ico.png"));

    boutton_connect = new QPushButton("Connect", this);
    boutton_connect->setDisabled(true);

    boutton_refresh = new QPushButton("Refresh", this);

    h_layout = new QHBoxLayout();
    h_layout->addWidget(boutton_connect);
    h_layout->addWidget(boutton_refresh);

    cam_name_display = new QComboBox(this);

    label = new QLabel("Choisir une caméra", this);
    label->setAlignment(Qt::AlignCenter);

    v_layout = new QVBoxLayout(this);
    v_layout->addWidget(label);
    v_layout->addWidget(cam_name_display);
    v_layout->addLayout(h_layout);
    v_layout->addSpacing(30);

    connect(boutton_connect, SIGNAL(clicked()), this, SLOT(slot_connectClicked()));
    connect(boutton_refresh, SIGNAL(clicked()), this, SLOT(slot_refrechClicked()));

    std::clog << "ConnecWindow :: Constructor. Thread : " << QThread::currentThreadId()
              << std::endl;
}

/*!
    \fn ConnectWindow::~ConnectWindow() {}

    Destructor of the connect Window.
    Nothing to do !
*/
ConnectWindow::~ConnectWindow() {}

/*!
    \fn void void ConnectWindow::refresh()

    public methods use to manually force a refresh on scan camera !

*/
void ConnectWindow::refresh()
{
    slot_refrechClicked();
}

/*!
    \fn void ConnectWindow::slot_cameraFound(CameraNameId *name_ids)

    public slot called when the camloder finished scannig for camera.
    It set the combo list with the camera names and allow the connect
    button to be clicked

*/
void ConnectWindow::slot_cameraFound(CamList *names_list)
{
    QStringList out;
    boutton_refresh->setDisabled(false);
    cam_name_display->clear();
    if (!names_list->name.empty()) {
        boutton_connect->setDisabled(false);
        label->setText("Connect to selected camera");

        for (size_t i = 0; i < names_list->name.size(); i++) {
            out.append(names_list->name[i].c_str());
        }
    } else {
        label->setText("No camera found !");
    }
    cam_name_display->insertItems(0, out);
}

/*!
    \fn void ConnectWindow::slot_cameraConnect()

    public slot called when the camloder succesfully connect with a camera.
    Pass the camera_handle along then close connect Window

*/
void ConnectWindow::slot_cameraConnect(ICamera *camera)
{
    emit signal_cameraConnected(camera);
    this->close();
}

/*!
    \fn void ConnectWindow::slot_failledCamConnect()

    public slot called when the camloder unsuccesfully connect with a camera.
    Display failled connection on Label

*/
void ConnectWindow::slot_failledCamConnect()
{
    label->setText("Failled Connection");
}

/*!
    \fn void ConnectWindow::slot_connectClicked()

    private slot called when we clik the connect button.
    It send the combo Id of the selected camera to the cam loader
    with the signal cameraConnect(Qstring, int)
*/
void ConnectWindow::slot_connectClicked()
{
    int id = cam_name_display->currentIndex();
    emit signal_Connect(id);
    std::clog << "ConnecWindow :: ConnectClicked. Thread : " << QThread::currentThreadId()
              << std::endl;
}

/*!
    \fn void ConnectWindow::slot_refrechClicked()

    private slot called when we clik on the refresh button.
    It send a signal to the CamLoader for scanning camera 
*/
void ConnectWindow::slot_refrechClicked()
{
    emit signal_Refresh();
    boutton_refresh->setDisabled(true);
    boutton_connect->setDisabled(true);
    label->setText("Searching for camera ...");
}
