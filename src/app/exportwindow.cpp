#include "exportwindow.h"
/*!
 * \brief exportWindow::exportWindow
 * \param parent
 * 
 * Constructor of the export window. Set up the UI adn connect button
 * 
 */
exportWindow::exportWindow(QWidget *parent)
    : QWidget{parent}
{
    setUpUI();
    connect(buttonOK, &QPushButton::clicked, this, &exportWindow::saveFiles);
    setAttribute(Qt::WA_DeleteOnClose);
}

/*!
 * \brief exportWindow::~exportWindow
 * 
 * Destructor
 * 
 */
exportWindow::~exportWindow()
{
    std::clog << "Destroy Export Window" << std::endl;
    buttonExport->setDisabled(false);
}

/*!
 * \brief exportWindow::setUpUI
 * 
 * Set up the UI for export
 * 
 */
void exportWindow::setUpUI()
{
    // ----------------------------------------
    // Push Buttons Set Up
    // ----------------------------------------
    buttonOK = new QPushButton(this);
    buttonOK->setText("OK");

    // ----------------------------------------
    // CheckBoxes
    // ----------------------------------------
    cbTXT = new QCheckBox("Text file (2D)", this);
    cbTXT->setCheckState(Qt::Checked);
    cbX = new QCheckBox("Text file (X-cut)", this);
    cbY = new QCheckBox("Text file (Y-cut)", this);
    cbPNG = new QCheckBox("PNG image", this);

    // ----------------------------------------
    // Label
    // ----------------------------------------
    label = new QLabel("A new folder containing all file will be created !", this);

    // Layouts
    layout = new QVBoxLayout(this);
    layout->addSpacing(10);
    layout->addWidget(label);
    layout->addSpacing(30);
    layout->addWidget(cbTXT);
    layout->addWidget(cbX);
    layout->addWidget(cbY);
    layout->addWidget(cbPNG);
    layout->addSpacing(30);
    layout->addWidget(buttonOK);
    layout->addSpacing(10);
    layout->setAlignment(Qt::AlignCenter);

    //setFixedSize(300, 220);

    setWindowTitle("Export selection");
    setWindowIcon(QIcon(":/icon/cam_ico.png"));
}

/*!
 * \brief exportWindow::colorMap
 * \param float x (normalize value of the camera)
 * \return Qcolor reprensting the heat colormap!
 */
QColor exportWindow::colorMap(float x)
{
    float step = 0.125;
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    QColor col;

    if (x <= step) {
        r = (47.0 / 255.0) / step * x;
        g = (50.0 / 255.0) / step * x;
        b = (140.0 / 255.0) / step * x;
        col.setRgbF(r, g, b);
        return col;
    }

    if (x <= 2 * step) {
        r = -(47.0 / 255.0) / step * x + (94.0 / 255.0);
        g = (146.0 / 255.0) / step * x - (96.0 / 255.0);
        b = (115.0 / 255.0) / step * x + (25.0 / 255.0);
        col.setRgbF(r, g, b);
        return col;
    }

    if (x <= 3 * step) {
        r = (90.0 / 255.0) / step * x - (180.0 / 255.0);
        g = -(6.0 / 255.0) / step * x + (208.0 / 255.0);
        b = -(221.0 / 255.0) / step * x + (697.0 / 255.0);
        col.setRgbF(r, g, b);
        return col;
    }

    if (x <= 4 * step) {
        r = (165.0 / 255.0) / step * x - (405.0 / 255.0);
        g = (35.0 / 255.0) / step * x + (85.0 / 255.0);
        b = -(34.0 / 255.0) / step * x + (136.0 / 255.0);
        col.setRgbF(r, g, b);
        return col;
    }

    if (x <= 5 * step) {
        r = -(15.0 / 255.0) / step * x + (315.0 / 255.0);
        g = -(85.0 / 255.0) / step * x + (565.0 / 255.0);
        b = (11.0 / 255.0) / step * x - (44.0 / 255.0);
        col.setRgbF(r, g, b);
        return col;
    }

    if (x <= 6 * step) {
        r = +(15.0 / 255.0) / step * x + (165.0 / 255.0);
        g = -(140.0 / 255.0) / step * x + (840.0 / 255.0);
        b = -(11.0 / 255.0) / step * x + (66.0 / 255.0);
        col.setRgbF(r, g, b);
        return col;
    }

    if (x <= 7 * step) {
        r = -(19.0 / 255.0) / step * x + (369.0 / 255.0);
        g = (19.0 / 255.0) / step * x - (114.0 / 255.0);
        b = (142.0 / 255.0) / step * x - (852.0 / 255.0);
        col.setRgbF(r, g, b);
        return col;
    } else {
        r = (19.0 / 255.0) / step * x + (103.0 / 255.0);
        g = (236.0 / 255.0) / step * x - (1633.0 / 255.0);
        b = (113.0 / 255.0) / step * x - (649.0 / 255.0);
        col.setRgbF(r, g, b);
        return col;
    }
}

