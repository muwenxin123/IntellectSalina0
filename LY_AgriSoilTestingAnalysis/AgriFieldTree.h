#ifndef AGRIFIELDMANAGERTREE_H_
#define AGRIFIELDMANAGERTREE_H_
 
#include <QWidget>
#include <QSqlQuery>
#include <QTreeWidget>

#include "IProj.h"
#include "map3d/IEarth.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "IAgriSoilAnalysisPredictActivator.h"
#include "LY_AgriSoilAnalysisPredictInterface.h"
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"

namespace Ui
{
	class AgriFieldTree;
}

class AgriFieldTree : public QWidget
{
	Q_OBJECT

public:
	explicit AgriFieldTree(QWidget *parent = nullptr);
	~AgriFieldTree();

public:
	void ZoneHighLight(QPainter &painter, IProj *pProj);
	void SwitchActiveFarm();
	void ClearZoneHighLight();
	qnzkna::ZoneManage::CZoneBase GetActiveFieldZone();
	void setActiveFieldInfo(AgriFieldInfo fieldInfo);
	QString GetCurrentFarm();

private:
	void Init();
	void InitTree();
	void CreateTree();
	void InitActiveFarm();
	void BuildTree(QTreeWidget *treeWidge);
	void SetScreenCenter(double &dLon, double &dLat);

signals:
	void LeafNodeClicked(QTreeWidgetItem *item, int column);

private slots:
	void onItemClicked(QTreeWidgetItem *item, int column);
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
	QString													m_strTreeTableName = "";
	QString													m_sCurActiveFarm = "";
	AgriFieldInfo											m_fieldInfo;
	std::map<std::string, std::string>                      SystemSysIDAndName;
	ThreadSafeMap<qnzkna::ZoneManage::CZoneBase>			m_zoneDrawMap;
	AgriFieldInfoVec										m_vecFieldList;

private:
	Ui::AgriFieldTree									*ui;
};
#endif  // AgriFieldTree_H