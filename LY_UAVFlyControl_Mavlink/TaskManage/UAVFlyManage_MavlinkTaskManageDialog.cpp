#include "UAVFlyManage_MavlinkMissionManagerDialog.h"
#include "UAVFlyManage_MavlinkTaskManageDialog.h"
#include "ui_UAVFlyManage_MavlinkTaskManageDialog.h"
#include "TaskManage/UAVFlyManage_MavlinkMissionManagerOpenFileDialog.h"
#include "QGCApplication.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "MissionItem.h"
#include "IProj.h"
#include "lyUI/LyMessageBox.h"
#include "lyUI/LyInputDialog.h"
#include <QMouseEvent>
#include <QPainter>
#include <QTextCodec>
#include <QCString.h>

#define  LINE_LENGTH              3
#define  LINE_DEFAULT_ALTITUDE    35

#define UAVFLYMANAGE_MAVLINKMISSIONMANAGERDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID "VehicleWeaponId_VehicleID"

UAVFlyManage_MavlinkTaskManageDialog::UAVFlyManage_MavlinkTaskManageDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_MavlinkTaskManageDialog)
    , m_TaskManageLineEditStates(TaskManageLineEditStates::TaskManageLinePickNone)
    , m_TabWidgetMenu(nullptr)
    , m_defaultAltitude(LINE_DEFAULT_ALTITUDE)
{
    ui->setupUi(this);

    ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesRemoveAll->setVisible(false);
    ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesRemoveAll->hide();

    ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll->setVisible(false);
    ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll->hide();

    if(!QGCApplication::upDownVehicleAllMissionEnabled()){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_UploadAllWayPoints->setVisible(false);
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_UploadAllWayPoints->hide();

        ui->pushButton_UAVFlyManage_MavlinkTaskManage_DownloadAllWayPoints->setVisible(false);
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_DownloadAllWayPoints->hide();
    }

    ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->setMovable(false);
    ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->clear();

    int colorIndex = 0;
    m_Color[colorIndex]   = 0xffff00;
    m_Color[colorIndex++] = 0xffff00;
    m_Color[colorIndex++] = 0x800000;
    m_Color[colorIndex++] = 0x808000;
    m_Color[colorIndex++] = 0x00ff00;
    m_Color[colorIndex++] = 0x008000;
    m_Color[colorIndex++] = 0x00ffff;
    m_Color[colorIndex++] = 0x008080;
    m_Color[colorIndex++] = 0x0000ff;
    m_Color[colorIndex++] = 0x000080;
    m_Color[colorIndex++] = 0xff00ff;
    m_Color[colorIndex++] = 0x800080;
    m_Color[colorIndex++] = 0xffa500;
    m_Color[colorIndex++] = 0x404040;
    m_Color[colorIndex++] = 0xc000c0;
    m_Color[colorIndex++] = 0x00ffff;
    m_Color[colorIndex++] = 0xaa5500;
    m_Color[colorIndex++] = 0xc0c0c0;
    m_Color[colorIndex++] = 0x808080;
    m_Color[colorIndex++] = 0x000000;
    m_Color[colorIndex++] = 0x0FF0FF;
    m_Color[colorIndex++] = 0x0FFF0F;
    m_Color[colorIndex++] = 0xFF0F0F;
    m_Color[colorIndex++] = 0xF0FF0F;

    m_Color[colorIndex]   = 0xffffff;

    m_WayPointImage.load(":/images/marker.png");
    m_WayPointImageSelected.load(":/images/marker_sel.png");

    connect(ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task, &QComboBox::currentTextChanged, this, &UAVFlyManage_MavlinkTaskManageDialog::on_comboBox_UAVFlyManage_MavlinkTaskManage_Task_currentChanged);

    m_TabWidgetMenu = new QMenu(this);
	m_actionMenuStart = new QAction(tr("\xe5\xbc\x80\xe5\xa7\x8b\xe7\xbc\x96\xe8\xbe\x91"));
	m_actionMenuUpdate = new QAction(tr("\xe4\xb8\x8a\xe4\xbc\xa0\xe8\x88\xaa\xe7\x82\xb9"));
	m_actionMenuUpdown = new QAction(tr("\xe4\xb8\x8b\xe8\xbd\xbd\xe8\x88\xaa\xe7\x82\xb9"));
	m_TabWidgetMenu->addAction(m_actionMenuStart);

    connect(m_actionMenuStart, &QAction::triggered, this, [this](){
        this->m_TaskManageLineEditStates = TaskManageLineEditStates::TaskManageLinePickLinePoint;

        if(!ui){
            return ;
        }

        if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return ;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return ;
        }

        startMissionPlan();

    });

    connect(m_TabWidgetMenu->addAction(tr("\xe5\x81\x9c\xe6\xad\xa2\xe7\xbc\x96\xe8\xbe\x91")), &QAction::triggered, this, [this](){
        this->m_TaskManageLineEditStates = TaskManageLineEditStates::TaskManageLinePickNone;

        if(!ui){
            return ;
        }

        if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return ;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return ;
        }

        stopMissionPlan();

    });

    connect(m_TabWidgetMenu->addAction(tr("\xe6\x8f\x92\xe5\x85\xa5\xe8\x88\xaa\xe7\x82\xb9")), &QAction::triggered, this, [this](){
        if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {

            if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
            if(!pUAVFlyManage_MavlinkMissionManagerDialog){
                return ;
            }

            m_TaskManageLineEditStates = TaskManageLineEditStates::TaskManageLinePickSinglePoint;

        }
    });

	m_TabWidgetMenu->addAction(m_actionMenuUpdate);
    connect(m_actionMenuUpdate, &QAction::triggered, this, [this](){
         if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {
                if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
                    return ;
                }

                if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
                    return ;
                }

                UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
                if(!pUAVFlyManage_MavlinkMissionManagerDialog){
                    return ;
                }

                pUAVFlyManage_MavlinkMissionManagerDialog->sendToVehicle();
         }

    });

	m_TabWidgetMenu->addAction(m_actionMenuUpdown);
    connect(m_actionMenuUpdown, &QAction::triggered, this, [this](){
        if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {
            if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
            if(!pUAVFlyManage_MavlinkMissionManagerDialog){
                return ;
            }

            pUAVFlyManage_MavlinkMissionManagerDialog->loadFromVehicle();

        }
    });

    connect(m_TabWidgetMenu->addAction(tr("\xe5\x88\xa0\xe9\x99\xa4\xe9\x80\x89\xe6\x8b\xa9\xe8\x88\xaa\xe7\x82\xb9")), &QAction::triggered, this, [this](){
        if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {

            if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
            if(!pUAVFlyManage_MavlinkMissionManagerDialog){
                return ;
            }

            pUAVFlyManage_MavlinkMissionManagerDialog->removeSelectedMissionItem();
        }
    });

    connect(m_TabWidgetMenu->addAction(tr("\xe5\x88\xa0\xe9\x99\xa4\xe6\x89\x80\xe6\x9c\x89\xe8\x88\xaa\xe7\x82\xb9")), &QAction::triggered, this, [this](){
        if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint){

            if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
            if(!pUAVFlyManage_MavlinkMissionManagerDialog){
                return ;
            }

            pUAVFlyManage_MavlinkMissionManagerDialog->clearMissionItems();
        }
    });

    connect(m_TabWidgetMenu->addAction(tr("\xe8\xae\xbe\xe7\xbd\xae\xe4\xb8\xba\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe5\xbd\x93\xe5\x89\x8d\xe4\xbd\x8d\xe7\xbd\xae")), &QAction::triggered, this, [this](){
        if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint){

            if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
                return ;
            }

            UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
            if(!pUAVFlyManage_MavlinkMissionManagerDialog){
                return ;
            }

            pUAVFlyManage_MavlinkMissionManagerDialog->updateMissionItemFromVehicle();
        }
    });

    stopMissionPlan();

    if(ui != nullptr){
        m_UAVFlyManage_MavlinkMissionManagerDialogList.clear();
        ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->clear();

        const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
        for(int i = 0; i < tabWidgetCount; i++){
            QWidget *widget = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i);
            UAVFlyManage_MavlinkMissionManagerDialog *pUAVFlyManagpe_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(widget);
            if(pUAVFlyManagpe_MavlinkMissionManagerDialog){
                pUAVFlyManagpe_MavlinkMissionManagerDialog->clearMissionItems();
            }
            ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->removeTab(0);
            if(widget){
                widget->deleteLater();
            }
            qDebug() << __FILE__ << __LINE__ << i << tabWidgetCount;
        }
    }

    //if(!QGCApplication::createVehicleMissionLineFromTaskEnabled())
	{
        InitWeaponInfoListNoTask();
    }

    if(QGCApplication::multiVehicleManager()){
        connect(QGCApplication::multiVehicleManager(), &MultiVehicleManager::vehicleAdded, this, [this](Vehicle* vehicle){
            InitWeaponInfoListAdded(vehicle);
        });

        connect(QGCApplication::multiVehicleManager(), &MultiVehicleManager::vehicleRemoved, this, [this](Vehicle* vehicle){
            InitWeaponInfoListRemoved(vehicle);
        });
    }

}

