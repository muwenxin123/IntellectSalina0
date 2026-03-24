#ifndef AGRISATELLITEDATAANALYSIS_H_
#define AGRISATELLITEDATAANALYSIS_H_
 
#include <QWidget>
#include <QtableWidget>

#include "AgriMultispectralSensorDecl.h"
#include "AgriSatelliteDataTree.h"

namespace Ui
{
	class AgriSatelliteDataAnalysis;
}

class AgriSatelliteDataAnalysis : public QWidget
{
	Q_OBJECT

public:
	explicit AgriSatelliteDataAnalysis(QWidget *parent = nullptr);
	~AgriSatelliteDataAnalysis();

public:
	void UpdateShowLegend(std::vector<AgriSatelliteDataTree::ImageInfo>& imageVec);

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void Init();
	void InitGrowthDisplayLegend(QTableWidget *table);
	void InitYieldDisplayLegend(QTableWidget *table);

	void UpdateGrowthLegendLabel(QLabel *label);
	void UpdateYieldLegendLabel(QLabel *label);

	void UpdateGrowthLegendTable(QTableWidget *table, const QVector<QVector<QString>> &growthList);
	void UpdateYieldLegendTable(QTableWidget *table, const QVector<QVector<QString>> &yieldList);

private:
	Ui::AgriSatelliteDataAnalysis		*ui;
	std::unordered_map<std::string, QVector<QVector<QString>>> m_growthListMap = {
		{"20250416T051208", {
			{ "#379123", tr2("优"), "11", "16" },
			{ "#b9db86", tr2("良"), "19", "29" },
			{ "#f2ae79", tr2("中"), "23", "35" },
			{ "#ba1414", tr2("差"), "14", "20" },
		} },
		{ "20250511T020512",{
			{ "#379123", tr2("优"), "19", "28" },
			{ "#b9db86", tr2("良"), "18", "27" },
			{ "#f2ae79", tr2("中"), "19", "28" },
			{ "#ba1414", tr2("差"), "11", "17" },
		} },
		{ "20250607T012549",{
			{ "#379123", tr2("优"), "13", "19" },
			{ "#b9db86", tr2("良"), "21", "29" },
			{ "#f2ae79", tr2("中"), "20", "32" },
			{ "#ba1414", tr2("差"), "13", "20" },
		} },
		{ "20250620T030519",{
			{ "#379123", tr2("优"), "30", "45" },
			{ "#b9db86", tr2("良"), "28", "41" },
			{ "#f2ae79", tr2("中"), "8", "11" },
			{ "#ba1414", tr2("差"), "2",  "3" },
		} }
	};

	QVector<QVector<QString>> m_yieldList = {
		{ "#6b0000", "90-115", "18.76", "28" },
		{ "#ad5313", "116-131", "16.75", "25" },
		{ "#f2a72e", "132-145", "16.08", "24" },
		{ "#fad155", "146-160", "10.05", "15" },
		{ "#ffff80", "161-183", "5.36", "8" },
	};
};

#endif  // AgriSatelliteDataAnalysis_H