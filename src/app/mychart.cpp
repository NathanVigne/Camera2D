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
 * \param int offset
 * 
 * Add data to the courbeData scatters serie
 * 
 * update the x-axis by taking into account the xoffset
 * 
 */
void MyChart::addDataPoint(int offset)
{
    courbeData->replace(data);
    axisX->setRange(-offset, data.size() - 1 - offset);
}

/*!
 * \brief MyChart::addFitPoint
 * \param offset
 * 
 * Add data to the courbefir lineserie
 * 
 * TO DO : compute fit
 * 
 */
void MyChart::addFitPoint(int offset)
{
    courbeFit->replace(fit);
}

/*!
 * \brief MyChart::setMem
 * \param newMem
 * 
 * setter for the memory manager to use
 * 
 */
void MyChart::setMem(MemoryManager *newMem)
{
    m_mem = newMem;
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
    courbeData->setMarkerSize(1.5);
    courbeData->setUseOpenGL(true);

    // Courbe style for Fit
    QPen pen(penColor);
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);    courbeFit->setPen(pen);

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
 * \brief MyChart::getDatas
 * \param int &offset
 * \param int &size
 * \param int &XY (0 for X graphh, 1 for Y grpah)
 * \param int &axe_offset
 * \param std::mutex &mutex_
 * 
 * Load data from the display buffer onto the QList<QPointF> private property
 * 
 * TO DO on other thread ??? maybe not since double buffer prevent a lot
 * of memory conflict !
 * 
 */
void MyChart::getDatas(int &offset, int &size, int &XY, int &axe_offset, std::mutex *mutex_)
{
    std::scoped_lock locker{*mutex_};
    int w = m_mem->getWidth();

    qreal x = 0;
    qreal y = 0;
    data.clear();

    // For X display
    if (XY == 0) {
        for (int i = 0; i < size; ++i) {
            switch (m_mem->type()) {
            case U8:
                y = float(static_cast<unsigned char *>(m_mem->display())[offset * w + i]);
                x = float(i - axe_offset);
                break;
            case U16:
                y = float(static_cast<unsigned short *>(m_mem->display())[offset * w + i]);
                x = float(i - axe_offset);
                break;
            case BUFF_END:
                x = float(i - axe_offset);
                break;
            }
            data.append(QPointF(x, y));
        }
    }

    // For Y display
    if (XY == 1) {
        for (int i = 0; i < size; ++i) {
            switch (m_mem->type()) {
            case U8:
                y = float(static_cast<unsigned char *>(m_mem->display())[i * w + offset]);
                x = float(i - axe_offset);
                break;
            case U16:
                y = float(static_cast<unsigned short *>(m_mem->display())[i * w + offset]);
                x = float(i - axe_offset);
                break;
            case BUFF_END:
                x = float(i - axe_offset);
                break;
            }
            data.append(QPointF(x, y));
        }
    }
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

/*!
 * \brief MyChart::myUpdate
 * \param int &offset
 * \param int &size
 * \param int &XY (0 for X grpah. 1 for Y graph)
 * \param int &axe_offset
 * \param std::mutex *mutex_
 * 
 * Update the graphe
 * 
 * Laod data using the memory manager + display on chart
 * 
 * TO DO compute fit
 * 
 */
void MyChart::myUpdate(int &mem_offset, int &size, int &XY, int &axe_offset, std::mutex *mutex_)
{
    getDatas(mem_offset, size, XY, axe_offset, mutex_);
    addDataPoint(axe_offset);
}