UAVFlyManage_MavlinkTaskManageDialog::~UAVFlyManage_MavlinkTaskManageDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkTaskManageDialog::switchTocurrentTask(const QString& strTask)
{
	ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->setCurrentIndex(0);
}

void UAVFlyManage_MavlinkTaskManageDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    this->m_TaskManageLineEditStates = TaskManageLineEditStates::TaskManageLinePickNone;

    if(QGCApplication::createVehicleMissionLineFromTaskEnabled()){
        InitTaskListInfo();
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::InitTaskListInfo()
{

    if(ui == nullptr){
        return ;
    }

    ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->disconnect();
    ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->clear();
    qnzkna::TaskManage::INebulaTaskManageService* pINebulaTaskManageService = IUAVFlyControl_MavlinkActivator::Get().getPTaskManageService();
    QStringList taskNameList;
    if (nullptr != pINebulaTaskManageService)
    {
        auto TaskListInfo = pINebulaTaskManageService->GetActivatingMissions();
        for (auto info : TaskListInfo)
        {
            taskNameList << QString::fromStdString(info.GetID());
        }
    }
    ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->addItems(taskNameList);

    connect(ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task, &QComboBox::currentTextChanged, this, &UAVFlyManage_MavlinkTaskManageDialog::on_comboBox_UAVFlyManage_MavlinkTaskManage_Task_currentChanged);

    bool findConfigSetting = false;
    if(taskNameList.size() > 0 && nullptr != pINebulaTaskManageService)
    {
        QString strActivateID("");
        QString strActivateMissionID("");

        QSettings settings("../data/conf/TaskManageMavConfigSetting.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));
        settings.beginGroup("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVAT_MISSION_SETTINGS");

        if(settings.contains("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEID_SETTINGS") && settings.value("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEID_SETTINGS").canConvert(QVariant::Type::String)){
            strActivateID    = settings.value("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEID_SETTINGS", "").toString();
        }

        if(settings.contains("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS") && settings.value("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS").canConvert(QVariant::Type::String)){
            strActivateMissionID = settings.value("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS", "").toString();
        }

        settings.setValue("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEID_SETTINGS",        strActivateID);
        settings.setValue("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS", strActivateMissionID);
        settings.endGroup();

        QString strSelectedActivatingActionIDLocal = QString::number(pINebulaTaskManageService->GetSelectedActivatingActionIDLocal());
        if(!strActivateID.isEmpty() && strActivateID == strSelectedActivatingActionIDLocal){
            int findTextIndex = ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->findText(strActivateMissionID);
            if(findTextIndex > 0){
                findConfigSetting = true;
                ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->setCurrentIndex(findTextIndex);
            }
        }
    }

    if(!findConfigSetting){
        ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->setCurrentIndex(0);
    }

    if(ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->currentIndex() == 0){
        on_comboBox_UAVFlyManage_MavlinkTaskManage_Task_currentChanged(ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->currentText());
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    stopMissionPlan();
}

void UAVFlyManage_MavlinkTaskManageDialog::InitWeaponInfoList()
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    m_UAVFlyManage_MavlinkMissionManagerDialogList.clear();
    ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->clear();

	//隐藏控件
	ui->label->hide();
	ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->hide();
	ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->hide();
	ui->label_UAVFlyManage_MavlinkTaskManage_VehiclesInfo->hide();
	ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll->hide();
	ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesRemoveAll->hide();
	ui->pushButton_UAVFlyManage_MavlinkTaskManage_UploadAllWayPoints->hide();
	ui->pushButton_UAVFlyManage_MavlinkTaskManage_DownloadAllWayPoints->hide();
	ui->pushButton_UAVFlyManage_MavlinkTaskManage_SaveAllPoint->hide();
	ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Save->hide();
	ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Open->hide();

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
    for(int i = 0; i < tabWidgetCount; i++){
        QWidget *widget = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i);
        UAVFlyManage_MavlinkMissionManagerDialog *pUAVFlyManagpe_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(widget);
        if(pUAVFlyManagpe_MavlinkMissionManagerDialog){
            pUAVFlyManagpe_MavlinkMissionManagerDialog->clearMissionItems();
        }
        ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->removeTab(0);
        if(widget){
            widget->deleteLater();
        }
        qDebug() << __FILE__ << __LINE__ << i << tabWidgetCount;
    }

    if(!MavlinkApp()){
        return ;
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){
        return ;
    }
    auto* const pZoneManageService = IUAVFlyControl_MavlinkActivator::Get().getZoneManageService();
    if (nullptr == pZoneManageService)
        return;

	auto allLine = pZoneManageService->GetAllAirTrafficLine();
	if (allLine.size() == 0) {
		return;
	}

	const auto vehiclesCount = vehiclesList->count();
	for (int i = 0; i < vehiclesCount; i++)
	{
		Vehicle * vehicle = vehiclesList->at(i);
		if (!vehicle) {
			return;
		}

		auto vehicleID = vehicle->id();
		auto vehicleWeaponId = vehicle->weaponId();

		auto missionLineColor = m_Color[i % (sizeof(m_Color) / sizeof(m_Color[0]))];

		QGCApplication::updateMissionLineColor(vehicleWeaponId, vehicleID, missionLineColor);
		vehicle->setColor(missionLineColor);

		UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog
			= new UAVFlyManage_MavlinkMissionManagerDialog(
				ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles,
				vehicle,
				vehicleID,
				missionLineColor);
		QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);
		pUAVFlyManage_MavlinkMissionManagerDialog->setProperty(UAVFLYMANAGE_MAVLINKMISSIONMANAGERDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID, userData);
		pUAVFlyManage_MavlinkMissionManagerDialog->setMissionLineShow(true);
		CAirTrafficLine TrafficLine;
		for (auto& route = allLine.rbegin();route != allLine.rend(); ++route)
		{
			char* endPtr;
			long long strUAVID = std::strtoll(route->second.strUAVID, &endPtr, 10);
			if (strUAVID == vehicleWeaponId && route->second.LinePointVec.size() > 2)
			{
				CAirTrafficLinePoint stTakeoff = route->second.LinePointVec[0];
				CAirTrafficLinePoint stReturn = route->second.LinePointVec[route->second.LinePointVec.size() - 1];
				route->second.LinePointVec.insert(route->second.LinePointVec.begin(), stTakeoff);
				route->second.LinePointVec.push_back(stReturn);
				TrafficLine = route->second;
				break;
			}
		}

		for (const auto &poi : TrafficLine.LinePointVec) {
			pUAVFlyManage_MavlinkMissionManagerDialog->appendSimpleMissionItemWayPoint(poi.dLat, poi.dLon, poi.dAlt);
		}

		QString strLineName = "";
		const QString& strVehicleName = QGCApplication::vehicleName(vehicleWeaponId, vehicleID);
		strLineName += strVehicleName;
		strLineName += ":";
		strLineName += QString::fromLocal8Bit(TrafficLine.strName);
		ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->addTab(pUAVFlyManage_MavlinkMissionManagerDialog, strLineName);
		m_UAVFlyManage_MavlinkMissionManagerDialogList.append(pUAVFlyManage_MavlinkMissionManagerDialog);
		pUAVFlyManage_MavlinkMissionManagerDialog->setColor(missionLineColor);

	}
}

