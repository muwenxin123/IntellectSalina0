#ifndef AGRIAPSIMMET_H_
#define AGRIAPSIMMET_H_
 
#include <QWidget>

#include "QCustomPlot/qcustomplot.h"
#include "AgriDataAnalysisPredictDecl.h"

namespace Ui
{
	class AgriAPSimMet;
}

class AgriAPSimMet : public QWidget
{
	Q_OBJECT

public:
	explicit AgriAPSimMet(QWidget *parent = nullptr);
	~AgriAPSimMet();
private:
	bool parseMetFile(AgriMetDataVec& vecMetData);
	void onTabBarClicked(int index);
	void handleRainfallChart();
	void handleTempeartureChart();
	void plotRainfall(QCustomPlot *customPlot, const QVector<double>& days, const QVector<double>& rainfall);
	void plotTemperate(QCustomPlot *customPlot, const QVector<double>& days, const QVector<double>& maxts, const QVector<double>& mint);
private:
	void showContextMenu(const QPoint &pos);
	void updatePlotStyle(QCustomPlot *customPlot);
private slots:
	void on_BT_LoadData_clicked();

private:
	Ui::AgriAPSimMet			*ui;
	QString						sFilePath;
	AgriMetDataVec				vecMetData;
};
#endif  // AgriAPSimMet_H