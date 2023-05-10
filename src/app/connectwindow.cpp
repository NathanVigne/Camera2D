#include "connectwindow.h"
#include <QDebug>
#include <QEvent>
#include <thorlabs.h>

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

    connect(boutton_connect, SIGNAL(clicked()), this, SLOT(connectClicked()));
    connect(boutton_refresh, SIGNAL(clicked()), this, SLOT(refrechClicked()));

    refrechClicked();
}

/*!
    \fn ConnectWindow::~ConnectWindow() {}

    Destructor of the connect Window.
*/
ConnectWindow::~ConnectWindow() {}

/*!
    \fn void ConnectWindow::cameraFound(QStringList *cam_name)

    public slot called when the cameraloder found some camera. It set the combo
    list with the camera names and allow the connect buttonto be clicked
*/
void ConnectWindow::cameraFound(CameraNameId *name_ids)
{
    cam_name_display->clear();
    cam_name_display->insertItems(0, name_ids->camera_Name);
    if (!name_ids->camera_Name.at(0).isEmpty()) {
        boutton_connect->setDisabled(false);
    }
}

/*!
    \fn void ConnectWindow::cameraSelect()

    public slot called when we clike the connect button. It send the combo
    the name and Id of the selected camera with the signal cameraConnect(Qstring, int)

    TO DO : refactoriser with cam load
*/
void ConnectWindow::connectClicked()
{
    int id = cam_name_display->currentIndex();
    QString str = cam_name_display->itemText(id);
    emit cameraConnect(str, id);
    this->close();
}

/*!
    \fn void ConnectWindow::refrechClicked()

    public slot called when we clik the refresh button. It re-launch the
    thor_searchCamera() function

    TO DO : refactoriser with cam load
    TO DO : move to thread or something (maybe camloader on it's own thread)
            to keeping ui responsive (or add loading bar ?)
*/
void ConnectWindow::refrechClicked()
{
    // TO DO : Done with the CamLoader implementation !
    name_id = thor_searchCamera();
    cameraFound(&name_id);
}
