#ifndef MYCHART_H
#define MYCHART_H

#include <QChart>
#include <QChartView>
#include <QWidget>

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

class myChart : public QWidget
{
    Q_OBJECT
public:
    explicit myChart(QWidget *parent = 0);
    ~myChart();

private:
    DataTable generateRandomData(int listCount, int valueMax, int valueCount) const;
    QChart *createLineChart() const;

private:
    QChartView *m_chartsView;
    DataTable m_dataTable;
    int m_listCount;
    int m_valueMax;
    int m_valueCount;
};

#endif // MYCHART_H
