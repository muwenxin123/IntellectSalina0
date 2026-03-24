#ifndef MeteorologicalIntelligenceWidget_H
#define MeteorologicalIntelligenceWidget_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QLegendMarker>

namespace Ui {
class MeteorologicalIntelligenceWidget;
}

class MeteorologicalIntelligenceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MeteorologicalIntelligenceWidget(QWidget *parent = nullptr);
    ~MeteorologicalIntelligenceWidget();

	void setShowData(const QVector<int> &trueWindSpeed, const QVector<int> &forecastWindSpeed);

private:
    Ui::MeteorologicalIntelligenceWidget *ui;

	QtCharts::QChart* windSpeedChart;
	QtCharts::QSplineSeries* trueWindSpeedSeries;
	QtCharts::QSplineSeries* forecastWindSpeedSeries;
};

#endif
