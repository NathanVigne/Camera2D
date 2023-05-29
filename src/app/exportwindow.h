#ifndef EXPORTWINDOW_H
#define EXPORTWINDOW_H

#include <QCheckBox>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include "gldisplay.h"
#include "icamera.h"
#include <mutex>

class exportWindow : public QWidget
{
    Q_OBJECT
public:
    explicit exportWindow(QWidget *parent = nullptr);
    ~exportWindow();

    // Setter
    void setMutex(std::mutex *newMutex);
    void setCam(ICamera *newCam);

    void setColChoice(ColorChoice newColChoice);

    void setIsRunning(bool newIsRunning);

private:
    void setUpUI();
    float setRed(float x, float norm_step);
    float setBlue(float x, float norm_step);
    float setGreen(float x, float norm_step);
    QColor colorMap(float x, float norm_step);
    bool checkCheckBox();

private slots:
    void saveFiles();

private:
    std::mutex *m_mutex;
    ICamera *cam;
    ColorChoice colChoice;
    bool isRunning = false;

    // Layout
    QVBoxLayout *layout;

    // Label
    QLabel *label;

    // CheckBoxes
    QCheckBox *cbPNG;
    QCheckBox *cbTXT;
    QCheckBox *cbX;
    QCheckBox *cbY;

    // PushButton
    QPushButton *buttonOK;

    QWidget *window;
};

#endif // EXPORTWINDOW_H
