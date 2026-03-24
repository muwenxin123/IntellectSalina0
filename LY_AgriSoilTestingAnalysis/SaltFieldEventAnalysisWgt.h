#ifndef SALT_FIELD_EVENT_ANALYSIS_WGT_H__
#define SALT_FIELD_EVENT_ANALYSIS_WGT_H__

#include <QWidget>
#include <QGeoCoordinate>
#include <Protocol/XygsJL/sharedFile.h>

#include "IProj.h"
#include "map3d/IEarth.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "IAgriSoilAnalysisPredictActivator.h"
#include "AgriSoilTestingDistributeWidget.h"
#include "AgriFieldTree.h"
#include "SaltFieldSortFilterModel.h"
#include "CommDelegate.h"
#include "SaltFieldEventAnalysisDetailWgt.h"
#include <QMenu>
#include <QAction>

namespace Ui
{
	class SaltFieldEventAnalysisWgt;
}

class AgriSoilTestingDataSetDialog;
class SaltFieldEventAnalysisWgt : public QWidget
{
	Q_OBJECT

public:
	explicit SaltFieldEventAnalysisWgt(QWidget *parent = nullptr);
	~SaltFieldEventAnalysisWgt();

	AgriFieldTree* getAgriFieldTree();

	qnzkna::ZoneManage::CZoneBase GetActiveFieldZone();
	void UpdataCurActiveFarm();
	void DrawDynamic(QPainter &painter, IProj *pProj);
	void On2d3dChanged(int wParam);

	void AddEvent(SaltFieldEventInfo& soilTestingDataset);

	QString GetAreaID(double lon, double lat);
	QString GenerateName(SaltFieldEventInfo& info);

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void Init();
	void InitDisplayTable();

	// 射线法判断点是否在多边形内
	bool pointInPolygon(const QGeoCoordinate &point,
		const QVector<QGeoCoordinate> &polygon);

	void ViewImage(QString fileName);
private:
	void UpdateTableData();
	void AppendOneRowTotable(SaltFieldEventInfoPtr soilTestingDataset);
	void UpdateOneRowTotable(SaltFieldEventInfoPtr soilTestingDataset);
	void Update3DIconDraw(const SaltFieldEventInfoPtr& soilTestingCollectVec, bool isDraw);
	void UpdateTableIndex();
	QString GetWeaponName(const QString& reporterID);

private slots:
	void onCellClicked(const QModelIndex &index);
	void onCellDoubleClicked(const QModelIndex &index);
	void onItemChanged(QStandardItem *item);
	void 	onSearchClicked();
	void 	onDeleteClicked();

	void on_Menu_Anakysis();

private:
	Ui::SaltFieldEventAnalysisWgt											*ui;
	bool																	m_show = false;
	int																		m_nSelectedIndex = -1;
	AgriFieldInfoPtr														m_pFieldInfo;
	QString																	m_sIconPath;
	std::vector<ILabel*>													m_3DPestCollectIconVec;
	ThreadSafeMap<SaltFieldEventInfoPtr>									m_soiltestingDrawMap;
	SaltFieldEventAnalysisDetailWgt*										m_pDetaiWidget;

	QStandardItemModel*														m_pModel = nullptr;
	SaltFieldSortFilterModel*												m_pSortModel = nullptr;
	CommPixMapDelegate*														m_pPixMapDelegate = nullptr;
	ComboboxDelegate*														m_pEventTypeDelegate = nullptr;
	QMap<int, QStandardItem*>												m_id2DataMap;
	QMap<int, SaltFieldEventTypeInfo>										m_eventID2DataMap;
	QMap<QString, int>														m_eventCode2IDMap;

	QMenu*																	m_tableMenu;
	QAction*																m_tableDelete;
	QAction*																m_tableAnakysis;
	QAction*																m_tableView;


};

#endif // SALT_FIELD_EVENT_ANALYSIS_WGT_H__