void UAVFlyManage_MavlinkTaskManageDialog::InitWeaponInfoListNoTask()
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    m_UAVFlyManage_MavlinkMissionManagerDialogList.clear();
    ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->clear();

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
    for(int i = 0; i < tabWidgetCount; i++){
        QWidget *widget = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i);
        UAVFlyManage_MavlinkMissionManagerDialog *pUAVFlyManagpe_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(widget);
        if(pUAVFlyManagpe_MavlinkMissionManagerDialog){
            pUAVFlyManagpe_MavlinkMissionManagerDialog->clearMissionItems();
        }
        ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->removeTab(0);
        if(widget){
            widget->deleteLater();
        }
        qDebug() << __FILE__ << __LINE__ << i << tabWidgetCount;
    }

    if(!MavlinkApp()){
        return ;
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){
        return ;
    }

    const auto vehiclesCount = vehiclesList->count();
    for (int i = 0; i < vehiclesCount; i++) {
        Vehicle * vehicle = vehiclesList->at(i);
        if(!vehicle){
            continue ;
        }

        const auto vehicleID       = vehicle->id();
        const auto vehicleWeaponId = vehicle->weaponId();

        QString strLineName = "";

        const QString& strVehicleName = QGCApplication::vehicleName(vehicleWeaponId, vehicleID);
        strLineName += strVehicleName;
        QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);
        ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->addItem(strLineName, userData);

        auto missionLineColor = m_Color[i % (sizeof(m_Color) / sizeof(m_Color[0]))];
        QGCApplication::updateMissionLineColor(vehicleWeaponId, vehicleID, missionLineColor);
        vehicle->setColor(missionLineColor);

        UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog
            = new UAVFlyManage_MavlinkMissionManagerDialog(
                ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles,
                vehicle,
                vehicleID,
                missionLineColor);
        pUAVFlyManage_MavlinkMissionManagerDialog->setProperty(UAVFLYMANAGE_MAVLINKMISSIONMANAGERDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID, userData);

        ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->addTab(
            pUAVFlyManage_MavlinkMissionManagerDialog,
            strLineName);
        m_UAVFlyManage_MavlinkMissionManagerDialogList.append(pUAVFlyManage_MavlinkMissionManagerDialog);

        pUAVFlyManage_MavlinkMissionManagerDialog->setColor(missionLineColor);

        qDebug() << __FILE__ << __LINE__ << vehiclesCount << " " << vehicleID << " " << vehicleWeaponId << " " << i << " " << sizeof(m_Color) << " " << sizeof(m_Color[0]) << missionLineColor;
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::InitWeaponInfoListAdded(Vehicle* vehicleAdded)
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!MavlinkApp()){
        return ;
    }

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
    for(int i = 0 ; i < tabWidgetCount; i++){
        UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
        if(widget && widget->getVehicle() == vehicleAdded){
            return ;
        }
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){
        return ;
    }

    QList<Vehicle *> vehiclesListTemp(*vehiclesList);

    std::sort(vehiclesListTemp.begin(), vehiclesListTemp.end(),
              [](const auto* vehicle1, const auto* vehicle2) {
        return (vehicle1->weaponId() < vehicle2->weaponId());
    } );

    const auto vehiclesCount = vehiclesListTemp.count();
    for (int i = 0; i < vehiclesCount; i++) {
        Vehicle * vehicle = vehiclesListTemp.at(i);
        if(!vehicle){
            continue ;
        }

        if(vehicle != vehicleAdded){
            continue ;
        }

        const auto vehicleID       = vehicle->id();
        const auto vehicleWeaponId = vehicle->weaponId();

        const QString& strVehicleName = QGCApplication::vehicleName(vehicleWeaponId, vehicleID);
        QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);
        ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->insertItem(i, strVehicleName, userData);

        auto missionLineColor = m_Color[(vehiclesCount - 1) % (sizeof(m_Color) / sizeof(m_Color[0]))];
        QGCApplication::updateMissionLineColor(vehicleWeaponId, vehicleID, missionLineColor);
        vehicle->setColor(missionLineColor);

        UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog
                = new UAVFlyManage_MavlinkMissionManagerDialog(
                    ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles,
                    vehicle,
                    vehicleID,
                    missionLineColor);
        pUAVFlyManage_MavlinkMissionManagerDialog->setProperty(UAVFLYMANAGE_MAVLINKMISSIONMANAGERDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID, userData);

        ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->insertTab(i,
                                                                         pUAVFlyManage_MavlinkMissionManagerDialog,
                                                                         strVehicleName);
        m_UAVFlyManage_MavlinkMissionManagerDialogList.insert(i, pUAVFlyManage_MavlinkMissionManagerDialog);

        pUAVFlyManage_MavlinkMissionManagerDialog->setColor(missionLineColor);

        qDebug() << __FILE__ << __LINE__ << vehiclesCount << " " << vehicleID << " " << vehicleWeaponId << " " << i << " " << sizeof(m_Color) << " " << sizeof(m_Color[0]) << missionLineColor;

        break;
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::InitWeaponInfoListRemoved(Vehicle* vehicleRemoved)
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){
        return ;
    }

    const auto vehiclesCount = vehiclesList->count();

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
    for(int i = 0 ; i < tabWidgetCount; i++){
        UAVFlyManage_MavlinkMissionManagerDialog *widget_Vehicles = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
        if(widget_Vehicles && widget_Vehicles->getVehicle() == vehicleRemoved){
            QWidget *widget = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i);
            UAVFlyManage_MavlinkMissionManagerDialog *pUAVFlyManagpe_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(widget);
            if(pUAVFlyManagpe_MavlinkMissionManagerDialog){
                pUAVFlyManagpe_MavlinkMissionManagerDialog->clearMissionItems();
            }
            ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->removeTab(i);
            if(widget){
                widget->deleteLater();
            }

            for(int comboxIndex = 0; comboxIndex < ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->count(); comboxIndex++){
                const auto vehicleID       = widget_Vehicles->getVehicle()->id();
                const auto vehicleWeaponId = widget_Vehicles->getVehicle()->weaponId();
                QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);
                if(ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->itemData(comboxIndex).toString() == userData){
                    ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->removeItem(comboxIndex);
                    break;
                }
            }

            qDebug() << __FILE__ << __LINE__ << vehiclesCount << " " << widget_Vehicles->getVehicle()->id() << " " << widget_Vehicles->getVehicle()->weaponId();

            m_UAVFlyManage_MavlinkMissionManagerDialogList.removeOne(widget_Vehicles);

            break;
        }
    }
}
void UAVFlyManage_MavlinkTaskManageDialog::AutoScriptToRoute()
{
	emit m_actionMenuStart->triggered();
	emit m_actionMenuUpdate->triggered();
}
void UAVFlyManage_MavlinkTaskManageDialog::AutoScriptToUpDown()
{
	b_CloseDialog = true;
	emit m_actionMenuUpdown->triggered();
}

