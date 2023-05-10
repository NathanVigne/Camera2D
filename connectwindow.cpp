#include "connectwindow.h"
#include <QDebug>
#include <QEvent>
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

    cam_name_display = new QComboBox(this);

    label = new QLabel("Choisir une caméra", this);
    label->setAlignment(Qt::AlignCenter);

    layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(cam_name_display);
    layout->addWidget(boutton_connect);
    layout->addSpacing(30);

    // Place holder for the signal from the camLoader !
    QStringList strList = {"Arial", "Helvetica", "Times"};
    cameraFound(&strList);

    connect(boutton_connect, SIGNAL(clicked()), this, SLOT(cameraSelect()));
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
void ConnectWindow::cameraFound(QStringList *cam_name)
{
    cam_name_display->insertItems(0, *cam_name);
    boutton_connect->setDisabled(false);
}

/*!
    \fn void ConnectWindow::cameraSelect()

    public slot called when we clike the connect button. It send the combo
    the name and Id of the selected camera with the signal cameraConnect(Qstring, int)
*/
void ConnectWindow::cameraSelect()
{
    int id = cam_name_display->currentIndex();
    QString str = cam_name_display->itemText(id);
    emit cameraConnect(str, id);
    this->close();
}
