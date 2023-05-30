#include "mychart.h"
#include <QtCharts/QValueAxis>
#include "qgraphicslayout.h"

/*!
 * \brief MyChart::MyChart
 * \param QWidget *parent
 * \param QColor pen_Color
 * \param int bitDepth
 * 
 * constructor for custom graph
 * Need to take the color for the data/fit plots
 * and the bitDepth to compute the max value and set-up 
 * the Y-axis
 * 
 * 
 */
MyChart::MyChart(QWidget *parent, QColor pen_Color, int bitDepth)
    : QWidget{parent}
{
    maxY = int(pow(2, bitDepth)) - 1;
    penColor = pen_Color;
    setUpChart();
}

/*!
 * \brief MyChart::~MyChart
 * 
 * Destructor
 * 
 */
MyChart::~MyChart() {}

/*!
 * \brief MyChart::addDataPoint
 * \param std::vector<int> &datas
 * \param int xoffset
 * 
 * Add data to the courbeData scatters serie
 * 
 * update the x-axis by taking into account the xoffset
 * 
 */
void MyChart::addDataPoint(std::vector<int> &datas, int xoffset)
{
    courbeData->clear();
    for (int i = 0; i < datas.size(); ++i) {
        courbeData->append(QPointF(i - xoffset, datas[i]));
    }
    axisX->setRange(-xoffset, datas.size() - 1 - xoffset);
}

/*!
 * \brief MyChart::addFitPoint
 * \param std::vector<int> &datas
 * \param xoffset
 * 
 * Add data to the courbefir lineserie
 * 
 */
void MyChart::addFitPoint(std::vector<int> &datas, int xoffset)
{
    courbeFit->clear();
    for (int i = 0; i < datas.size(); ++i) {
        courbeFit->append(QPointF(i - xoffset, datas[i]));
    }
}

/*!
 * \brief MyChart::setUpChart
 * 
 * Set up the chart Widget with the wanted style !
 * 
 * create two series for fit and data
 * 
 */
void MyChart::setUpChart()
{
    courbeData = new QScatterSeries();
    courbeFit = new QLineSeries();

    // Courbe style for Data
    courbeData->setColor(penColor);
    courbeData->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    courbeData->setMarkerSize(3);
    courbeData->setUseOpenGL(true);

    // Courbe style for Fit
    QPen pen(penColor);
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);
    courbeFit->setPen(pen);

    // Creat new chart with two series (Data & Fit)
    graphe = new QChart();
    graphe->addSeries(courbeData);
    graphe->addSeries(courbeFit);

    // No legend
    graphe->legend()->hide();

    // Axes Styles !
    QPen minorGrid = QPen(QColor(200, 200, 200));
    minorGrid.setStyle(Qt::DashLine);
    QPen majorGrid = QPen(QColor(175, 175, 175));
    axisX = new QValueAxis();
    axisX->setRange(0, 1);
    axisX->setLabelFormat("%i");
    axisX->setTickCount(5);
    axisX->setGridLineVisible(true);
    axisX->setMinorTickCount(1);
    axisX->setMinorGridLineVisible(true);
    axisX->setGridLinePen(majorGrid);
    axisX->setMinorGridLinePen(minorGrid);
    graphe->addAxis(axisX, Qt::AlignBottom);
    courbeFit->attachAxis(axisX);
    courbeData->attachAxis(axisX);

    axisY = new QValueAxis();
    axisY->setRange(0, maxY);
    axisY->setLabelFormat("%i");
    axisY->setTickCount(5);
    axisY->setGridLineVisible(true);
    axisY->setMinorTickCount(1);
    axisY->setMinorGridLineVisible(true);
    axisY->setGridLinePen(majorGrid);
    axisY->setMinorGridLinePen(minorGrid);
    graphe->addAxis(axisY, Qt::AlignLeft);
    courbeData->attachAxis(axisY);
    courbeFit->attachAxis(axisY);

    // Remove padding
    graphe->setMargins(QMargins(0, 0, 0, 0));

    // Remove roundness
    //graphe->setBackgroundRoundness(0);

    // Remove margin
    graphe->layout()->setContentsMargins(0, 0, 0, 0);

    // ChartView Widget (antialiasing renderer)
    graphique = new QChartView(graphe);
    graphique->setRenderHint(QPainter::Antialiasing);

    // Add to a layout otherwise it is hidden
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(graphique);
    layout->setContentsMargins(0, 0, 0, 0);

    //All margins are remove !
    // Spacing control is now done with the layout in mainwindow !
}

/*!
 * \brief MyChart::setMaxY
 * \param int newMaxY
 * 
 * update the maxY value
 * 
 */
void MyChart::setMaxY(int newMaxY)
{
    maxY = newMaxY;
    axisY->setRange(0, maxY);
}
