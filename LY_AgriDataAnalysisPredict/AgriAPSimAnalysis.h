#ifndef AGRIAPSIMANALYSIS_H_
#define AGRIAPSIMANALYSIS_H_
 
#include <QWidget>

#include "QCustomPlot/qcustomplot.h"
#include "AgriDataAnalysisPredictDecl.h"

namespace Ui
{
	class AgriAPSimAnalysis;
}

class AgriAPSimAnalysis : public QWidget
{
	Q_OBJECT

public:
	explicit AgriAPSimAnalysis(QWidget *parent = nullptr);
	~AgriAPSimAnalysis();

public:
	void setOutputPath(const QString& filePath);

private:
	void onTabBarClicked(int index);
	bool parseOutputFile(AgriOutputDataVec& vecOutputData);
private:
	void plotChart(QCustomPlot *customPlot, const QVector<double>& xAxis, const QVector<double>& yAxis, const QString& xLabel, const QString& yLabel);
private:
	void showContextMenu(const QPoint &pos);
	void updatePlotStyle(QCustomPlot *customPlot);
	void updateXAxisLabels(QCustomPlot* customPlot, const AgriOutputDataVec& data);
private:
	Ui::AgriAPSimAnalysis			*ui;
	QString							sFilePath;
	AgriOutputDataVec				vecOutputData;
};

#endif  // AgriAPSimAnalysis_H