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
}

/*!
 * \brief exportWindow::~exportWindow
 * 
 * Destructor
 * 
 */
exportWindow::~exportWindow() {}

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
 * \brief exportWindow::setRed
 * \param float x (normalize value of the camera)
 * \param float norm_step (normalize camera intensity step)
 * \return float red value !
 */
float exportWindow::setRed(float x, float norm_step)
{
    return 4.0 * x - 2;
}

/*!
 * \brief exportWindow::setBlue
 * \param float x (normalize value of the camera)
 * \param float norm_step (normalize camera intensity step)
 * \return float blue value !
 */
float exportWindow::setBlue(float x, float norm_step)
{
    if (x < 0.375) {
        return 4.0 * x + 0.0;
    } else {
        if (x < (1 - 3 * norm_step)) {
            return -4.0 * x + 3.0;
        } else {
            return 1 / (3 * norm_step) * x - 1;
        }
    }
}

/*!
 * \brief exportWindow::setGreen
 * \param float x (normalize value of the camera)
 * \param float norm_step (normalize camera intensity step)
 * \return float green value !
 */
float exportWindow::setGreen(float x, float norm_step)
{
    if (x < 0.5) {
        return 4.0 * x - 1.0;
    } else {
        if (x < (1 - 3 * norm_step)) {
            return -4.0 * x + 4.0;
        } else {
            return 1 / (3 * norm_step) * x - 1;
        }
    }
}

/*!
 * \brief exportWindow::colorMap
 * \param float x (normalize value of the camera)
 * \param float norm_step (normalize camera intensity step)
 * \return Qcolor reprensting the heat colormap!
 */
QColor exportWindow::colorMap(float x, float norm_step)
{
    float r = setRed(x, norm_step);
    if (r > 1.0) {
        r = 1.0;
    }
    if (r < 0.0) {
        r = 0.0;
    }
    float g = setGreen(x, norm_step);
    if (g > 1.0) {
        g = 1.0;
    }
    if (g < 0.0) {
        g = 0.0;
    }
    float b = setBlue(x, norm_step);
    if (b > 1.0) {
        b = 1.0;
    }
    if (b < 0.0) {
        b = 0.0;
    }
    QColor col;
    col.setRgbF(r, g, b);
    return col;
}

/*!
 * \brief exportWindow::saveFiles
 * 
 * Private slot called whand Ok button pressed
 * It launch the file save window then save all selected files
 * also closes the window !
 * 
 */
void exportWindow::saveFiles()
{
    std::scoped_lock lock{*m_mutex};

    // Open savefile window
    QString directory = QFileDialog::getSaveFileName(this, tr("Save Files"), "", tr(""));
    if (!directory.isEmpty()) {
        QDir dir(directory);
        if (!dir.exists()) {
            if (!dir.mkpath(directory)) {
                QMessageBox::critical(nullptr, "Error", "Failed to create the directory.");
                return;
            }
        }
        QFileInfo fi(directory);

        // Check wich files to save !
        if (cbTXT->isChecked()) {
            // get buffer size
            int h = cam->getSensorHeigth();
            int w = cam->getSensorWidth();

            QString filePath = directory + "/" + fi.baseName() + "-2D.txt";
            // Perform file-saving operations
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                for (int i = 0; i < h; ++i) {
                    for (int j = 0; j < w; ++j) {
                        stream << static_cast<int>(cam->temp_image_buffer[i * w + j]) << "\t";
                    }
                    stream << "\n"; // Start a new line for each buffer width
                }
                file.close();
            } else {
                QMessageBox::critical(nullptr, "Error", "Failed to save all files.");
                return;
            }
        }

        if (cbX->isChecked()) {
            // TO DO
        }

        if (cbY->isChecked()) {
            // TO DO
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
                    float x = float(cam->temp_image_buffer[i * w + j]) / float(maxVal);
                    //x = float((i * h + j) % maxVal) / float(maxVal);
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
                        col = colorMap(x, norm_step);
                        break;
                    };
                    image.setPixelColor(j, i, col);
                }
            }
            if (image.save(filePath, "PNG")) {
            } else {
                QMessageBox::critical(nullptr, "Error", "Failed to save all files.");
                return;
            }
        }

        QMessageBox::information(nullptr, "Success", "All File saved successfully.");

    } else {
        QMessageBox::information(nullptr, "Information", "No name entered.");
    }

    // close window
    if (isRunning) {
        cam->Start();
    }
    close();
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
