#include "mychart.h"
#include <QVBoxLayout>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>

myChart::myChart(QWidget *parent)
    : QWidget(parent)
    , m_listCount(3)
    , m_valueMax(10)
    , m_valueCount(7)

{
    m_dataTable = generateRandomData(m_listCount, m_valueMax, m_valueCount);
    //create charts
    m_chartsView = new QChartView(createLineChart());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_chartsView->setRenderHint(QPainter::Antialiasing, true);
    m_chartsView->chart()->legend()->hide();
    m_chartsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(m_chartsView);
    mainLayout->setContentsMargins(QMargins(0, 0, 0, 0));
}

myChart::~myChart()
{
    delete m_chartsView;
}

DataTable myChart::generateRandomData(int listCount, int valueMax, int valueCount) const
{
    DataTable dataTable;

    // generate random data
    for (int i(0); i < listCount; i++) {
        DataList dataList;
        qreal yValue(0);
        for (int j(0); j < valueCount; j++) {
            yValue = yValue + QRandomGenerator::global()->bounded(valueMax / (qreal) valueCount);
            QPointF value((j + QRandomGenerator::global()->generateDouble())
                              * ((qreal) m_valueMax / (qreal) valueCount),
                          yValue);
            QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
            dataList << Data(value, label);
        }
        dataTable << dataList;
    }

    return dataTable;
}

QChart *myChart::createLineChart() const
{
    //![1]
    QChart *chart = new QChart();
    chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chart->setMargins(QMargins(0, 0, 0, 0));
    //chart->setTitle("Line chart");
    //![1]

    //![2]
    QString name("Series ");
    int nameIndex = 0;
    for (const DataList &list : m_dataTable) {
        QLineSeries *series = new QLineSeries(chart);
        for (const Data &data : list)
            series->append(data.first);
        series->setName(name + QString::number(nameIndex));
        nameIndex++;
        chart->addSeries(series);
    }
    //![2]

    //![3]
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, m_valueMax);
    chart->axes(Qt::Vertical).first()->setRange(0, m_valueCount);
    //![3]
    //![4]
    // Add space to label to add space between labels and axis
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");
    //![4]

    return chart;
}
