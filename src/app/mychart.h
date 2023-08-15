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

class MyChart : public QWidget
{
    Q_OBJECT
public:
    explicit MyChart(const int _axe_XY,
                     QColor pen_Color = Qt::black,
                     int bitDepth = 8,
                     QWidget *parent = nullptr);
    ~MyChart();

    double *getD_data() const;
    std::mutex *mutex_fit();
    QList<QPointF> *getDataList();
    double getFitParam(int id);

    void setSize(size_t newN);
    void setMem(MemoryManager *newMem);
    void setMaxY(int newMaxY);

    void myUpdate(int &mem_offset, std::mutex *mutex_);

signals:
    void receivedFit(std::mutex *mutex, double *params);
    void receivedData(double *datas);

public slots:
    void copyFitData(double *fit_data, double *fit_param, std::mutex *mutex);

private slots:
    void processFitData(std::mutex *mutex, double *params);

private:
    void setUpChart();
    void getDatas(int &offset, std::mutex *mutex_);
    void drawData();
    void drawFit();

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
    double *d_data;
    double *f_data;
    double *f_param;
    std::mutex m_mutex_fit;
};

#endif // MYCHART_H
