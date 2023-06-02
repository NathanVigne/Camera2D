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

    void setButtonExport(QPushButton *newButtonExport);

private:
    void setUpUI();
    QColor colorMap(float x);
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
    QPushButton *buttonExport;

    QWidget *window;
};

#endif // EXPORTWINDOW_H
