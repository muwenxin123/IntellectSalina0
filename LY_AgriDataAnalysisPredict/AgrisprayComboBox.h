#ifndef AgrisprayComboBox_H
#define AgrisprayComboBox_H

#include <QWidget>
#include "IProj.h"
#include "Zone/ZoneManagerInfo.h"
#include "QComboBox"
#include "QString"
#include "QPainter"
#include "AgriFarm/AgriFarmManagerDecl.h"
#include "AgriDataAnalysisPredictUtil.h"

class AgrisprayComboBox : public QComboBox
{
	Q_OBJECT
public:
	explicit AgrisprayComboBox(QWidget *parent = nullptr);
	~AgrisprayComboBox();

public:
	void initData();
	void zoneHighLight(QPainter &painter, IProj *pProj);
	void clearZoneHighLight();
	void selectedActiveFarm();
	void switchActiveFarm();
	qnzkna::ZoneManage::CZoneBase GetActiveFieldZone();
	void setActiveFieldInfo(AgriFieldInfo fieldInfo);

public slots:
	void slotCurrentTextChange(const QString& strText);

signals:
	void sigCurrentTextChange(const QString& strText);
	void setupField(QString &text, QString &file_id);

private:
	void setScreenCenter(double &dLon, double &dLat);

private:
	AgriFieldInfoVec m_vecFieldList;
	QString m_currentFarm;
	ThreadSafeMap<qnzkna::ZoneManage::CZoneBase> m_zoneDrawMap;
	AgriFieldInfo m_fieldInfo;

};

#endif // AgrisprayComboBox_H
