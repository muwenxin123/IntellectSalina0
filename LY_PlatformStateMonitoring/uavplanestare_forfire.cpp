#include "uavplanestare_forfire.h"
#include "ui_uavplanestare_forfire.h"

#include "QCString.h"

#include <QLabel>
#include <QFile>
#include <QDateTime>
#include <QSettings>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>

const char* const UAVPlanSystemState_Vehicle_WeaponTelemetryInfoWarning = "WeaponTelemetryInfoWarning";
const char* const UAVPlanSystemState_Vehicle_WeaponID = "WeaponID";
const char* const UAVPlanSystemState_PowervoltageWarningMin = "PowervoltageWarningMin";
const char* const UAVPlanSystemState_GpsNumberWarningMin = "GpsNumberWarningMin";
const char* const UAVPlanSystemState_AngleofPitchWarningMin = "AngleofPitchWarningMin";
const char* const UAVPlanSystemState_AngleofRollWarningMin = "AngleofRollWarningMin";

const char* const UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableSettings = "WeaponTelemetryInfoTableSettings";
const char* const UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumn = "TableColumn";
const char* const UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumnWidth = "TableColumnWidth";

UAVPlaneStare_ForFire::UAVPlaneStare_ForFire(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::UAVPlaneStare_ForFire)
{
	ui->setupUi(this);
	init();
	CreateTree();
	setTableWidget();

}

UAVPlaneStare_ForFire::~UAVPlaneStare_ForFire()
{
    delete ui;
}

void UAVPlaneStare_ForFire::init()
{
	m_pWeaponManageService = IPlatformStateMonitoringActivator::Get().getPIWeaponManageService();
	if (m_pWeaponManageService != nullptr)
	{
		m_pWeaponManageService->GetWeaponSystemMap(m_pWeaponSystemMap);
		auto weaponSystemItor = m_pWeaponSystemMap.begin();
		while (weaponSystemItor != m_pWeaponSystemMap.end())
		{
			if (weaponSystemItor->second.getSystemType() != qnzkna::WeaponManage::WeaponSystem_Type_UAV)
			{
				weaponSystemItor = m_pWeaponSystemMap.erase(weaponSystemItor);
			}
			else
			{
				weaponSystemItor++;
			}
		}
	}

	m_pix_G.load(tr2(":/PlatformStateMonitoring/state_grey.png"));
	m_pix_B.load(tr2(":/PlatformStateMonitoring/state_blue.png"));
	m_pix_R.load(tr2(":/PlatformStateMonitoring/state_red.png"));
}

void UAVPlaneStare_ForFire::CreateTree()
{
	ui->treeWidget->clear();

	ui->treeWidget->setColumnCount(6);

	QStringList listheader;
	listheader << tr2("装备平台") << tr2(" 挂载 ") << tr2(" 通信状态 ") << tr2("经度(°)") << tr2("纬度(°)") << tr2("高度(m)") << tr2("电压") << tr2("电流");
	ui->treeWidget->setHeaderLabels(listheader);

	for (int i = 0; i < listheader.size(); i++)
	{
			ui->treeWidget->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
	}

	ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
}

void UAVPlaneStare_ForFire::setTableWidget()
{
	auto weaponSystemItor = m_pWeaponSystemMap.begin();
	while (weaponSystemItor != m_pWeaponSystemMap.end() && weaponSystemItor->second.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type_UAV)
	{
		auto weaponSystem = weaponSystemItor->second.getMapWeaponComponent();
		for (auto weapon : weaponSystem)
		{
			QTreeWidgetItem* pRootItem = new QTreeWidgetItem(ui->treeWidget);
			pRootItem->setText(0, QString::fromStdString(weapon.second.getComponentName()));
			pRootItem->setData(0, Qt::UserRole, QString::fromStdString(weapon.second.getComponentID()));
			pRootItem->setTextAlignment(0, Qt::AlignCenter);

			if (UAVItems.find(QString::fromStdString(weapon.second.getComponentID())) == UAVItems.end())
			{
				UAVItems[QString::fromStdString(weapon.second.getComponentID())] = pRootItem;
			}

			QLabel *pic = new QLabel();
			pic->setPixmap(m_pix_G);
			pic->setAlignment(Qt::AlignCenter);

			pRootItem->setText(3, tr("-"));
			pRootItem->setTextAlignment(3, Qt::AlignCenter);
			pRootItem->setText(4, tr("-"));
			pRootItem->setTextAlignment(4, Qt::AlignCenter);
			pRootItem->setText(5, tr("-"));
			pRootItem->setTextAlignment(5, Qt::AlignCenter);
			pRootItem->setText(6, tr("-"));
			pRootItem->setTextAlignment(6, Qt::AlignCenter);
			pRootItem->setText(7, tr("-"));
			pRootItem->setTextAlignment(7, Qt::AlignCenter);

			ui->treeWidget->setItemWidget(pRootItem, 2, pic);

			QTreeWidgetItem* CarChildItem = new QTreeWidgetItem(pRootItem);
			CarChildItem->setText(1, tr2("吊舱"));
			CarChildItem->setTextAlignment(1, Qt::AlignCenter);
			ui->treeWidget->setItemWidget(CarChildItem, 2, pic);

			if (weapon.second.getComponentID() != "110301")
			{

				QTreeWidgetItem* DanChildItem = new QTreeWidgetItem(pRootItem);
				DanChildItem->setText(1, tr2("火力"));
				DanChildItem->setTextAlignment(1, Qt::AlignCenter);
			}

		}
		weaponSystemItor++;
	}
}

void UAVPlaneStare_ForFire::updateTelemetryInformation(const QString & strWeaponComponentName, const QString & strWeaponComponentID, const telemetryInformation & telemetryInfo)
{
	auto uavItem = UAVItems.find(strWeaponComponentID);
	if (uavItem != UAVItems.end())
	{
		QTreeWidgetItem* tWItem = uavItem->second;
		QLabel *pic = new QLabel();
		pic->setPixmap(m_pix_B);
		pic->setAlignment(Qt::AlignCenter);
		ui->treeWidget->setItemWidget(uavItem->second, 2, pic);

		uavItem->second->setText(3, QString::number(telemetryInfo.position.lng));
		uavItem->second->setTextAlignment(3, Qt::AlignCenter);
		uavItem->second->setText(4, QString::number(telemetryInfo.position.lat));
		uavItem->second->setTextAlignment(4, Qt::AlignCenter);
		uavItem->second->setText(5, QString::number(telemetryInfo.gpsHeigth));
		uavItem->second->setTextAlignment(5, Qt::AlignCenter);
		uavItem->second->setText(6, QString::number(telemetryInfo._voltage));
		uavItem->second->setTextAlignment(6, Qt::AlignCenter);
		uavItem->second->setText(7, QString::number(telemetryInfo._current));
		uavItem->second->setTextAlignment(7, Qt::AlignCenter);
	}
}

void UAVPlaneStare_ForFire::updateTelemetryInformation_timeOut(QString strWeaponComponentID)
{
	auto uavItem = UAVItems.find(strWeaponComponentID);
	if (uavItem != UAVItems.end())
	{
		QTreeWidgetItem* tWItem = uavItem->second;
		QLabel *pic = new QLabel();
		pic->setPixmap(m_pix_G);
		pic->setAlignment(Qt::AlignCenter);
		ui->treeWidget->setItemWidget(uavItem->second, 2, pic);
	}
}

