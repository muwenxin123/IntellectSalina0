#ifndef UAVPLANESTARE_FORFIRE_H
#define UAVPLANESTARE_FORFIRE_H

#include <QWidget>

#include <QTableWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <Protocol/XygsJL/sharedFile.h>
#include <QTreeWidgetItem>

#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/XYWeaponManage/XYWeaponManage.h"
#include "DataManage/IWeaponManageService.h"

#include "IPlatformStateMonitoringActivator.h"
#include "DataManage/WeaponSystem/WeaponSystem.h"

namespace Ui {
class UAVPlaneStare_ForFire;
}

class UAVPlaneStare_ForFire : public QWidget
{
    Q_OBJECT

public:
    explicit UAVPlaneStare_ForFire(QWidget *parent = nullptr);
    ~UAVPlaneStare_ForFire();

public:

	void init();

	void CreateTree();
	void setTableWidget();
	void updateTelemetryInformation(const QString &strWeaponComponentName, const QString &strWeaponComponentID, const telemetryInformation& telemetryInfo);
	void updateTelemetryInformation_timeOut(QString strWeaponComponentID);

private:
    Ui::UAVPlaneStare_ForFire *ui;

	qnzkna::WeaponManage::IWeaponManageService*				m_pWeaponManageService = nullptr;
	qnzkna::WeaponManage::WeaponSystemMap					m_pWeaponSystemMap;

	QPixmap													m_pix_G;
	QPixmap													m_pix_B;
	QPixmap													m_pix_R;

	bool                                              m_Heartbeats;

	int                                               m_msec;

	std::map<QString, QTreeWidgetItem*>				UAVItems;
};

#endif
