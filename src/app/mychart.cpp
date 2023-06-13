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
MyChart::MyChart(const int _axe_XY, QColor pen_Color, int bitDepth, QWidget *parent)
    : QWidget{parent}
    , axe_XY(_axe_XY)
    , penColor(pen_Color)
{
    // Initialize params
    maxY = int(pow(2, bitDepth)) - 1;
    f_data = new double[N_fit];

    // Create chart
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
void MyChart::drawData()
{
    m_serieData->replace(data);
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
void MyChart::drawFit()
{
    m_serieFit->replace(fit);
}

/*!
 * \brief MyChart::setSize
 * \param newN
 * 
 * Setter for the size of the cut
 * 
 */
void MyChart::setSize(size_t newN)
{
    N = newN;
    axisX->setRange(0, N - 1);
}

/*!
 * \brief MyChart::getDataList
 * \return 
 * 
 * Get pointer to data list
 */
QList<QPointF> *MyChart::getDataList()
{
    return &data;
}

/*!
 * \brief MyChart::getFitList
 * \return 
 * 
 * Get pionter to fit list
 */
double MyChart::getFitParam(int id)
{
    return f_param[id];
}

/*!
 * \brief MyChart::copyFitData
 * \param fit_params
 * 
 * Private slot called when fit is finished to copy
 * the fit datas, emit a signal for transforming to QList
 * then draw !
 * 
 * TO DO in the worker class into a different thread ?
 * 
 */
void MyChart::copyFitData(double *fit_data, double *fit_param)
{
    std::scoped_lock lock(m_mutex_fit);
    memcpy(f_data, fit_data, sizeof(double) * N_fit);
    memcpy(f_param, fit_param, sizeof(double) * P);
    emit receivedFit(&m_mutex_fit, f_param);
}

/*!
 * \brief MyChart::processFitData
 * 
 * Private slot to proces the fit data (tranfor to QList)
 *  * 
 */
void MyChart::processFitData(std::mutex *mutex, double *params)
{
    double x = 0.0;
    fit.clear();
    for (int i = 0; i < N_fit; ++i) {
        x = i / ((double) (N_fit - 1)) * (N - 1);
        fit.append(QPointF(x, f_data[i]));
    }
    drawFit();
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
    m_serieData = new QScatterSeries();
    m_serieFit = new QLineSeries();

    // Courbe style for Data
    m_serieData->setColor(penColor);
    m_serieData->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_serieData->setMarkerSize(1.5);
    m_serieData->setUseOpenGL(true);

    // Courbe style for Fit
    QPen pen(penColor);
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);
    m_serieFit->setPen(pen);

    // Creat new chart with two series (Data & Fit)
    m_chart = new QChart();
    m_chart->addSeries(m_serieData);
    m_chart->addSeries(m_serieFit);

    // No legend
    m_chart->legend()->hide();

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
    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_serieFit->attachAxis(axisX);
    m_serieData->attachAxis(axisX);
    //axisX->setRange(0, N - 1);

    axisY = new QValueAxis();
    axisY->setRange(0, maxY);
    axisY->setLabelFormat("%i");
    axisY->setTickCount(5);
    axisY->setGridLineVisible(true);
    axisY->setMinorTickCount(1);
    axisY->setMinorGridLineVisible(true);
    axisY->setGridLinePen(majorGrid);
    axisY->setMinorGridLinePen(minorGrid);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_serieData->attachAxis(axisY);
    m_serieFit->attachAxis(axisY);

    // Remove padding
    m_chart->setMargins(QMargins(0, 0, 0, 0));

    // Remove roundness
    //graphe->setBackgroundRoundness(0);

    // Remove margin
    m_chart->layout()->setContentsMargins(0, 0, 0, 0);

    // ChartView Widget (antialiasing renderer)
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    // Add to a layout otherwise it is hidden
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_chartView);
    layout->setContentsMargins(0, 0, 0, 0);

    //All margins are remove !
    // Spacing control is now done with the layout in mainwindow !
}

