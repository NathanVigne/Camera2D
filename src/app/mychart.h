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
    explicit MyChart(QWidget *parent = nullptr, QColor pen_Color = Qt::black, int bitepth = 8);
    ~MyChart();

    void setMaxY(int newMaxY);
    void myUpdate(int &mem_offset, int &size, int &XY, int &axe_offset, std::mutex *mutex_);
    void setMem(MemoryManager *newMem);

private:
    void setUpChart();
    void getDatas(int &offset, int &size, int &XY, int &axe_offset, std::mutex *mutex_);
    void addDataPoint(int offset);
    void addFitPoint(int offset);

private:
    // Creating the chart Object !!
    QChartView *graphique;      // un widget pour afficher un graphe
    QChart *graphe;             // la représentation d'un graphe
    QScatterSeries *courbeData; // les données
    QLineSeries *courbeFit;     // les données
    QValueAxis *axisX;          // Axis
    QValueAxis *axisY;
    MemoryManager *m_mem;

    QColor penColor = Qt::black;
    int maxY;

    QList<QPointF> data;
    QList<QPointF> fit;
};

#endif // MYCHART_H