/*!
 * \brief exportWindow::checkCheckBox
 * \return bool
 * 
 * retrun true is one or more boxes are check otherwise return false
 * 
 */
bool exportWindow::checkCheckBox()
{
    return cbTXT->isChecked() | cbX->isChecked() | cbY->isChecked() | cbPNG->isChecked();
}

/*!
 * \brief exportWindow::saveFiles
 * 
 * Private slot called when Ok button pressed
 * It launch the file save window then save all selected files
 * also closes the window !
 * 
 * the save routines are dependant on buffer types
 * 
 */
void exportWindow::saveFiles()
{
    std::scoped_lock lock{*m_mutex};
    QMessageBox msgBox;
    msgBox.setWindowIcon(QIcon(":/icon/cam_ico.png"));
    int h = 0;
    int w = 0;

    if (!checkCheckBox()) {
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("No file selected");
        msgBox.setWindowTitle("Error");
        msgBox.exec();
        return;
    }

    // Open savefile window
    QString directory = QFileDialog::getSaveFileName(this, tr("Save Files"), "", tr(""));
    if (!directory.isEmpty()) {
        QDir dir(directory);
        if (!dir.exists()) {
            if (!dir.mkpath(directory)) {
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setText("Failed to create the directory.");
                msgBox.setWindowTitle("Error");
                msgBox.exec();
                return;
            }
        }
        QFileInfo fi(directory);

        // Check wich files to save !
        if (cbTXT->isChecked()) {
            // get buffer size
            h = cam->getSensorHeigth();
            w = cam->getSensorWidth();

            QString filePath = directory + "/" + fi.baseName() + "-2D.txt";
            // Perform file-saving operations
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                for (int i = 0; i < h; ++i) {
                    for (int j = 0; j < w; ++j) {
                        switch (m_mem->type()) {
                        case U8:
                            stream << static_cast<int>(
                                static_cast<unsigned char *>(m_mem->display())[i * w + j])
                                   << "\t";
                            break;
                        case U16:
                            stream << static_cast<int>(
                                static_cast<unsigned short *>(m_mem->display())[i * w + j])
                                   << "\t";
                            break;
                        case BUFF_END:
                            break;
                        }
                    }
                    stream << "\n"; // Start a new line for each buffer width
                }
                file.close();
            } else {
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setText("Failed to save all files.");
                msgBox.setWindowTitle("Error");
                msgBox.exec();
                return;
            }
        }

        if (cbX->isChecked()) {
            // get buffer size
            h = cam->getSensorHeigth();
            w = cam->getSensorWidth();

            QString filePath = directory + "/" + fi.baseName() + "-X.txt";
            // Perform file-saving operations
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                stream << "X"
                       << "\t"
                       << "Y"
                       << "\n";
                for (int i = 0; i < w; ++i) {
                    stream << xCut->getDataList()->at(i).x() << "\t"
                           << xCut->getDataList()->at(i).y() << "\n"; // Start a new lineh
                }
                file.close();
            } else {
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setText("Failed to save all files.");
                msgBox.setWindowTitle("Error");
                msgBox.exec();
                return;
            }
        }

        if (cbY->isChecked()) {
            // get buffer size
            h = cam->getSensorHeigth();
            w = cam->getSensorWidth();

            QString filePath = directory + "/" + fi.baseName() + "-Y.txt";
            // Perform file-saving operations
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                stream << "X"
                       << "\t"
                       << "Y"
                       << "\n";
                for (int i = 0; i < h; ++i) {
                    stream << yCut->getDataList()->at(i).x() << "\t"
                           << yCut->getDataList()->at(i).y() << "\n"; // Start a new lineh
                }
                file.close();
            } else {
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setText("Failed to save all files.");
                msgBox.setWindowTitle("Error");
                msgBox.exec();
                return;
            }
        }

        if (cbPNG->isChecked()) {
            // get buffer size
            int h = cam->getSensorHeigth();
            int w = cam->getSensorWidth();
            int maxVal = pow(2, cam->getBitDepth()) - 1;

            float x = 0;
            float norm_step = 1 / float(maxVal);
            QImage image(w, h, QImage::Format_RGB16);
            QString filePath = directory + "/" + fi.baseName() + ".png";
            QColor col;
            // Perform file-saving operations
            for (int i = 0; i < h; ++i) {
                for (int j = 0; j < w; ++j) {
                    switch (m_mem->type()) {
                    case U8:
                        x = float(static_cast<unsigned char *>(m_mem->display())[i * w + j])
                            / float(maxVal);
                        break;
                    case U16:
                        x = float(static_cast<unsigned short *>(m_mem->display())[i * w + j])
                            / float(maxVal);
                        break;
                    case BUFF_END:
                        x = 0;
                        break;
                    }
                    switch (colChoice) {
                    case MONO:
                        col.setRgbF(x, x, x);
                        break;
                    case MONO_SAT:
                        if (x >= (1 - 1 * norm_step)) {
                            col.setRgbF(1.0, 0.0, 0.0);
                        } else {
                            col.setRgbF(x, x, x);
                        }
                        break;
                    case COLOR_SAT:
                        col = colorMap(x);
                        break;
                    };
                    image.setPixelColor(j, i, col);
                }
            }
            if (image.save(filePath, "PNG")) {
            } else {
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setText("Failed to save all files.");
                msgBox.setWindowTitle("Error");
                msgBox.exec();
                return;
            }
        }

        //        msgBox.setIcon(QMessageBox::Information);
        //        msgBox.setText("All File saved successfully.");
        //        msgBox.setWindowTitle("Success");
        //        msgBox.exec();

    } else {
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("No name entered.");
        msgBox.setWindowTitle("Error");
        msgBox.exec();
    }

    // close window
    if (isRunning) {
        cam->Start();
    }
    close();
}