/*!
 * \brief MyChart::setUpWorker
 * 
 * function to create the worker !
 * 
 * 
 */
void MyChart::setUpWorker()
{
    if (!workerSet) {
        d_data = new double[N];
        f_param = new double[P];
        work_fit = new WorkerFit(N, P, N_fit, &m_mutex_fit);
        for (int i = 0; i < N; i++) {
            d_data[i] = 0;
        }

        // Connection
        myConnection();

        workerSet = true;
    }
}

/*!
 * \brief MyChart::stopFit
 * 
 * stop the fit loop
 * 
 */
void MyChart::stopFit()
{
    if (workerSet) {
        work_fit->stop();
    }
}

/*!
 * \brief MyChart::startSingleFit
 * 
 * launch fit for a single frame
 * 
 */
void MyChart::startSingleFit()
{
    stopFit();
    if (workerSet) {
        work_fit->startsingle();
    }
}

/*!
 * \brief MyChart::startLoopFit
 * 
 * Start fitting loop
 * 
 */
void MyChart::startLoopFit()
{
    if (workerSet) {
        work_fit->startLoop(100);
    }
}

/*!
 * \brief MyChart::connect
 * 
 * Set up all connection between worker and chart
 * 
 */
void MyChart::myConnection()
{
    // connect end of copy fit to process fit
    connect(this, &MyChart::receivedFit, this, &MyChart::processFitData);

    // Connect seriesCahnge (whan new data arrived) to setData of the worker
    connect(this, &MyChart::receivedData, work_fit, &WorkerFit::setData, Qt::QueuedConnection);

    // Connect end fit to copy dataFit
    connect(work_fit, &WorkerFit::fitEND, this, &MyChart::copyFitData, Qt::QueuedConnection);
}

/*!
 * \brief MyChart::getDatas
 * \param int &offset
 * \param int &axe_offset
 * \param std::mutex &mutex_
 * 
 * Load data from the display buffer onto the QList<QPointF> private property
 * for quick display and onto the double array for quick copying into the 
 * the workerFit
 * 
 * TO DO on other thread ??? maybe not since double buffer prevent a lot
 * of memory conflict !
 * 
 */
void MyChart::getDatas(int &offset, std::mutex *mutex_)
{
    std::scoped_lock locker(*mutex_, m_mutex_fit);
    int w = m_mem->getWidth();

    qreal x = 0;
    qreal y = 0;
    data.clear();

    // For X display
    if (axe_XY == 0) {
        for (int i = 0; i < N; ++i) {
            switch (m_mem->type()) {
            case U8:
                y = float(static_cast<unsigned char *>(m_mem->display())[offset * w + i]);
                x = i; //float(i - axe_offset);
                break;
            case U16:
                y = float(static_cast<unsigned short *>(m_mem->display())[offset * w + i]);
                x = i; //float(i - axe_offset);
                break;
            case BUFF_END:
                x = i; //float(i - axe_offset);
                break;
            }
            data.append(QPointF(x, y));
            d_data[i] = (double) y;
        }
    }

    // For Y display
    if (axe_XY == 1) {
        for (int i = 0; i < N; ++i) {
            switch (m_mem->type()) {
            case U8:
                y = float(static_cast<unsigned char *>(m_mem->display())[i * w + offset]);
                x = i; //float(i - axe_offset);
                break;
            case U16:
                y = float(static_cast<unsigned short *>(m_mem->display())[i * w + offset]);
                x = i; //float(i - axe_offset);
                break;
            case BUFF_END:
                x = i; //float(i - axe_offset);
                break;
            }
            data.append(QPointF(x, y));
            d_data[i] = (double) y;
        }
    }

    emit receivedData(d_data);
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
 * \param int &axe_offset
 * \param std::mutex *mutex_
 * 
 * Update the graphe
 * 
 * Laod data using the memory manager + display on chart
 * 
 * emit signal to compute fit / start fit ??!
 * 
 */
void MyChart::myUpdate(int &mem_offset, std::mutex *mutex_)
{
    getDatas(mem_offset, mutex_);
    drawData();
    drawFit();
}
