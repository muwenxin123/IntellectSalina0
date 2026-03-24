#ifndef UAVPLANSYSTEMSTATE_H
#define UAVPLANSYSTEMSTATE_H

#include <QWidget>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <Protocol/XygsJL/sharedFile.h>

#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/XYWeaponManage/XYWeaponManage.h"

#include <QTreeWidget>
#include "DataManage/SystemInfo/SystemInternalInterface.h"
#include "QMutex"
#include "QtPositioning/QGeoCoordinate"

#include "Zone/ZoneManagerInfo.h"
#include "IProj.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IZoneManageService.h"
#include "DataManage/IWeaponManageService.h"
typedef QMap<QString,QTreeWidgetItem*> UAVTreeWidgetItemMap;

typedef std::vector<int> EventVec;
typedef std::map<QString, EventVec> UAVEventMap;
typedef std::map<QString, int> UAVRealTimeEventMap;

namespace Ui {
class UAVPlanSystemState;
}

class UAVPlanSystemState : public QWidget
{
    Q_OBJECT

public:
    explicit UAVPlanSystemState(QWidget *parent = nullptr);
    ~UAVPlanSystemState();

public:
    void setTableWidget();
    void updateTelemetryInformation(const QString &strWeaponComponentID, const QString &strWeaponComponentcontrolMode, const telemetryInformation & telemetryInfor);
    void updateTelemetryInformationTimeout(const QString &strWeaponComponentID);
	void updateTelemetryInformationTree(const QString &strWeaponComponentID, const QString &strWeaponComponentcontrolMode, const telemetryInformation & telemetryInfor);
	void updateTelemetryInformationTimeoutTree(const QString &strWeaponComponentID);

    void updateTelemetryInformationWarningTips(const QString &strWeaponComponentID, const qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t & telemetryInfo);
    void InitAllState();

	void CreateTree();

	void UpdateTreeComunicateStates(QTreeWidgetItem* pItem, int nStates);

	void UpdateTreeEventStates(const QString& strID, QTreeWidgetItem* pItem);

	void UpdateMissionStates(const EventInfo& stInfo);

	void InitUAVEventStates();

	bool CheckRoutePointOfZoneInside(qnzkna::ZoneManage::CZoneBase zone, QGeoCoordinate position);

	bool MatchToAccessRouteID_UAVName(const QString& strWeaponComponentID, int& nAirLineID);

	std::string LineMatchPlatform(const std::string &strMissionID, const int &nLineID, const int &nNo);

	void localWeaponNameBindToId();

private slots:

    void on_tableWidget_UAVPlanSystemState_itemDoubleClicked(QTableWidgetItem *item);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

protected:

    virtual void changeEvent(QEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    typedef struct RealTimeTelemetryInformation_Warning
    {
        unsigned int                                                   gpsNumberWarningMin    = 12;
        qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t information;
        double                                                         powervoltageWarningMin = 43.5;
        double                                                         angleofPitchWarningMin = 25;
        double                                                         angleofRollWarningMin  = 25;
    } Information_Warning_t;

    typedef struct WeaponTelemetryInfoTableSettings
    {
        int                                                            tableColumn           = -1;
        int                                                            tableColumnWidth      = 12;
    } WeaponTelemetryInfoTableSettings_t;

    void loadWeaponTelemetryInfoWarningSetting();
    void saveWeaponTelemetryInfoWarningSetting();

private:
	bool													m_Heartbeats;
	bool													m_show;
	unsigned int											m_msec;
	unsigned int											m_msecHeartbeats;

	Ui::UAVPlanSystemState									*ui;
	QMap<QString, QString>									m_WeaponComponentMap;
	QMap<QString, Information_Warning_t>					m_WeaponComponenTelemetryInformationMap;
	QMap<int, WeaponTelemetryInfoTableSettings_t>			m_WeaponTelemetryInfoTableSettingsMap;
	const CXYWeaponManage									m_CXYWeaponManage;
	QPixmap													m_pix_G;
	QPixmap													m_pix_B;
	QPixmap													m_pix_R;
	UAVTreeWidgetItemMap									m_UAVTreeWidgetItemMap;
	QStringList												m_TreeHeader;
	UAVEventMap												m_UAVEventMap;
	UAVRealTimeEventMap										m_UAVRealTimeEventMap;
	QMutex													sMutex;

	IProj*													m_proj;
	qnzkna::TaskManage::INebulaTaskManageService*			m_pITaskManageService = nullptr;
	qnzkna::ZoneManage::IZoneManageService*					m_pIZoneManageService = nullptr;
	qnzkna::WeaponManage::IWeaponManageService*				m_pIWeaponManageService = nullptr;

	std::map<std::string, std::string>						m_SystemSysNameAndID;
	QMap<QString, QGeoCoordinate>							m_mapLocatonCoverUAV;
	size_t													n_DeviationVoyageCount = 0;
	QMap<QString, int>										m_mapDeviationedtoReover;
};

#endif