void UAVFlyManage_MavlinkTaskManageDialog::startMissionPlan()
{
    this->m_TaskManageLineEditStates = TaskManageLineEditStates::TaskManageLinePickLinePoint;

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
    for(int i = 0 ; i < tabWidgetCount; i++){
        UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
        if(widget){
            widget->startMissionPlan();
        }
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll->setEnabled(true);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesRemoveAll){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesRemoveAll->setEnabled(true);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_UploadAllWayPoints){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_UploadAllWayPoints->setEnabled(true);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_DownloadAllWayPoints){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_DownloadAllWayPoints->setEnabled(true);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Save){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Save->setEnabled(true);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Open){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Open->setEnabled(true);
    }

    for(auto* action : m_TabWidgetMenu->actions()){
        if(action){
            action->setEnabled(true);
        }
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::stopMissionPlan()
{
    this->m_TaskManageLineEditStates = TaskManageLineEditStates::TaskManageLinePickNone;

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
    for(int i = 0 ; i < tabWidgetCount; i++){
        UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
        if(widget){
            widget->stopMissionPlan();
        }
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll->setEnabled(false);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesRemoveAll){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesRemoveAll->setEnabled(false);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_UploadAllWayPoints){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_UploadAllWayPoints->setEnabled(false);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_DownloadAllWayPoints){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_DownloadAllWayPoints->setEnabled(false);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Save){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Save->setEnabled(false);
    }

    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Open){
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Open->setEnabled(false);
    }

    QList<QAction*> actions = m_TabWidgetMenu->actions();
    for(int i = 0 ; i < actions.size(); i++){
        auto* action = actions.at(i);
        if(action && i != 0){
            action->setEnabled(false);
        }
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::DrawWayPointImage(QPainter &painter, const QPoint& pos, const bool Selected)
{
    if (Selected == false)
    {
        painter.drawPixmap(QPointF(pos.x() - 9, pos.y() - 21), m_WayPointImage);
    }
    else
    {
        painter.drawPixmap(QPointF(pos.x() - 7, pos.y() - 21), m_WayPointImageSelected);
    }
}

bool UAVFlyManage_MavlinkTaskManageDialog::DrawDynamic(QPainter &painter, IProj *pProj)
{
    for(auto* missionManagerDialogList : m_UAVFlyManage_MavlinkMissionManagerDialogList)
    {
        if(!missionManagerDialogList->getVehicle()){

        }

        missionManagerDialogList->DrawDynamic(painter, pProj);
    }
    return false;
}

bool UAVFlyManage_MavlinkTaskManageDialog::OnMouseButtonDown(QMouseEvent *e, IProj *pProj)
{
    if (e->button() == Qt::RightButton || !pProj) {
        return false;
    }

    if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {

        if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
        if(!pUAVFlyManage_MavlinkMissionManagerDialog){
            return false;
        }

        pUAVFlyManage_MavlinkMissionManagerDialog->OnMouseButtonDown(e, pProj);
    }
    return false;
}

bool UAVFlyManage_MavlinkTaskManageDialog::OnMouseButtonRelease(QMouseEvent *e, IProj *pProj)
{
    if (e->button() == Qt::RightButton || !pProj) {
        return false;
    }

    if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {

        if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
        if(!pUAVFlyManage_MavlinkMissionManagerDialog){
            return false;
        }

        pUAVFlyManage_MavlinkMissionManagerDialog->setSequenceNumberSelected(-1);
    }
    return false;
}

bool UAVFlyManage_MavlinkTaskManageDialog::OnMouseDoubleClick(QMouseEvent *e, IProj *pProj)
{
    if (e->button() == Qt::RightButton || !pProj) {
        return false;
    }

    if(m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickSinglePoint){
        if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
        if(!pUAVFlyManage_MavlinkMissionManagerDialog){
            return false;
        }
        pUAVFlyManage_MavlinkMissionManagerDialog->setSelected(true);

        double dLon = 0, dLat = 0;
        const auto& pt = e->pos();
        pProj->Screen2Jwd(pt, dLon, dLat);

        MissionItem *missionItem = pUAVFlyManage_MavlinkMissionManagerDialog->getSelecteMissionItem();
        if(missionItem){
            pUAVFlyManage_MavlinkMissionManagerDialog->insertSimpleMissionItemWayPoint(missionItem->sequenceNumber(), dLat, dLon, m_defaultAltitude);
        }

        m_TaskManageLineEditStates = TaskManageLineEditStates::TaskManageLinePickLinePoint;
    }
    else if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {

        if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
        if(!pUAVFlyManage_MavlinkMissionManagerDialog){
            return false;
        }

        pUAVFlyManage_MavlinkMissionManagerDialog->setSelected(true);

        double dLon = 0, dLat = 0;
        const auto& pt = e->pos();
        pProj->Screen2Jwd(pt, dLon, dLat);
        pUAVFlyManage_MavlinkMissionManagerDialog->appendSimpleMissionItemWayPoint(dLat, dLon, m_defaultAltitude);
    }
    return false;
}

bool UAVFlyManage_MavlinkTaskManageDialog::OnMouseMove(QMouseEvent *e, IProj *pProj)
{
    if (e->button() == Qt::RightButton || !pProj) {
        return false;
    }

    if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {

        if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return false;
        }

        UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
        if(!pUAVFlyManage_MavlinkMissionManagerDialog){
            return false;
        }

        pUAVFlyManage_MavlinkMissionManagerDialog->OnMouseMove(e, pProj);
    }
    return false;
}

void UAVFlyManage_MavlinkTaskManageDialog::changeLineUpWidgetToStr_like(QString strName)
{
	int count = ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->count();
	int willIndex = -1;
	for (int i = 0; i < count; i++)
	{
		QString indexStr = ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->itemText(i);
		if (indexStr.indexOf(strName) != -1)
		{
			willIndex = i;
		}
	}
	if (ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->currentIndex() == willIndex)
	{
		return;
	}
	else
	{
		ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->setCurrentIndex(willIndex);
	}
}

bool UAVFlyManage_MavlinkTaskManageDialog::GetUAVLinkMissionManager()
{
	if(m_UAVFlyManage_MavlinkMissionManagerDialogList.size() <=0)
		return false;
	auto dialog = m_UAVFlyManage_MavlinkMissionManagerDialogList.first();
	int value = dialog->GetProgressBarValue();
	if (value == 100)
		return true;
	return false;
}
void UAVFlyManage_MavlinkTaskManageDialog::GetUAVLinkMissionHide()
{
	if (m_UAVFlyManage_MavlinkMissionManagerDialogList.size() <= 0)
		return;
	auto dialog = m_UAVFlyManage_MavlinkMissionManagerDialogList.first();
	dialog->AutoScriptToClose();
}

void UAVFlyManage_MavlinkTaskManageDialog::on_tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles_customContextMenuRequested(const QPoint &pos)
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(m_TabWidgetMenu){
        m_TabWidgetMenu->popup(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->mapToGlobal(pos));
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::on_tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles_currentChanged(int index)
{
    if(index < 0){
        return ;
    }

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    const auto& tabWidgeCurrentIndex = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentIndex();
    if(tabWidgeCurrentIndex < 0 || tabWidgeCurrentIndex >= ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count()){
        return ;
    }

    ui->label_UAVFlyManage_MavlinkTaskManage_VehiclesInfo->setText("");

    auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
    if(!tabWidgeCurrentWidget){
        return ;
    }

    if(m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint){
        tabWidgeCurrentWidget->startMissionPlan();
    }else{
        tabWidgeCurrentWidget->stopMissionPlan();
    }

    const QString& strVehicleWeaponId_VehicleID = tabWidgeCurrentWidget->property(UAVFLYMANAGE_MAVLINKMISSIONMANAGERDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID).toString();
    if(strVehicleWeaponId_VehicleID.isEmpty()){
        return ;
    }

    tabWidgeCurrentWidget->setSelected(true);
    ui->label_UAVFlyManage_MavlinkTaskManage_VehiclesInfo->setText(tabWidgeCurrentWidget->getVehicleInfo());

    const auto& tabWidgetCurrentText = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->tabText(tabWidgeCurrentIndex);
    const auto& curIndex = ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->findText(tabWidgetCurrentText);
    if(curIndex < 0 || curIndex >= ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->count()){
        return ;
    }

    const auto& curDataIndex = ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->findData(strVehicleWeaponId_VehicleID);
    if(curDataIndex < 0 || curDataIndex >= ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->count()){
        return ;
    }

    if(curDataIndex != curIndex){
        LyMessageBox::warning(nullptr, tr("Tip"), tr("Task Manage : Vehicle id and name of tab and combox do not match, the configuration is error!"), QMessageBox::Yes);
        return ;
    }

    ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->setCurrentIndex(curIndex);
}

void UAVFlyManage_MavlinkTaskManageDialog::on_comboBox_UAVFlyManage_MavlinkTaskManage_Task_currentChanged(const QString & arg1)
{
    Q_UNUSED(arg1);
    if (!ui) {
        return;
    }

    if (!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task) {
        return;
    }

    if (!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles) {
        return;
    }

    QString taskID = ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->currentText();
    ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->clear();

    auto* vehiclesList = QGCApplication::vehicles();
    if (!vehiclesList) {
        return;
    }
    qnzkna::TaskManage::INebulaTaskManageService *pINebulaTaskManageService = IUAVFlyControl_MavlinkActivator::Get().getPTaskManageService();
    if (nullptr == pINebulaTaskManageService)
		return;
	m_MapVehicle.clear();
	const auto vehiclesCount = vehiclesList->count();
	for (int i = 0; i < vehiclesCount; i++) {
		Vehicle * vehicle = vehiclesList->at(i);
		if (!vehicle) {
			continue;
		}

		const auto vehicleID = vehicle->id();
		const auto vehicleWeaponId = vehicle->weaponId();
        std::string platFormMatch = pINebulaTaskManageService->readPlatFormMatchZBXH(to_string(vehicleWeaponId), taskID.toStdString());
		if (platFormMatch.empty())
			continue;
		const QString& strVehicleName = QGCApplication::vehicleName(vehicleWeaponId, vehicleID);
		m_MapVehicle[vehicleWeaponId] = strVehicleName;
	}

	for (auto vehicle : m_MapVehicle)
		ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->addItem(vehicle.second);
	InitWeaponInfoList();

    auto allMissionLine = pINebulaTaskManageService->GetMissionLinesByTask(taskID.toStdString());
	int count = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
	for (int i = 0; i < count; i++)
	{
		if (i >= allMissionLine.size())
			break;

		QWidget* tabPage = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i);
		if (nullptr == tabPage)
			continue;

		UAVFlyManage_MavlinkMissionManagerDialog* pUAVFlyManage_MavlinkMissionManagerDialog = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(tabPage);
		if (!pUAVFlyManage_MavlinkMissionManagerDialog) {
			continue;
		}
        for(auto info : allMissionLine[i].LinePointVec){
            pUAVFlyManage_MavlinkMissionManagerDialog->appendSimpleMissionItemWayPoint(info.dLat, info.dLon, info.dAlt);
        }

        QList<MissionItem *> listMissionItem = pUAVFlyManage_MavlinkMissionManagerDialog->getMissionItems();
        if(allMissionLine[i].LinePointVec.size() == listMissionItem.size()){
            for(int index = 0; index < listMissionItem.size(); index++){
                const auto& pointInfo  = allMissionLine[i].LinePointVec[index];
                auto missionItem = listMissionItem.at(index);

                if(pointInfo.cType == 49 && missionItem->command() == MAV_CMD_NAV_WAYPOINT){

                    missionItem->setParam1(pointInfo.dHoverTime);
                    pUAVFlyManage_MavlinkMissionManagerDialog->updateSimpleMissionItemWayPoint(index);
                }else if(pointInfo.cType == 50){

                }else if(pointInfo.cType == 51){

                    missionItem->setCommand(MAV_CMD_NAV_RETURN_TO_LAUNCH);
                    pUAVFlyManage_MavlinkMissionManagerDialog->updateSimpleMissionItemWayPoint(index);
                }
            }
        }
	}

    if(pINebulaTaskManageService != nullptr)
    {
        QString strActivateID = QString::number(pINebulaTaskManageService->GetSelectedActivatingActionIDLocal());
        QString strActivateMissionID(arg1);

        QSettings settings("../data/conf/TaskManageMavConfigSetting.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));
        settings.beginGroup("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVAT_MISSION_SETTINGS");
        settings.setValue("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEID_SETTINGS", strActivateID);
        settings.setValue("LY_TASKMANAGEMAV_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS", strActivateMissionID);
        settings.endGroup();
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::on_comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->label_UAVFlyManage_MavlinkTaskManage_VehiclesInfo){
        return ;
    }

    ui->label_UAVFlyManage_MavlinkTaskManage_VehiclesInfo->setText("");

    const auto& curText = ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->currentText();
    if(curText.isEmpty()){
        return ;
    }

    const auto& curData = ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles->currentData().toString();
    if(curData.isEmpty()){
        return ;
    }

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
    if(tabWidgetCount <= 0){
        return ;
    }
    bool findTabWidget = false;
    for(int i = 0 ; i < tabWidgetCount; i++){
        const auto& tabWidgetText = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->tabText(i);
        auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
        if(tabWidgeCurrentWidget && tabWidgetText == curText){
            const QString& strVehicleWeaponId_VehicleID = tabWidgeCurrentWidget->property(UAVFLYMANAGE_MAVLINKMISSIONMANAGERDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID).toString();
            if(!strVehicleWeaponId_VehicleID.isEmpty() && curData == strVehicleWeaponId_VehicleID){
                ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->setCurrentIndex(i);

                auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->currentWidget());
                if(tabWidgeCurrentWidget){
                    tabWidgeCurrentWidget->setSelected(true);
                    ui->label_UAVFlyManage_MavlinkTaskManage_VehiclesInfo->setText(tabWidgeCurrentWidget->getVehicleInfo());

                    if(m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint){
                        tabWidgeCurrentWidget->startMissionPlan();
                    }else{
                        tabWidgeCurrentWidget->stopMissionPlan();
                    }

                }
                findTabWidget = true;
                break ;
            }
        }
    }

    if(!findTabWidget){
        LyMessageBox::warning(nullptr, tr("Tip"), tr("Task Manage : Vehicle id and name of combox and tab do not match, the configuration is error!"), QMessageBox::Yes);
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::on_pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Save_clicked()
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
        return ;
    }

    QString strTaskFileDir = UAVFlyManage_MavlinkMissionManagerDialog::getTaskFileDir();
    if(strTaskFileDir.isEmpty()){
        strTaskFileDir = UAVFlyManage_MavlinkMissionManagerDialog::getCreateTaskFileDir();
    }

    bool ok = false;

    const QString& textTaskFileDir = LyInputDialog::getText(this, tr("\xe4\xbf\x9d\xe5\xad\x98\xe4\xbb\xbb\xe5\x8a\xa1                                                                                 "),
                                         tr("\xe4\xbb\xbb\xe5\x8a\xa1\xe5\x90\x8d\xe7\xa7\xb0:"), QLineEdit::Normal,
                                         strTaskFileDir, &ok);
    if (ok && !textTaskFileDir.isEmpty()){
        QString errorString;
        const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
        for(int i = 0 ; i < tabWidgetCount; i++){
            UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
            if(widget){
                QString errorStringTemp(tr("\xe6\x97\xa0\xe9\x94\x99\xe8\xaf\xaf"));
                widget->saveToTaskFileDir(textTaskFileDir, errorStringTemp);
                errorString += (widget->getVehicle() != nullptr ? widget->getVehicle()->weaponTitle() : "") + errorStringTemp + "\n";
            }
        }

        LyMessageBox::warning(this, tr("\xe4\xbf\x9d\xe5\xad\x98\xe4\xbb\xbb\xe5\x8a\xa1"),
                              errorString , QMessageBox::Yes);
    }
    else if(ok){
        LyMessageBox::warning(this, tr("\xe4\xbf\x9d\xe5\xad\x98\xe4\xbb\xbb\xe5\x8a\xa1"),
                              tr("\xe4\xbb\xbb\xe5\x8a\xa1\xe5\x90\x8d\xe7\xa7\xb0\xe9\x9d\x9e\xe7\xa9\xba!"), QMessageBox::Yes);
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::on_pushButton_UAVFlyManage_MavlinkTaskManage_Vehicles_Open_clicked()
{
    if(!ui){
        return ;
    }

    UAVFlyManage_MavlinkMissionManagerOpenFileDialog* openFileDialog = new UAVFlyManage_MavlinkMissionManagerOpenFileDialog(this);
    openFileDialog->setAttribute(Qt::WA_DeleteOnClose);
    openFileDialog->show();
    openFileDialog->raise();
    connect(openFileDialog, &UAVFlyManage_MavlinkMissionManagerOpenFileDialog::accepted, [this, openFileDialog](){
        if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return ;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return ;
        }

        const QString& taskFileDir = openFileDialog->TaskFileDir();

        QString errorString;
        const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
        for(int i = 0 ; i < tabWidgetCount; i++){
            UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
            if(widget){
                QString errorStringTemp(tr("\xe6\x97\xa0\xe9\x94\x99\xe8\xaf\xaf"));
                widget->loadFromTaskFileDir(taskFileDir, errorStringTemp);
                errorString += (widget->getVehicle() != nullptr ? widget->getVehicle()->weaponTitle() : "") + errorStringTemp + "\n";
            }
        }

        LyMessageBox::warning(this, tr("\xe5\x8a\xa0\xe8\xbd\xbd\xe4\xbb\xbb\xe5\x8a\xa1\xe8\x88\xaa\xe7\x82\xb9"),
                              errorString , QMessageBox::Yes);
    });
}

void UAVFlyManage_MavlinkTaskManageDialog::on_pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll_clicked()
{

    if(!ui){
        return ;
    }

    if(!ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll){
        return ;
    }

    bool missionLineShow = false;
    if(ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll->text() == tr("\xe6\x98\xbe\xe7\xa4\xba\xe6\x89\x80\xe6\x9c\x89\xe8\x88\xaa\xe7\x82\xb9")){
        missionLineShow = true;
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll->setText(tr("\xe9\x9a\x90\xe8\x97\x8f\xe6\x89\x80\xe6\x9c\x89\xe8\x88\xaa\xe7\x82\xb9"));
    }else{
        ui->pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesHideAll->setText(tr("\xe6\x98\xbe\xe7\xa4\xba\xe6\x89\x80\xe6\x9c\x89\xe8\x88\xaa\xe7\x82\xb9"));
    }

    {

        if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return ;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
            return ;
        }

        const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
        for(int i = 0 ; i < tabWidgetCount; i++){
            UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
            if(widget){
                widget->setMissionLineShow(missionLineShow);
            }
        }
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::on_pushButton_UAVFlyManage_MavlinkTaskManage_VehiclesRemoveAll_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you remove all vehicle waypoints?"), QMessageBox::Yes | QMessageBox::No)) {
        if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint)
        {
            if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {

                if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
                    return ;
                }

                if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
                    return ;
                }

                const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
                for(int i = 0 ; i < tabWidgetCount; i++){
                    UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
                    if(widget){
                        widget->clearMissionItems();
                    }
                }
            }
        }
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::on_pushButton_UAVFlyManage_MavlinkTaskManage_UploadAllWayPoints_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you upload all vehicle waypoints?"), QMessageBox::Yes | QMessageBox::No)) {
        if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint)
        {
            if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {

                if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
                    return ;
                }

                if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
                    return ;
                }

                const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
                for(int i = 0 ; i < tabWidgetCount; i++){
                    UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
                    if(widget){
                        widget->sendToVehicle();
                    }
                }
            }
        }
    }
}

