#include "MeteorologicalIntelligenceWidget.h"
#include "ui_MeteorologicalIntelligenceWidget.h"
#include "QCString.h"

MeteorologicalIntelligenceWidget::MeteorologicalIntelligenceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MeteorologicalIntelligenceWidget)
{
    ui->setupUi(this);

	windSpeedChart = new QtCharts::QChart;
	windSpeedChart->setTheme(QtCharts::QChart::ChartThemeBlueCerulean);
	QBrush brush(QColor("#072037"));
	windSpeedChart->setBackgroundBrush(brush);
	auto windSpeedChartView = new QtCharts::QChartView(windSpeedChart);

	trueWindSpeedSeries = new QtCharts::QSplineSeries;
	trueWindSpeedSeries->setName(tr2("实测风速"));
	windSpeedChart->addSeries(trueWindSpeedSeries);
	forecastWindSpeedSeries = new QtCharts::QSplineSeries;
	forecastWindSpeedSeries->setName(tr2("预测风速"));
	windSpeedChart->addSeries(forecastWindSpeedSeries);
	windSpeedChart->createDefaultAxes();
	windSpeedChart->axisX()->setRange(0, 24);

	windSpeedChart->axisX()->setLabelsColor(QColor("#cad4e4"));
	brush.setColor(QColor("#cad4e4"));
	windSpeedChart->axisX()->setTitleBrush(brush);
	windSpeedChart->axisY()->setRange(0, 12);

	windSpeedChart->axisY()->setLabelsColor(QColor("#cad4e4"));
	windSpeedChart->axisY()->setTitleBrush(brush);
	windSpeedChartView->setChart(windSpeedChart);
	ui->groupBox_chart->layout()->addWidget(windSpeedChartView);

	ui->lb_flyadvice_text->setStyleSheet("font-size: 20px;");
	QPixmap pixmap1;
	pixmap1.load(":/images/1k/black/MeteorologicalIntelligenceWidget/sun.png");
	pixmap1.scaled(QSize(24, 24));
	ui->lb_weather_icon->setPixmap(pixmap1);
	QPixmap pixmap2;
	pixmap2.load(":/images/1k/black/MeteorologicalIntelligenceWidget/wind.png");
	pixmap2.scaled(QSize(24, 24));
	ui->lb_windspeed_icon->setPixmap(pixmap2);
	QPixmap pixmap3;
	pixmap3.load(":/images/1k/black/MeteorologicalIntelligenceWidget/winddir.png");
	pixmap3.scaled(QSize(24, 24));
	ui->lb_winddir_icon->setPixmap(pixmap3);

	QVector<int> trueWindSpeed{ 2,2,3,3,4,6,6,7,8,8,8,8,9,9,8,7,5,5,4,4,5,7,7,7 };
	QVector<int> forecastWindSpeed{ 1,1,2,2,4,6,6,7,7,7,9,9,10,10,9,6,5,5,3,3,5,5,5,4 };

	setShowData(trueWindSpeed, forecastWindSpeed);
}

MeteorologicalIntelligenceWidget::~MeteorologicalIntelligenceWidget()
{
    delete ui;
}

void MeteorologicalIntelligenceWidget::setShowData(const QVector<int> &trueWindSpeed, const QVector<int> &forecastWindSpeed)
{
	for (int i = 0; i < 24; i++) {
		trueWindSpeedSeries->append(i + 1, trueWindSpeed[i]);
		forecastWindSpeedSeries->append(i + 1, forecastWindSpeed[i]);
	}
}
