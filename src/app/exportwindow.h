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
#include "mychart.h"
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

    void setMem(MemoryManager *newMem);

    void setXCut(MyChart *newXCut);

    void setYCut(MyChart *newYCut);

private:
    void setUpUI();
    QColor colorMap(float x);
    bool checkCheckBox();

private slots:
    void saveFiles();

private:
    std::mutex *m_mutex;
    MemoryManager *m_mem;
    ICamera *cam;
    MyChart *xCut;
    MyChart *yCut;
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