/*!
 * \brief exportWindow::setYCut
 * \param newYCut
 * 
 * Setter
 * 
 */
void exportWindow::setYCut(MyChart *newYCut)
{
    yCut = newYCut;
}

/*!
 * \brief exportWindow::setXCut
 * \param newXCut
 * 
 * setter
 * 
 */
void exportWindow::setXCut(MyChart *newXCut)
{
    xCut = newXCut;
}

/*!
 * \brief exportWindow::setMem
 * \param newMem
 * 
 * setter
 * 
 */
void exportWindow::setMem(MemoryManager *newMem)
{
    m_mem = newMem;
}

/*!
 * \brief exportWindow::setButtonExport
 * \param newButtonExport
 * 
 * Setter for controlling button export
 * Disable it the time the export window is open
 * 
 */
void exportWindow::setButtonExport(QPushButton *newButtonExport)
{
    buttonExport = newButtonExport;
    buttonExport->setDisabled(true);
}

/*!
 * \brief exportWindow::setIsRunning
 * \param newIsRunning
 * 
 * Setter
 * 
 */
void exportWindow::setIsRunning(bool newIsRunning)
{
    isRunning = newIsRunning;
}

/*!
 * \brief exportWindow::setColChoice
 * \param newColChoice
 * 
 * setter for the current colorchoice
 * 
 */
void exportWindow::setColChoice(ColorChoice newColChoice)
{
    colChoice = newColChoice;
}

/*!
 * \brief exportWindow::setCam
 * \param newCam
 * 
 * Setter for the camera handle
 * 
 */
void exportWindow::setCam(ICamera *newCam)
{
    cam = newCam;
}

/*!
 * \brief exportWindow::setMutex
 * \param newMutex
 * 
 * Setter for the mutex
 * 
 */
void exportWindow::setMutex(std::mutex *newMutex)
{
    m_mutex = newMutex;
}
