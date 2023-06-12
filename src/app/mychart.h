#ifndef MYCHART_H
#define MYCHART_H

#include <QGridLayout>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include "memorymanager.h"
#include "workerfit.h"

class MyChart : public QWidget
{
    Q_OBJECT
public:
    explicit MyChart(const int _axe_XY,
                     QColor pen_Color = Qt::black,
                     int bitDepth = 8,
                     QWidget *parent = nullptr);
    ~MyChart();

    void setMaxY(int newMaxY);
    void myUpdate(int &mem_offset, int &axe_offset, std::mutex *mutex_);
    void setMem(MemoryManager *newMem);

    QList<QPointF> *getDataList();
    QList<QPointF> *getFitList();

    void setSize(size_t newN);
    void setUpWorker();

signals:
    void receivedFit();
    void receivedData(double *datas);
    void updateLabel(std::mutex *mutex, double *params);

private slots:
    void copyFitData(double *fit_data, double *fit_param);
    void processFitData();

private:
    void setUpChart();
    void myConnection();
    void getDatas(int &offset, int &axe_offset, std::mutex *mutex_);
    void drawData(int offset);
    void drawFit(int offset);

private:
    // General properties
    const int axe_XY;
    size_t N;
    const size_t N_fit = 201;
    const size_t P = 4;

    // Creating the chart Object !!
    QChartView *m_chartView;     // un widget pour afficher un graphe
    QChart *m_chart;             // la représentation d'un graphe
    QScatterSeries *m_serieData; // les données
    QLineSeries *m_serieFit;     // les données
    QValueAxis *axisX;          // Axis
    QValueAxis *axisY;
    QColor penColor;
    int maxY;

    // Memory manager
    MemoryManager *m_mem;

    // Data list for display (faster in QList)
    QList<QPointF> data;
    QList<QPointF> fit;

    // Data for fitting (faster in double)
    WorkerFit *work_fit;
    double *d_data;
    double *dummy_data;
    double *f_data;
    double *f_param;
    std::mutex m_mutex_fit;
    bool workerSet = false;
};

#endif // MYCHART_H
