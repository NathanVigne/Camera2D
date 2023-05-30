#ifndef MYCHART_H
#define MYCHART_H

#include <QGridLayout>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>

class MyChart : public QWidget
{
    Q_OBJECT
public:
    explicit MyChart(QWidget *parent = nullptr, QColor pen_Color = Qt::black, int bitepth = 8);
    ~MyChart();

    void addDataPoint(std::vector<int> &datas, int xoffset);
    void addFitPoint(std::vector<int> &datas, int xoffset);

    void setMaxY(int newMaxY);

private:
    void setUpChart();

private:
    // Creating the chart Object !!
    QChartView *graphique;      // un widget pour afficher un graphe
    QChart *graphe;             // la représentation d'un graphe
    QScatterSeries *courbeData; // les données
    QLineSeries *courbeFit;     // les données
    QValueAxis *axisX;          // Axis
    QValueAxis *axisY;

    QColor penColor = Qt::black;
    int maxY;
};

#endif // MYCHART_H
