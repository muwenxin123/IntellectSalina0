#ifndef AGRISATELLITEDATATABLE_H_
#define AGRISATELLITEDATATABLE_H_
 
#include <QWidget>
#include <QTableWidget>
#include <QProgressBar>

#include <osg/Vec3>

#include "AgriMultispectralSensorDecl.h"
#include "IAgriMultispectralSensorActivator.h"

class CustomProgressBar : public QProgressBar {
	Q_OBJECT
public:
	CustomProgressBar(QProgressBar *parent = nullptr);
	void updateValue(float value);
protected:
	/*void paintEvent(QPaintEvent *) override;*/
private:
	float m_value;
	float m_max;
};

class AgriSatelliteDataTable : public QTableWidget
{
	Q_OBJECT
public:
	explicit AgriSatelliteDataTable(QWidget *parent = 0);

public Q_SLOTS:
	void load();

private:
	void insertSize(const AgriFieldInfo& fieldInfo);
	bool writePolygonToShapefile(const std::vector<osg::Vec3>& points, const std::string& shpPath);

private:
	QString													m_sCurActiveFarm = "";
	AgriFieldInfoVec										m_vecFieldList;
};

#endif  // AgriSatelliteDataTable_H