void UAVFlyManage_MavlinkTaskManageDialog::on_pushButton_UAVFlyManage_MavlinkTaskManage_DownloadAllWayPoints_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you download all vehicle waypoints?"), QMessageBox::Yes | QMessageBox::No)) {
        if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint)
        {
            if (m_TaskManageLineEditStates == TaskManageLineEditStates::TaskManageLinePickLinePoint) {

                if(!ui->comboBox_UAVFlyManage_MavlinkTaskManage_Vehicles){
                    return ;
                }

                if(!ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles){
                    return ;
                }

                const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
                for(int i = 0 ; i < tabWidgetCount; i++){
                    UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
                    if(widget){
                        widget->loadFromVehicle();
                    }
                }
            }
        }
    }
}
void UAVFlyManage_MavlinkTaskManageDialog::on_pushButton_UAVFlyManage_MavlinkTaskManage_SaveAllPoint_clicked()
{
    QString strTaskID = ui->comboBox_UAVFlyManage_MavlinkTaskManage_Task->currentText();
    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->count();
    auto* const pINebulaTaskManageService = IUAVFlyControl_MavlinkActivator::Get().getPTaskManageService();
    if (nullptr == pINebulaTaskManageService)
        return;
    auto TaskLineList = pINebulaTaskManageService->GetMissionLinesByTask(strTaskID.toStdString());
    vector<std::string> LineNameVec;
    vector<int>         LineIDVec;
    for (auto line : TaskLineList)
    {

        LineIDVec.push_back(line.nID);
        LineNameVec.push_back(line.strName);
    }
    for (int i = 0; i < tabWidgetCount; i++)
    {
        UAVFlyManage_MavlinkMissionManagerDialog *widget = qobject_cast<UAVFlyManage_MavlinkMissionManagerDialog*>(ui->tabWidget_UAVFlyManage_MavlinkTaskManage_Vehicles->widget(i));
        if (nullptr == widget)
            continue;
        QList<MissionItem *> MissionInfo = widget->getMissionItems();

        if (i >= LineNameVec.size())
            break;
        CNeubulaMissionLine stLine;
        const std::string &LineName = LineNameVec[i];
        stLine.nID = LineIDVec[i];
        strncpy(stLine.strName, LineName.data(), MAX_LINE_LEN_1);
        strncpy(stLine.strMissionID, tr3N(strTaskID), MAX_LINE_LEN_1);
        stLine.cType = 0;
        stLine.cProtocolType = 0;
        stLine.cShowFlag = 1;
        int MaxLinePoint = 1;
        for (auto info : MissionInfo)
        {
            CNeubulaMissionLinePoint pointInfo;
            pointInfo.nNo = MaxLinePoint;
            pointInfo.nLineID = stLine.nID;
            QGeoCoordinate point = info->coordinate();
            pointInfo.dLon = point.longitude();
            pointInfo.dLat = point.latitude();
            pointInfo.dAlt = point.altitude();

            pointInfo.dSpeed = 30;
            pointInfo.dHoverTime = 0;
            pointInfo.cHeightCtrl = 1;
            pointInfo.DRadius = 20;
            pointInfo.cLineMode = 1;
            pointInfo.cTurnMode = 1;
            pointInfo.cLanding = 0;
            pointInfo.cType = 1;
            if(info->command() == MAV_CMD_NAV_WAYPOINT){
                pointInfo.cType = 49;
                pointInfo.dHoverTime = info->param1();
            }else if(info->command() == MAV_CMD_NAV_TAKEOFF || info->command() == MAV_CMD_NAV_VTOL_TAKEOFF){
                pointInfo.cType = 50;
            }else if(info->command() == MAV_CMD_NAV_RETURN_TO_LAUNCH){
                pointInfo.cType = 51;
            }

            stLine.LinePointVec.push_back(pointInfo);
            MaxLinePoint++;
        }

        bool bresult = pINebulaTaskManageService->EditMissionLine(stLine);
        if (!bresult)
        {

            LyMessageBox::information(this, "\xe6\x8f\x90\xe7\xa4\xba", "\xe4\xbf\x9d\xe5\xad\x98\xe5\xa4\xb1\xe8\xb4\xa5!");
            return;
        }
    }

    LyMessageBox::information(this, "\xe6\x8f\x90\xe7\xa4\xba", "\xe4\xbf\x9d\xe5\xad\x98\xe6\x88\x90\xe5\x8a\x9f!");
}
