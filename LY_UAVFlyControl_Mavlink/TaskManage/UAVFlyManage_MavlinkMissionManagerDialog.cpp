#include "UAVFlyManage_MavlinkMissionManagerDialog.h"
#include "ui_UAVFlyManage_MavlinkMissionManagerDialog.h"

#include "LY_UAVFlyControl_Mavlink.h"

#include "lyUI/LyMessageBox.h"

#include <QComboBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QUuid>
#include <limits>

#include "IProj.h"
#include "CreateShape.h"
#include "GetHeight.h"
#include "MavlinkMissionManagerMissionCommandList.h"

#include "QGCApplication.h"
#include "JsonHelper.h"
#include "MissionCommandUIInfo.h"
#include "QGCLoggingCategory.h"
#include "MissionItem.h"
#include "SimpleMissionItem.h"
#include "MissionCommandTree.h"
#include "FirmwarePlugin.h"
#include "MissionManager.h"
#include "DataManage/SystemInfo/SystemInternalInterface.h"
#include "LY_UAVFlyControl_MavlinkInterface.h"
#include "GLES3/gl3.h"

#define  LINE_LENGTH              3
#define  LINE_DEFAULT_ALTITUDE    20

unsigned int pointSelectSize = LINE_LENGTH;

static CGetHeight* s_pCGetHeight = nullptr;

QString     UAVFlyManage_MavlinkMissionManagerDialog::m_TaskFileDir =                "";
const int   UAVFlyManage_MavlinkMissionManagerDialog::_kPlanFileVersion =            1;
const char* UAVFlyManage_MavlinkMissionManagerDialog::_kPlanFileType =               "Plan";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_kJsonMissionObjectKey =       "mission";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_kJsonGeoFenceObjectKey =      "geoFence";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_kJsonRallyPointsObjectKey =   "rallyPoints";

const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonFileTypeValue =             "Mission";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonItemsKey =                  "items";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonPlannedHomePositionKey =    "plannedHomePosition";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonFirmwareTypeKey =           "firmwareType";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonVehicleTypeKey =            "vehicleType";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonCruiseSpeedKey =            "cruiseSpeed";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonHoverSpeedKey =             "hoverSpeed";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonParamsKey =                 "params";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonGlobalPlanAltitudeModeKey = "globalPlanAltitudeMode";

const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonComplexItemsKey =           "complexItems";
const char* UAVFlyManage_MavlinkMissionManagerDialog::_jsonMavAutopilotKey =           "MAV_AUTOPILOT";

const int   UAVFlyManage_MavlinkMissionManagerDialog::_missionFileVersion  =            2;

ILabel *    UAVFlyManage_MavlinkMissionManagerDialog::_pLabelMap3D         =            nullptr;

QGC_LOGGING_CATEGORY(MavlinkMissionManagerDialog, "MavlinkMissionManagerDialog")

UAVFlyManage_MavlinkMissionManagerDialog::UAVFlyManage_MavlinkMissionManagerDialog( QWidget *parent, Vehicle *vehicle, int vehicleId, unsigned int color)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_MavlinkMissionManagerDialog)
    , m_MavlinkMissionManagerMissionCommandList(nullptr)
    , m_vehicle(vehicle)
    , m_vehicleId(vehicleId)
    , m_sendHomePositionToVehicle(false)
    , m_homePosition()
    , m_MissionLineShow(true)
    , m_Selected(false)
    , m_Color(color)
    , m_SequenceNumberSelected(-1)
    , m_MavFrameDefault(MAV_FRAME::MAV_FRAME_GLOBAL_RELATIVE_ALT)
    , m_FlightTaskManageEditLineElevation(false)
{
    ui->setupUi(this);

    m_MavlinkMissionManagerMissionCommandList = new MavlinkMissionManagerMissionCommandList(":/taskmanager/TaskManageMavCmdInfoCommon.json", true);

    ui->doubleSpinBox_VehicleUpDownMissionWayPointAltitude->setValue(QGCApplication::defaultMissionItemAltitude());

    if(m_vehicle){
        m_homePosition = m_vehicle->homePosition();
        connect(m_vehicle, &Vehicle::homePositionChanged, this, [this](){
            m_homePosition = m_vehicle->homePosition();
        });

        if(m_vehicle->firmwarePlugin()){
            m_sendHomePositionToVehicle = m_vehicle->firmwarePlugin()->sendHomePositionToVehicle();
        }
    }

    double flightSpeed = 0;
    if (m_vehicle && m_vehicle->multiRotor()) {
        flightSpeed = m_vehicle->defaultHoverSpeed();
    } else {
        flightSpeed = m_vehicle->defaultCruiseSpeed();
    }
    ui->doubleSpinBox_VehicleUpDownMissionWayPointSpeed->setValue(flightSpeed);

    ui->groupBox_VehicleUpDownMissionLoiterRadius->setVisible(false);
    ui->groupBox_VehicleUpDownMissionLoiterRadius->hide();

    ui->groupBox_VehicleUpDownMissionWayPointRadius->setVisible(false);
    ui->groupBox_VehicleUpDownMissionWayPointRadius->hide();

    ui->groupBox_VehicleUpDownMissionWayPointSpeed->setVisible(false);
    ui->groupBox_VehicleUpDownMissionWayPointSpeed->hide();

    m_WayPointImage.load(":/images/marker.png");
    m_WayPointImageSelected.load(":/images/marker_sel.png");
    m_WayPointImage32.load(":/images/marker32.png");
    m_WayPointImage32Selected.load(":/images/marker_sel32.png");

    QString strFlightTaskManageEditLineElevationDir = "../data/dem/90";

    QSettings settings("../data/conf/FlightTaskManageSettings.ini", QSettings::IniFormat);
    settings.beginGroup("FlightTaskManageEditLineSettings");
    if(settings.contains("FlightTaskManageEditLineElevationDir")){
        QString flightTaskManageEditLineElevationDirTemp = settings.value("FlightTaskManageEditLineElevationDir", "").toString();
        if(!flightTaskManageEditLineElevationDirTemp.isEmpty()){
            if(QDir(flightTaskManageEditLineElevationDirTemp).exists()){
                strFlightTaskManageEditLineElevationDir = flightTaskManageEditLineElevationDirTemp;
            }
        }
    }

    if(settings.contains("FlightTaskManageEditLineElevation")){
        m_FlightTaskManageEditLineElevation = settings.value("FlightTaskManageEditLineElevation", false).toBool();
    }
    settings.endGroup();

    s_pCGetHeight = new CGetHeight (strFlightTaskManageEditLineElevationDir.toStdString().c_str());

    if(m_vehicle && m_vehicle->missionManager()){
        auto* vehicleMissionManager = m_vehicle->missionManager();
        connect(vehicleMissionManager, &MissionManager::newMissionItemsAvailable,       this, &UAVFlyManage_MavlinkMissionManagerDialog::_newMissionItemsAvailableFromVehicle);
        connect(vehicleMissionManager, &MissionManager::sendComplete,                   this, &UAVFlyManage_MavlinkMissionManagerDialog::_managerSendComplete);
        connect(vehicleMissionManager, &MissionManager::removeAllComplete,              this, &UAVFlyManage_MavlinkMissionManagerDialog::_managerRemoveAllComplete);
        connect(vehicleMissionManager, &MissionManager::inProgressChanged,              this, &UAVFlyManage_MavlinkMissionManagerDialog::_inProgressChanged);
        connect(vehicleMissionManager, &MissionManager::progressPct,                    this, &UAVFlyManage_MavlinkMissionManagerDialog::_progressPctChanged);
        connect(vehicleMissionManager, &MissionManager::currentIndexChanged,            this, &UAVFlyManage_MavlinkMissionManagerDialog::_currentMissionIndexChanged);
        connect(vehicleMissionManager, &MissionManager::lastCurrentIndexChanged,        this, &UAVFlyManage_MavlinkMissionManagerDialog::_resumeMissionIndexChanged);
        connect(vehicleMissionManager, &MissionManager::resumeMissionReady,             this, &UAVFlyManage_MavlinkMissionManagerDialog::_resumeMissionReady);
        connect(vehicleMissionManager, &MissionManager::resumeMissionUploadFail,        this, &UAVFlyManage_MavlinkMissionManagerDialog::_resumeMissionUploadFail);
        connect(vehicleMissionManager, &MissionManager::error,                           this, &UAVFlyManage_MavlinkMissionManagerDialog::_error);
        connect(vehicleMissionManager, &MissionManager::updateMissionItemTips,          this, &UAVFlyManage_MavlinkMissionManagerDialog::_updateMissionItemTips);
        connect(vehicleMissionManager, &MissionManager::sendCurrentMissionItemIndex,    this, &UAVFlyManage_MavlinkMissionManagerDialog::_sendCurrentMissionItemIndex);
        connect(vehicleMissionManager, &MissionManager::recvCurrentMissionItemIndex,    this, &UAVFlyManage_MavlinkMissionManagerDialog::_recvCurrentMissionItemIndex);
    }

    pointSelectSize = QGCApplication::selectApplicationSettingPointSize();

    m_MissionItems.clear();

    QList<SupportMissionMavCmd> supportMavCmdList;
    if(m_vehicle && m_vehicle->firmwarePlugin()){
        MissionCommandTree* commandTree = QGCApplication::missionCommandTree();
        const QList<MAV_CMD>&  vehicleSupportedMissionCommands = m_vehicle->firmwarePlugin()->supportedMissionCommands(m_vehicle->vehicleClass());
        for (const MAV_CMD command: vehicleSupportedMissionCommands) {
            SupportMissionMavCmd cmd;
            cmd.mav_cmd              = command;
            cmd.mav_cmd_rawName      = commandTree->rawName(command);
            cmd.mav_cmd_friendlyName = commandTree->friendlyName(command);

            if(m_MavlinkMissionManagerMissionCommandListEnable && m_MavlinkMissionManagerMissionCommandList != nullptr){
                const auto* const commandUIInfo = m_MavlinkMissionManagerMissionCommandList->getUIInfo(command);
                if(commandUIInfo != nullptr){
                    if(commandUIInfo != nullptr){
                        cmd.mav_cmd_rawName      = commandUIInfo->rawName();
                        cmd.mav_cmd_friendlyName = commandUIInfo->friendlyName();
                    }
                }
            }

            qDebug() << cmd.mav_cmd_friendlyName;
            supportMavCmdList.append(cmd);
        }
    }else{
        MissionCommandTree* commandTree = QGCApplication::missionCommandTree();
        for (const MAV_CMD command: commandTree->allCommandIds()) {
            SupportMissionMavCmd cmd;
            cmd.mav_cmd              = command;
            cmd.mav_cmd_rawName      = commandTree->rawName(command);
            cmd.mav_cmd_friendlyName = commandTree->friendlyName(command);

            if(m_MavlinkMissionManagerMissionCommandListEnable && m_MavlinkMissionManagerMissionCommandList != nullptr){
                const auto* const commandUIInfo = m_MavlinkMissionManagerMissionCommandList->getUIInfo(command);
                if(commandUIInfo != nullptr){
                    if(commandUIInfo != nullptr){
                        cmd.mav_cmd_rawName      = commandUIInfo->rawName();
                        cmd.mav_cmd_friendlyName = commandUIInfo->friendlyName();
                    }
                }
            }

            supportMavCmdList.append(cmd);
        }
    }

    bool missionSupportCommandMavCmdUpdate = false;
    const auto& missionSupportCommandMavCmd  = QGCApplication::missionSupportCommandMavCmd();
    if(missionSupportCommandMavCmd.size() > 0){
        missionSupportCommandMavCmdUpdate = true;
        for (const auto& cmd: supportMavCmdList) {
            if(missionSupportCommandMavCmd.find(cmd.mav_cmd_rawName) != missionSupportCommandMavCmd.end()){
                m_SupportMavCmdList.append(cmd);
            }
        }
    }

    if(!missionSupportCommandMavCmdUpdate){
        m_SupportMavCmdList = supportMavCmdList;
    }

    const auto& missionAddSupportCommandMavCmd = QGCApplication::missionAddSupportCommandMavCmd();
    if(missionAddSupportCommandMavCmd.size() > 0){
        missionSupportCommandMavCmdUpdate = true;
        MissionCommandTree* commandTree = QGCApplication::missionCommandTree();
        for (const MAV_CMD command: commandTree->allCommandIds()) {
            SupportMissionMavCmd cmd;
            cmd.mav_cmd              = command;
            cmd.mav_cmd_rawName      = commandTree->rawName(command);
            cmd.mav_cmd_friendlyName = commandTree->friendlyName(command);

            if(m_MavlinkMissionManagerMissionCommandListEnable && m_MavlinkMissionManagerMissionCommandList != nullptr){
                const auto* const commandUIInfo = m_MavlinkMissionManagerMissionCommandList->getUIInfo(command);
                if(commandUIInfo != nullptr){
                    if(commandUIInfo != nullptr){
                        cmd.mav_cmd_rawName      = commandUIInfo->rawName();
                        cmd.mav_cmd_friendlyName = commandUIInfo->friendlyName();
                    }
                }
            }

            if(missionAddSupportCommandMavCmd.find(cmd.mav_cmd_rawName) != missionAddSupportCommandMavCmd.end()){
                m_SupportMavCmdList.append(cmd);
            }
        }
    }

    const auto& missionUnSupportCommandMavCmd = QGCApplication::missionUnSupportCommandMavCmd();
    if(missionUnSupportCommandMavCmd.size() > 0){
        for (const auto& cmd: missionUnSupportCommandMavCmd.keys()) {
            QList<SupportMissionMavCmd>::iterator iter = std::find_if(m_SupportMavCmdList.begin(), m_SupportMavCmdList.end(),
                                                                      [&cmd](const SupportMissionMavCmd& value)
            {
                return (cmd == value.mav_cmd_rawName);
            });
            if(iter != m_SupportMavCmdList.end()){
                m_SupportMavCmdList.erase(iter);
            }
        }
    }

    m_SupportMavFrameInfoList = SimpleMissionItem::MavFrameInfoList();

    const auto& missionUnSupportCommandMavFrame = QGCApplication::missionUnSupportCommandMavFrame();
    if(missionUnSupportCommandMavFrame.size() > 0){
        for (const auto& frame: missionUnSupportCommandMavFrame.keys()) {
            QList<SimpleMissionItem::EnumInfo_s>::iterator iter = std::find_if(m_SupportMavFrameInfoList.begin(), m_SupportMavFrameInfoList.end(),
                                                                               [&frame](const SimpleMissionItem::EnumInfo_s& value)
            {
                return (frame == value.label);
            });
            if(iter != m_SupportMavFrameInfoList.end()){
                m_SupportMavFrameInfoList.erase(iter);
            }
        }
    }

    if(ui->comboBox_VehicleUpDownMissionWayPointAltitudeRelative){
        for(const auto& supportMavFrameInfo : m_SupportMavFrameInfoList){
            ui->comboBox_VehicleUpDownMissionWayPointAltitudeRelative->addItem(supportMavFrameInfo.label, supportMavFrameInfo.frame);
        }
    }

    const QString strMissionWaypointMavFrameDefault = QGCApplication::missionWaypointMavFrameDefault();
    for(const auto& supportMavFrameInfo : m_SupportMavFrameInfoList){
        if(strMissionWaypointMavFrameDefault == supportMavFrameInfo.label){
            m_MavFrameDefault = supportMavFrameInfo.frame;
            if(ui->comboBox_VehicleUpDownMissionWayPointAltitudeRelative){
                ui->comboBox_VehicleUpDownMissionWayPointAltitudeRelative->setCurrentText(strMissionWaypointMavFrameDefault);
            }
            break;
        }
    }

    connect(ui->tableWidget->verticalHeader(), &QHeaderView::sectionDoubleClicked, this, [this](const int logicalIndex){
        on_tableWidget_cellDoubleClicked(logicalIndex, 0);
    });

    MissionManagerTableWidgetUpdate();

    _resetVehicleUpDownMission();

    stopMissionPlan();
}

void UAVFlyManage_MavlinkMissionManagerDialog::_newMissionItemsAvailableFromVehicle(bool removeAllRequested)
{
    qCDebug(MavlinkMissionManagerDialog) << "_newMissionItemsAvailableFromVehicle flyView:count" << m_vehicle->missionManager()->missionItems().count();

    if (removeAllRequested || _itemsRequested || m_MissionItems.size() <= 1) {

        {
            while( m_MissionItems.size() > 0){
                auto* visualItem = m_MissionItems.takeAt(0);
                if(visualItem){
                    delete visualItem;
                }
            }
        }

        const QList<MissionItem*>& newMissionItems = m_vehicle->missionManager()->missionItems();
        for (int i=0; i < newMissionItems.size(); i++) {
            m_MissionItems.append(new MissionItem(*newMissionItems[i], m_vehicle));
        }
    }
    _itemsRequested = false;

    MissionManagerTableWidgetUpdate();
}

void UAVFlyManage_MavlinkMissionManagerDialog::_managerSendComplete(bool error)
{

    qCDebug(MavlinkMissionManagerDialog) << "_managerSendComplete:"<< error;
    if (!error) {
        qCDebug(MavlinkMissionManagerDialog) << "showPlanFromManagerVehicle";
        if (!m_vehicle->initialPlanRequestComplete()) {

            qCDebug(MavlinkMissionManagerDialog) << "showPlanFromManagerVehicle: !initialPlanRequestComplete, wait for signal";
            return ;
        } else if (_syncInProgress()) {

            qCDebug(MavlinkMissionManagerDialog) << "showPlanFromManagerVehicle: syncInProgress wait for signal";
            return ;
        } else {

            qCDebug(MavlinkMissionManagerDialog) << "showPlanFromManagerVehicle: sync complete simulate signal";
            _itemsRequested = true;
            _itemsRequested = false;
            return ;
        }
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::_managerRemoveAllComplete(bool error)
{
    qCDebug(MavlinkMissionManagerDialog) << "_managerRemoveAllComplete:"<< error;
    if (!error) {

        qCDebug(MavlinkMissionManagerDialog) << "showPlanFromManagerVehicle";
        if (!m_vehicle->initialPlanRequestComplete()) {

            qCDebug(MavlinkMissionManagerDialog) << "showPlanFromManagerVehicle: !initialPlanRequestComplete, wait for signal";
            return ;
        } else if (_syncInProgress()) {

            qCDebug(MavlinkMissionManagerDialog) << "showPlanFromManagerVehicle: syncInProgress wait for signal";
            return ;
        } else {

            qCDebug(MavlinkMissionManagerDialog) << "showPlanFromManagerVehicle: sync complete simulate signal";
            _itemsRequested = true;
            _itemsRequested = false;
            return ;
        }
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::_inProgressChanged(bool inProgress)
{
    qCDebug(MavlinkMissionManagerDialog) << "_inProgressChanged:" << inProgress;
    emit syncInProgressChanged(inProgress);
}

void UAVFlyManage_MavlinkMissionManagerDialog::_progressPctChanged(double progressPct)
{
    qCDebug(MavlinkMissionManagerDialog) << "_progressPctChanged:" << progressPct;
    if (!QGC::fuzzyCompare(progressPct, _progressPct)) {
        _progressPct = progressPct;
        emit progressPctChanged(progressPct);
    }

	if (abs(progressPct - 1) < 0.00001 && m_startLoad == true)
	{
		QDateTime curdt = QDateTime::currentDateTime();
		QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
		auto lParam = new EventInfo(strcurdt, tr2("系统事件"), tr2("无人机:[%1] 检测到火源并上报").arg(QString::number(m_vehicle->weaponId())));
		LY_UAVFlyControl_MavlinkInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900300, lParam);
		m_startLoad = false;
	}

    ui->progressBar_VehicleUpDownMissionProgress->setValue(progressPct * 100);
}

void UAVFlyManage_MavlinkMissionManagerDialog::_resumeMissionIndexChanged()
{
    qCDebug(MavlinkMissionManagerDialog) << "_resumeMissionIndexChanged";
    emit resumeMissionIndexChanged();

    _updateTips(tr2("重新开始任务，任务航点序号已改变"));
}

void UAVFlyManage_MavlinkMissionManagerDialog::_resumeMissionReady()
{
    qCDebug(MavlinkMissionManagerDialog) << "_resumeMissionReady";
    emit resumeMissionReady();

    _updateTips(QString("重新开始任务已就绪"));
}

void UAVFlyManage_MavlinkMissionManagerDialog::_resumeMissionUploadFail()
{
    qCDebug(MavlinkMissionManagerDialog) << "_resumeMissionUploadFail";
    emit resumeMissionUploadFail();

    _updateTips(QString("重新开始任务上传失败"));
}

void UAVFlyManage_MavlinkMissionManagerDialog::_error(int errorCode, const QString &errorMsg)
{
    qCDebug(MavlinkMissionManagerDialog) << "ErrorCode:" << errorCode << ", ErrorMsg:"<< errorMsg;

	_updateTips(tr2("错误代码：") + QString::number(errorCode) + tr2("，错误信息：") + errorMsg);
}

void UAVFlyManage_MavlinkMissionManagerDialog::_updateTips(const QString &tips)
{
    if(!ui){
        return ;
    }

    if(!ui->label_VehicleUpDownMissionProgressInfo){
        return ;
    }

    ui->label_VehicleUpDownMissionProgressInfo->setText(tips);
}

void UAVFlyManage_MavlinkMissionManagerDialog::_updateMissionItemTips(const QString &tips)
{
    qCDebug(MavlinkMissionManagerDialog) << "UpdateMissionItemTips:" << tips;

    _updateTips(tips);
}

void UAVFlyManage_MavlinkMissionManagerDialog::_sendCurrentMissionItemIndex(int sendIndex)
{
    qCDebug(MavlinkMissionManagerDialog) << "SendCurrentMissionItemIndex:" << sendIndex;

    _updateTips(tr2("发送当前任务航点序号：") + QString::number(sendIndex));
}

void UAVFlyManage_MavlinkMissionManagerDialog::_recvCurrentMissionItemIndex(int recvIndex)
{
    qCDebug(MavlinkMissionManagerDialog) << "RecvCurrentMissionItemIndex:" << recvIndex;

    _updateTips(tr2("接收当前任务航点序号：") + QString::number(recvIndex));
}

void UAVFlyManage_MavlinkMissionManagerDialog::_resetVehicleUpDownMission()
{
    if(!ui){
        return ;
    }

    ui->label_VehicleUpDownMissionInfo_Count->setText(tr2("点数："));
    ui->label_VehicleUpDownMissionInfo_Dist->setText(tr2("距离："));
    ui->progressBar_VehicleUpDownMissionProgress->setValue(0);
    ui->label_VehicleUpDownMissionProgressInfo->setText("");
}

int UAVFlyManage_MavlinkMissionManagerDialog::GetProgressBarValue()
{
	return ui->progressBar_VehicleUpDownMissionProgress->value();
}

void UAVFlyManage_MavlinkMissionManagerDialog::AutoScriptToClose()
{
    QHideEvent *event = nullptr;
    hideEvent(event);
}

bool UAVFlyManage_MavlinkMissionManagerDialog::_isStandaloneCoordinate(MissionItem* item) const
{
    const MissionCommandUIInfo* uiInfo = QGCApplication::missionCommandTree()->getUIInfo(m_vehicle, m_vehicle->vehicleClass(), item->command());
    if (uiInfo) {
        return uiInfo->isStandaloneCoordinate();
    } else {
        return false;
    }
}

bool UAVFlyManage_MavlinkMissionManagerDialog::_specifiesCoordinate(const MissionItem* item) const
{
    const MissionCommandUIInfo* uiInfo = QGCApplication::missionCommandTree()->getUIInfo(m_vehicle, m_vehicle->vehicleClass(), item->command());
    if (uiInfo) {
        return uiInfo->specifiesCoordinate();
    } else {
        return false;
    }
}

QString UAVFlyManage_MavlinkMissionManagerDialog::_commandDescription(const MissionItem* item) const
{
    const MissionCommandUIInfo* uiInfo = QGCApplication::missionCommandTree()->getUIInfo(m_vehicle, m_vehicle->vehicleClass(), item->command());
    if (uiInfo) {
        return uiInfo->description();
    } else {
        qWarning() << "Should not ask for command description on unknown command";
        return _commandName(item);
    }
}

QString UAVFlyManage_MavlinkMissionManagerDialog::_commandToolTipDescription(const MissionItem *item) const
{
    const MissionCommandUIInfo* uiInfo = QGCApplication::missionCommandTree()->getUIInfo(m_vehicle, m_vehicle->vehicleClass(), item->command());

    if(m_MavlinkMissionManagerMissionCommandListEnable && m_MavlinkMissionManagerMissionCommandList != nullptr){
        const auto* const commandUIInfo = m_MavlinkMissionManagerMissionCommandList->getUIInfo(item->command());
        if(commandUIInfo != nullptr){
            uiInfo = commandUIInfo;
        }
    }

    if (uiInfo) {
        QString toolTipDescription = uiInfo->category();
        toolTipDescription += QString(":") + uiInfo->description();
        toolTipDescription += QString("\n") + uiInfo->friendlyName();
        toolTipDescription += QString(":") +uiInfo->rawName();

        bool showUI = true;

        for(int index = 1; index <= 7; index++){
            const MissionCmdParamInfo* paramInfo = uiInfo->getParamInfo(index, showUI);
            if(paramInfo != nullptr){
                toolTipDescription += QString("\n") + QString("param") + QString::number(paramInfo->param());
                toolTipDescription += QString("") + QString(":") + paramInfo->label();
                toolTipDescription += QString(", ") + QString("decimalPlaces:") + QString::number(paramInfo->decimalPlaces());
                toolTipDescription += QString(", ") + QString("defaultValue:") + QString::number(paramInfo->defaultValue(), 'f', 7);

                QString enumStringsList("");
                for(const QString& enumString : paramInfo->enumStrings()){
                    enumStringsList += enumString + QString(",");
                }

                toolTipDescription += QString(", ") + QString("enumStrings:") + enumStringsList;

                QString enumValuesList("");
                for(const auto& enumValue : paramInfo->enumValues()){
                    enumValuesList += enumValue.toString() + QString(",");
                }
                toolTipDescription += QString(", ") + QString("enumValues:") + enumValuesList;

                toolTipDescription += QString(", ") + QString("units:") + paramInfo->units();
                toolTipDescription += QString(", ") + QString("nanUnchanged:") + QString::number(paramInfo->nanUnchanged());
            }
        }

        qWarning() << "toolTipDescription:"<<toolTipDescription;
        return toolTipDescription;
    } else {
        qWarning() << "Should not ask for command description on unknown command";
        return _commandName(item);
    }
}

QString UAVFlyManage_MavlinkMissionManagerDialog::_commandName(const MissionItem* item) const
{
    if(m_MavlinkMissionManagerMissionCommandListEnable && m_MavlinkMissionManagerMissionCommandList != nullptr){
        const auto* const commandUIInfo = m_MavlinkMissionManagerMissionCommandList->getUIInfo(item->command());
        if(commandUIInfo != nullptr){
            return commandUIInfo->friendlyName();
        }
    }

    const MissionCommandUIInfo* uiInfo = QGCApplication::missionCommandTree()->getUIInfo(m_vehicle, m_vehicle->vehicleClass(), item->command());
    if (uiInfo) {
        return uiInfo->friendlyName();
    } else {
        qWarning() << "Request for command name on unknown command";
        return tr("Unknown: %1").arg(item->command());
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::_calcPrevWaypointValues(const MissionItem* currentItem, const MissionItem* prevItem, double* azimuth, double* distance, double* altDifference)
{
    QGeoCoordinate  currentCoord =  currentItem->coordinate();
    QGeoCoordinate  prevCoord =     prevItem->coordinate();

    *distance = prevCoord.distanceTo(currentCoord);
    *azimuth = prevCoord.azimuthTo(currentCoord);
}

void UAVFlyManage_MavlinkMissionManagerDialog::_recalcMissionFlightStatus()
{
    typedef struct {
        double                      maxTelemetryDistance;
        double                      totalDistance;
        double                      totalTime;
        double                      hoverDistance;
        double                      hoverTime;
        double                      cruiseDistance;
        double                      cruiseTime;
        int                         mAhBattery;
        double                      hoverAmps;
        double                      cruiseAmps;
        double                      ampMinutesAvailable;
        double                      hoverAmpsTotal;
        double                      cruiseAmpsTotal;
        int                         batteryChangePoint;
        int                         batteriesRequired;
        double                      vehicleYaw;
        double                      gimbalYaw;
        double                      gimbalPitch;

        QGCMAVLink::VehicleClass_t  vtolMode;
        double                      cruiseSpeed;
        double                      hoverSpeed;
        double                      vehicleSpeed;
    } MissionFlightStatus_t;

    MissionFlightStatus_t _missionFlightStatus = {0};
    double                      _minAMSLAltitude =              0;
    double                      _maxAMSLAltitude =              0;

    if (!m_MissionItems.size()) {
        return;
    }

    bool                firstCoordinateItem =           true;

    MissionItem*  lastFlyThroughVI =   qobject_cast<MissionItem*>(m_MissionItems.at(0));

    bool homePositionValid = true;

    qCDebug(MavlinkMissionManagerDialog) << "_recalcMissionFlightStatus";

    _minAMSLAltitude = _maxAMSLAltitude = qQNaN();

    bool   linkStartToHome =            false;
    bool   foundRTL =                   false;
    double totalHorizontalDistance =    0;

    for (int i=0; i<m_MissionItems.size(); i++) {

        MissionItem*  item =          (m_MissionItems.at(i));

        if (item && item->command() == MAV_CMD_NAV_RETURN_TO_LAUNCH) {
            foundRTL = true;
        }

        if (i != 0 && !foundRTL) {

            if (firstCoordinateItem && item && (item->command() == MAV_CMD_NAV_TAKEOFF || item->command() == MAV_CMD_NAV_VTOL_TAKEOFF)) {
                if (homePositionValid) {
                    linkStartToHome = true;
                    if (m_vehicle->multiRotor() || m_vehicle->vtol()) {

                    }
                }
            }

            if (_specifiesCoordinate(item)) {

                if (!_isStandaloneCoordinate(item)) {
                    firstCoordinateItem = false;

                    if (
                            lastFlyThroughVI != item ||
                            linkStartToHome) {

                        double azimuth, distance, altDifference;

                        _calcPrevWaypointValues(item, lastFlyThroughVI, &azimuth, &distance, &altDifference);
                        totalHorizontalDistance += distance;

                    }

                    lastFlyThroughVI = item;
                }
            }
        }

        double newSpeed = item->specifiedFlightSpeed();
        if (!qIsNaN(newSpeed)) {
            if (m_vehicle->multiRotor()) {
                _missionFlightStatus.hoverSpeed = newSpeed;
            } else if (m_vehicle->vtol()) {
                if (_missionFlightStatus.vtolMode == QGCMAVLink::VehicleClassMultiRotor) {
                    _missionFlightStatus.hoverSpeed = newSpeed;
                } else {
                    _missionFlightStatus.cruiseSpeed = newSpeed;
                }
            } else {
                _missionFlightStatus.cruiseSpeed = newSpeed;
            }
            _missionFlightStatus.vehicleSpeed = newSpeed;
        }

        if (lastFlyThroughVI && m_vehicle->vtol()) {
            switch (lastFlyThroughVI->command()) {
            case MAV_CMD_NAV_TAKEOFF:
            case MAV_CMD_NAV_VTOL_TAKEOFF:
            case MAV_CMD_NAV_LAND:
                _missionFlightStatus.vtolMode = QGCMAVLink::VehicleClassFixedWing;
                break;
            case MAV_CMD_NAV_VTOL_LAND:
                _missionFlightStatus.vtolMode = QGCMAVLink::VehicleClassMultiRotor;
                break;
            case MAV_CMD_DO_VTOL_TRANSITION:
            {
                int transitionState = lastFlyThroughVI->param1();
                if (transitionState == MAV_VTOL_STATE_MC) {
                    _missionFlightStatus.vtolMode = QGCMAVLink::VehicleClassMultiRotor;
                } else if (transitionState == MAV_VTOL_STATE_FW) {
                    _missionFlightStatus.vtolMode = QGCMAVLink::VehicleClassFixedWing;
                }
            }
                break;
            default:
                break;
            }
        }
    }

    if (_missionFlightStatus.mAhBattery != 0 && _missionFlightStatus.batteryChangePoint == -1) {
        _missionFlightStatus.batteryChangePoint = 0;
    }

    ui->label_VehicleUpDownMissionInfo_Count->setText(tr2("点数") + QString(": %1").arg(m_MissionItems.size()));
    ui->label_VehicleUpDownMissionInfo_Dist->setText(tr2("距离") + QString(": %1").arg(totalHorizontalDistance, 0, 'f', 2));
}

void UAVFlyManage_MavlinkMissionManagerDialog::_setVehicleUpDownMissionProgress(double progress)
{

}

void UAVFlyManage_MavlinkMissionManagerDialog::_setVehicleUpDownMissionProgressInfo(const QString &progressInfo)
{

}

void UAVFlyManage_MavlinkMissionManagerDialog::_currentMissionIndexChanged(int sequenceNumber)
{
    qCDebug(MavlinkMissionManagerDialog) << "CurrentMissionIndex:" << sequenceNumber;

    _updateTips(tr2("当前任务航点序号已改变：") + QString::number(sequenceNumber));
}

QList<UAVFlyManage_MavlinkMissionManagerDialog::SupportMissionMavCmd> &UAVFlyManage_MavlinkMissionManagerDialog::SupportMissionMavCmdList()
{
    return m_SupportMavCmdList;
}

bool UAVFlyManage_MavlinkMissionManagerDialog::HasSupportMissionMavCmd(MAV_CMD command)
{
    for (const auto& commandMav: SupportMissionMavCmdList()) {
        if(commandMav.mav_cmd == command){
            return true;
        }
    }

    return false;
}

QString UAVFlyManage_MavlinkMissionManagerDialog::SupportMissionMavCmdFriendlyName(MAV_CMD command) const
{
    if(m_MavlinkMissionManagerMissionCommandListEnable && m_MavlinkMissionManagerMissionCommandList != nullptr){
        const auto* const commandUIInfo = m_MavlinkMissionManagerMissionCommandList->getUIInfo(command);
        if(commandUIInfo != nullptr){
            return commandUIInfo->friendlyName();
        }
    }

    MissionCommandTree* commandTree = QGCApplication::missionCommandTree();
    if(commandTree){
        return commandTree->friendlyName(command);
    }
    return QStringLiteral("MAV_CMD(%1)").arg((int)command);
}

QList<SimpleMissionItem::EnumInfo_s> &UAVFlyManage_MavlinkMissionManagerDialog::SupportMissionMavFrameInfoList()
{
    return m_SupportMavFrameInfoList;
}

bool UAVFlyManage_MavlinkMissionManagerDialog::HasSupportMissionMavFrameInfo(MAV_FRAME frame)
{
    for (const auto& frameMav: SupportMissionMavFrameInfoList()) {
        if(frameMav.frame == frame){
            return true;
        }
    }

    return false;
}

void UAVFlyManage_MavlinkMissionManagerDialog::sendItemsToVehicle(Vehicle *vehicle, QList<MissionItem *> *visualMissionItems)
{
    if (vehicle) {
        QList<MissionItem*> rgMissionItems;

        _convertToMissionItems(visualMissionItems, rgMissionItems, vehicle);

        vehicle->missionManager()->writeMissionItems(rgMissionItems);
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::on_checkBox_VehicleUpDownMissionHide_stateChanged(int arg1)
{
    if(!ui){
        return ;
    }

    if(!ui->checkBox_VehicleUpDownMissionHide){
        return ;
    }

    m_MissionLineShow = !(ui->checkBox_VehicleUpDownMissionHide->checkState() == Qt::CheckState::Checked);

    //if(!m_MissionLineShow){
    //    Clear3DlinesWeaponID();
    //}else{
    //    Draw3DLinesWeaponID();
    //}
}

void UAVFlyManage_MavlinkMissionManagerDialog::on_pushButton_VehicleUpDownMissionWayPointAltitudeRelative_clicked()
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_VehicleUpDownMissionWayPointAltitudeRelative){
        return ;
    }

    const QVariant wayPointAltitudeRelative = ui->comboBox_VehicleUpDownMissionWayPointAltitudeRelative->currentData();
    if(wayPointAltitudeRelative.isNull()){
        return ;
    }

    bool ok = false;
    const int wayPointAltitudeRelativeValue = wayPointAltitudeRelative.toInt(&ok);
    if(!ok){
        return ;
    }

    bool findWayPointAltitudeRelative = false;
    MAV_FRAME frame = MAV_FRAME_GLOBAL;
    for(const auto& supportMavFrameInfo : SupportMissionMavFrameInfoList()){
       if (wayPointAltitudeRelativeValue == supportMavFrameInfo.frame){
           frame = supportMavFrameInfo.frame;
           findWayPointAltitudeRelative = true;
           break;
       }
    }

    if(!findWayPointAltitudeRelative){
        return ;
    }

    for(auto* missionItem : m_MissionItems){
        missionItem->setFrame(frame);
    }

    MissionManagerTableWidgetUpdate();
}

void UAVFlyManage_MavlinkMissionManagerDialog::on_pushButton_VehicleUpDownMissionWayPointAltitude_clicked()
{
    if(!ui){
        return ;
    }

    if(!ui->doubleSpinBox_VehicleUpDownMissionWayPointAltitude){
        return ;
    }

    const double wayPointAltitude = ui->doubleSpinBox_VehicleUpDownMissionWayPointAltitude->value();

    for(auto* missionItem : m_MissionItems){
        if( (missionItem->command() == MAV_CMD::MAV_CMD_NAV_WAYPOINT) && _specifiesCoordinate(missionItem)){
            missionItem->setParam7(wayPointAltitude);
        }
    }

    MissionManagerTableWidgetUpdate();
}

void UAVFlyManage_MavlinkMissionManagerDialog::on_pushButton_VehicleUpDownMissionWayPointSpeed_clicked()
{
    if(!ui){
        return ;
    }

    if(!ui->doubleSpinBox_VehicleUpDownMissionWayPointSpeed){
        return ;
    }

    const double wayPointSpeed = ui->doubleSpinBox_VehicleUpDownMissionWayPointSpeed->value();
    if(wayPointSpeed < 0){
        return ;
    }

    for(auto* missionItem : m_MissionItems){
        if(missionItem->command() == MAV_CMD::MAV_CMD_DO_CHANGE_SPEED){
            missionItem->setParam2(wayPointSpeed);
        }
    }

    MissionManagerTableWidgetUpdate();
}

void UAVFlyManage_MavlinkMissionManagerDialog::on_pushButton_VehicleUpDownMissionWayPointRadius_clicked()
{
    if(!ui){
        return ;
    }

    if(!ui->doubleSpinBox_VehicleUpDownMissionWayPointRadius){
        return ;
    }

    const double wayPointRadius = ui->doubleSpinBox_VehicleUpDownMissionWayPointRadius->value();
    if(wayPointRadius < 0){
        return ;
    }

    for(auto* missionItem : m_MissionItems){
        if(missionItem->command() == MAV_CMD::MAV_CMD_NAV_WAYPOINT){
            missionItem->setParam2(wayPointRadius);
        }
    }

    MissionManagerTableWidgetUpdate();
}

void UAVFlyManage_MavlinkMissionManagerDialog::on_pushButton_VehicleUpDownMissionWayPointHoldTime_clicked()
{
    if(!ui){
        return ;
    }

    if(!ui->spinBox_VehicleUpDownMissionWayPointHoldTime){
        return ;
    }

    const int wayPointHoldTime = ui->spinBox_VehicleUpDownMissionWayPointHoldTime->value();
    if(wayPointHoldTime < 0){
        return ;
    }

    for(auto* missionItem : m_MissionItems){
        if(missionItem->command() == MAV_CMD::MAV_CMD_NAV_WAYPOINT){
            missionItem->setParam1(wayPointHoldTime);
        }
    }

    MissionManagerTableWidgetUpdate();
}

void UAVFlyManage_MavlinkMissionManagerDialog::on_pushButton_VehicleUpDownMissionLoiterRadius_clicked()
{

}

bool UAVFlyManage_MavlinkMissionManagerDialog::_syncInProgress() const
{
    return (m_vehicle && m_vehicle->missionManager()->inProgress());
}

void UAVFlyManage_MavlinkMissionManagerDialog::loadFromVehicle()
{
    if (!m_vehicle) {
        qCWarning(MavlinkMissionManagerDialog) << "MavlinkMissionManagerDialog::loadFromVehicle called while Vehicle nullptr";
    } else if (_syncInProgress()) {
        qCWarning(MavlinkMissionManagerDialog) << "MavlinkMissionManagerDialog::loadFromVehicle called while syncInProgress";
    } else {
        _itemsRequested = true;
        m_vehicle->missionManager()->loadFromVehicle();
    }
	m_startLoad = true;
}

void UAVFlyManage_MavlinkMissionManagerDialog::sendToVehicle()
{
    _updateTips("");
    if (!m_vehicle) {
        qCWarning(MavlinkMissionManagerDialog) << "MavlinkMissionManagerDialog::sendToVehicle called while Vehicle nullptr";
    } else if (_syncInProgress()) {
        qCWarning(MavlinkMissionManagerDialog) << "MavlinkMissionManagerDialog::sendToVehicle called while syncInProgress";
    } else {
        qCDebug(MavlinkMissionManagerDialog) << "MavlinkMissionManagerDialog::sendToVehicle";
        if (m_MissionItems.size() <= 1) {

            QList<MissionItem*> emptyModel;
            sendItemsToVehicle(m_vehicle, &emptyModel);
        } else {
            sendItemsToVehicle(m_vehicle, &m_MissionItems);
        }
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::removeAll()
{

}

void UAVFlyManage_MavlinkMissionManagerDialog::removeAllFromVehicle()
{
    if (!m_vehicle) {
        qCWarning(MavlinkMissionManagerDialog) << "MavlinkMissionManagerDialog::removeAllFromVehicle called while Vehicle nullptr";
    } else if (_syncInProgress()) {
        qCWarning(MavlinkMissionManagerDialog) << "MavlinkMissionManagerDialog::removeAllFromVehicle called while syncInProgress";
    } else {
        _itemsRequested = true;
        m_vehicle->missionManager()->removeAll();
    }
}

const QString& UAVFlyManage_MavlinkMissionManagerDialog::getTaskFileDir()
{
    return m_TaskFileDir;
}

const QString UAVFlyManage_MavlinkMissionManagerDialog::getCreateTaskFileDir()
{
    return QString(QString("../data/TaskRecord/TaskMav/TaskMav_") + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss_zzz") + QString("/"));
}

void UAVFlyManage_MavlinkMissionManagerDialog::saveToFile(const QString& filename, QString& errorString)
{
    if (filename.isEmpty()) {
        return;
    }

    QString planFilename = filename;
    if (!QFileInfo(filename).fileName().contains(".")) {
        planFilename += QString(".%1").arg("json");
    }

    QFile file(planFilename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QGCApplication::showAppMessage(m_vehicle->weaponTitle() + tr("\xe4\xbb\xbb\xe5\x8a\xa1\xe4\xbf\x9d\xe5\xad\x98\xe9\x94\x99\xe8\xaf\xaf %1 : %2").arg(filename).arg(file.errorString()));
    } else {
        QJsonDocument saveDoc = saveToJson();
        const auto result = file.write(saveDoc.toJson());
        if(result > 0){

            errorString = m_vehicle->weaponTitle() + tr("\xe4\xbb\xbb\xe5\x8a\xa1\xe4\xbf\x9d\xe5\xad\x98\xe6\x88\x90\xe5\x8a\x9f %1 : %2").arg(filename).arg(file.errorString());
            QGCApplication::showAppMessage(m_vehicle->weaponTitle() + tr("\xe4\xbb\xbb\xe5\x8a\xa1\xe4\xbf\x9d\xe5\xad\x98\xe6\x88\x90\xe5\x8a\x9f %1 : %2").arg(filename).arg(file.errorString()));
        }else{

            errorString = m_vehicle->weaponTitle() + tr("\xe4\xbb\xbb\xe5\x8a\xa1\xe4\xbf\x9d\xe5\xad\x98\xe9\x94\x99\xe8\xaf\xaf %1 : %2").arg(filename).arg(file.errorString());
            QGCApplication::showAppMessage(m_vehicle->weaponTitle() + tr("\xe4\xbb\xbb\xe5\x8a\xa1\xe4\xbf\x9d\xe5\xad\x98\xe9\x94\x99\xe8\xaf\xaf %1 : %2").arg(filename).arg(file.errorString()));
        }
    }

}

void UAVFlyManage_MavlinkMissionManagerDialog::saveToTaskFileDir(const QString &taskFileDir, QString& errorString)
{
    if(taskFileDir.isEmpty()){
        errorString += tr("\xe9\x94\x99\xe8\xaf\xaf\xef\xbc\x8c\xe4\xbb\xbb\xe5\x8a\xa1\xe7\x9b\xae\xe5\xbd\x95\xe4\xb8\x8d\xe5\xad\x98\xe5\x9c\xa8!");
        return ;
    }

    m_TaskFileDir = taskFileDir;
    QDir taskFileDirNew(taskFileDir);
    if (!taskFileDirNew.exists(taskFileDirNew.absolutePath())) {
        taskFileDirNew.mkdir(taskFileDirNew.absolutePath());
    }

    if(!taskFileDirNew.exists()){
        errorString += tr("\xe9\x94\x99\xe8\xaf\xaf\xef\xbc\x8c\xe4\xbb\xbb\xe5\x8a\xa1\xe6\x96\x87\xe4\xbb\xb6\xe4\xb8\x8d\xe5\xad\x98\xe5\x9c\xa8!");
        return ;
    }

    if(!m_TaskFileDir.isEmpty()){
        saveToFile(m_TaskFileDir + QString("Vehicle_Mission_%1.json").arg(getVehicle()->id()), errorString);
    }else{

        m_TaskFileDir = getCreateTaskFileDir();
        QDir taskFileDirNew(m_TaskFileDir);
        if(!taskFileDirNew.exists(taskFileDirNew.absolutePath())){
            taskFileDirNew.mkdir(taskFileDirNew.absolutePath());
        }
        saveToFile(m_TaskFileDir + QString("Vehicle_Mission_%1.json").arg(getVehicle()->id()), errorString);
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::loadFromFile(const QString& filename, QString& errorString)
{

    QString errorMessage = tr("\xe5\x8a\xa0\xe8\xbd\xbd\xe4\xbb\xbb\xe5\x8a\xa1\xe6\x96\x87\xe4\xbb\xb6\xe9\x94\x99\xe8\xaf\xaf (%1). %2").arg(filename).arg("%1");

    if (filename.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(filename);
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if(file.error() == QFileDevice::FileError::NoError){
            errorString = tr("\xe6\x97\xa0\xe9\x94\x99\xe8\xaf\xaf") + QStringLiteral(" ") + filename;
        }else{
            errorString = file.errorString() + QStringLiteral(" ") + filename;
        }
        QGCApplication::showAppMessage(m_vehicle->weaponTitle() + errorMessage.arg(errorString));
        return;
    }

    bool success = false;
    if (fileInfo.suffix() == "mission") {
        if (!loadJsonFile(file, errorString)) {
            QGCApplication::showAppMessage(m_vehicle->weaponTitle() + errorMessage.arg(errorString));
        } else {
            success = true;
        }
    }else{
        QJsonDocument   jsonDoc;
        QByteArray      bytes = file.readAll();

        if (!JsonHelper::isJsonFile(bytes, jsonDoc, errorString)) {
            QGCApplication::showAppMessage(m_vehicle->weaponTitle() + errorMessage.arg(errorString));
            return;
        }

        QJsonObject json = jsonDoc.object();

        int version;
        if (!JsonHelper::validateExternalQGCJsonFile(json, _kPlanFileType, _kPlanFileVersion, _kPlanFileVersion, version, errorString)) {
            QGCApplication::showAppMessage(m_vehicle->weaponTitle() + errorMessage.arg(errorString));
            return;
        }

        QList<JsonHelper::KeyValidateInfo> rgKeyInfo = {
            { _kJsonMissionObjectKey,        QJsonValue::Object, true },
            { _kJsonGeoFenceObjectKey,       QJsonValue::Object, true },
            { _kJsonRallyPointsObjectKey,    QJsonValue::Object, true },
        };
        if (!JsonHelper::validateKeys(json, rgKeyInfo, errorString)) {
            QGCApplication::showAppMessage(m_vehicle->weaponTitle() + errorMessage.arg(errorString));
            return;
        }

        if (!load(json[_kJsonMissionObjectKey].toObject(), errorString)
                ) {
            QGCApplication::showAppMessage(m_vehicle->weaponTitle() + errorMessage.arg(errorString));
        } else {

            success = true;
        }
    }

    QString _currentPlanFile;
    if(success){
        _currentPlanFile = QString::asprintf("%s/%s.%s", fileInfo.path().toLocal8Bit().data(), fileInfo.completeBaseName().toLocal8Bit().data(), "json");
    } else {
        _currentPlanFile.clear();
    }
}

QJsonDocument UAVFlyManage_MavlinkMissionManagerDialog::saveToJson()
{
    QJsonObject planJson;
    planJson["weaponID"]     = QJsonValue(qint64(m_vehicle->weaponId()));
    planJson["vehicleID"]    = m_vehicle->id();
    planJson["firmwareType"] = m_vehicle->firmwareType();
    planJson["vehicleType"]  = m_vehicle->vehicleType();

    QJsonObject missionJson;
    QJsonObject fenceJson;
    QJsonObject rallyJson;
    JsonHelper::saveQGCJsonFileHeader(planJson, _kPlanFileType, _kPlanFileVersion);

    save(missionJson);

    planJson[_kJsonMissionObjectKey]     = missionJson;
    planJson[_kJsonGeoFenceObjectKey]    = fenceJson;
    planJson[_kJsonRallyPointsObjectKey] = rallyJson;

    return QJsonDocument(planJson);
}

bool UAVFlyManage_MavlinkMissionManagerDialog::loadJsonFile(QFile& file, QString& errorString)
{
    QString         errorStr;
    QString         errorMessage = tr("Mission: %1");
    QJsonDocument   jsonDoc;
    QByteArray      bytes = file.readAll();

    if (!JsonHelper::isJsonFile(bytes, jsonDoc, errorStr)) {
        errorString = errorMessage.arg(errorStr);
        return false;
    }

    QJsonObject json = jsonDoc.object();

    QList<MissionItem*> missionItemsLoaded;
    if (!_loadItemsFromJson(json, &missionItemsLoaded, errorStr)) {
        errorString = errorMessage.arg(errorStr);
        return false;
    }

    if(missionItemsLoaded.size() > 0){
        _clearAndDeleteWriteMissionItems();
        m_MissionItems = std::move(missionItemsLoaded);
        MissionManagerTableWidgetUpdate();
    }

    return true;
}

void UAVFlyManage_MavlinkMissionManagerDialog::loadFromTaskFileDir(const QString &taskFileDir, QString& errorString)
{
    if(!taskFileDir.isEmpty()){
        _clearAndDeleteWriteMissionItems();
        MissionManagerTableWidgetUpdate();
        m_TaskFileDir = taskFileDir;
        loadFromFile(taskFileDir + QString("Vehicle_Mission_%1.json").arg(getVehicle()->id()), errorString);
    }
}

bool UAVFlyManage_MavlinkMissionManagerDialog::load(const QJsonObject& json, QString& errorString)
{
    QString errorStr;
    QString errorMessage = tr("Mission: %1");

    QList<MissionItem*> missionItemsLoaded;
    if (!_loadJsonMissionFileV2(json, &missionItemsLoaded, errorStr)) {
        errorString = errorMessage.arg(errorStr);
        return false;
    }

    if(missionItemsLoaded.size() > 0){
        _clearAndDeleteWriteMissionItems();
        m_MissionItems = std::move(missionItemsLoaded);
        MissionManagerTableWidgetUpdate();
    }

    return true;
}

bool UAVFlyManage_MavlinkMissionManagerDialog::_loadItemsFromJson(const QJsonObject& json, QList<MissionItem *> *visualItems, QString& errorString)
{

    if (!json.contains(JsonHelper::jsonFileTypeKey)) {
        json[JsonHelper::jsonFileTypeKey] = _jsonFileTypeValue;
    }

    int fileVersion;
    JsonHelper::validateExternalQGCJsonFile(json,
                                            _jsonFileTypeValue,
                                            1,
                                            2,
                                            fileVersion,
                                            errorString);

    if (fileVersion == 1) {
        return _loadJsonMissionFileV1(json, visualItems, errorString);
    } else {
        return _loadJsonMissionFileV2(json, visualItems, errorString);
    }
}

bool UAVFlyManage_MavlinkMissionManagerDialog::_loadJsonMissionFileV1(const QJsonObject& json, QList<MissionItem*>* visualItems, QString& errorString)
{

    QList<JsonHelper::KeyValidateInfo> rootKeyInfoList = {
        { _jsonPlannedHomePositionKey,      QJsonValue::Object, true },
        { _jsonItemsKey,                    QJsonValue::Array,  true },
        { _jsonMavAutopilotKey,             QJsonValue::Double, true },
        { _jsonComplexItemsKey,             QJsonValue::Array,  true },
    };
    if (!JsonHelper::validateKeys(json, rootKeyInfoList, errorString)) {
        return false;
    }

    QJsonArray complexArray(json[_jsonComplexItemsKey].toArray());
    qCDebug(MavlinkMissionManagerDialog) << "Json load: complex item count" << complexArray.count();

    int nextSimpleItemIndex= 0;
    int nextComplexItemIndex= 0;
    int nextSequenceNumber = 0;
    QJsonArray itemArray(json[_jsonItemsKey].toArray());

    if (json.contains(_jsonPlannedHomePositionKey)) {
        MissionItem* item = new MissionItem();
        item->_param1Fact._setName("Param1");
        item->_param1Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        item->_param2Fact._setName("Param2");
        item->_param2Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        item->_param3Fact._setName("Param3");
        item->_param3Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        item->_param4Fact._setName("Param4");
        item->_param4Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        item->_param5Fact._setName("Lat/X");
        item->_param5Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        item->_param6Fact._setName("Lon/Y");
        item->_param6Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        item->_param7Fact._setName("Alt/Z");
        item->_param7Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());
        if (item->load(json[_jsonPlannedHomePositionKey].toObject(), 0, errorString)) {

            item->deleteLater();
        } else {
            return false;
        }
    }

    qCDebug(MavlinkMissionManagerDialog) << "Json load: simple item loop start simpleItemCount:ComplexItemCount" << itemArray.count();
    do {
        qCDebug(MavlinkMissionManagerDialog) << "Json load: simple item loop nextSimpleItemIndex:nextComplexItemIndex:nextSequenceNumber" << nextSimpleItemIndex << nextComplexItemIndex << nextSequenceNumber;

        if (nextSimpleItemIndex < itemArray.count()) {
            const QJsonValue& itemValue = itemArray[nextSimpleItemIndex++];

            if (!itemValue.isObject()) {
                errorString = QStringLiteral("Mission item is not an object");
                return false;
            }

            const QJsonObject itemObject = itemValue.toObject();
            MissionItem* item = new MissionItem();
            item->_param1Fact._setName("Param1");
            item->_param1Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            item->_param2Fact._setName("Param2");
            item->_param2Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            item->_param3Fact._setName("Param3");
            item->_param3Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            item->_param4Fact._setName("Param4");
            item->_param4Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            item->_param5Fact._setName("Lat/X");
            item->_param5Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            item->_param6Fact._setName("Lon/Y");
            item->_param6Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            item->_param7Fact._setName("Alt/Z");
            item->_param7Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());
            if (item->load(itemObject, itemObject["id"].toInt(), errorString)) {
                qCDebug(MavlinkMissionManagerDialog) << "Json load: adding simple item expectedSequence:actualSequence" << nextSequenceNumber << item->sequenceNumber();
                nextSequenceNumber = item->sequenceNumber() + 1;
                visualItems->append(item);
            } else {
                return false;
            }
        }
    } while (nextSimpleItemIndex < itemArray.count());

    return true;
}

bool UAVFlyManage_MavlinkMissionManagerDialog::_loadJsonMissionFileV2(const QJsonObject& json, QList<MissionItem*>* visualItems, QString& errorString)
{

    QList<JsonHelper::KeyValidateInfo> rootKeyInfoList = {
        { _jsonPlannedHomePositionKey,      QJsonValue::Array,  true },
        { _jsonItemsKey,                    QJsonValue::Array,  true },
        { _jsonFirmwareTypeKey,             QJsonValue::Double, true },
        { _jsonVehicleTypeKey,              QJsonValue::Double, false },
        { _jsonCruiseSpeedKey,              QJsonValue::Double, false },
        { _jsonHoverSpeedKey,               QJsonValue::Double, false },
        { _jsonGlobalPlanAltitudeModeKey,   QJsonValue::Double, false },
    };
    if (!JsonHelper::validateKeys(json, rootKeyInfoList, errorString)) {
        return false;
    }

    qCDebug(MavlinkMissionManagerDialog) << "MissionController::_loadJsonMissionFileV2 itemCount:" << json[_jsonItemsKey].toArray().count();

    MAV_AUTOPILOT   planFileFirmwareType =  static_cast<MAV_AUTOPILOT>(json[_jsonFirmwareTypeKey].toInt());
    MAV_TYPE        planFileVehicleType =   static_cast<MAV_TYPE>     (QGCMAVLink::vehicleClassToMavType(m_vehicle->vehicleClass()));
    if (json.contains(_jsonVehicleTypeKey)) {
        planFileVehicleType = static_cast<MAV_TYPE>(json[_jsonVehicleTypeKey].toInt());
    }

    m_vehicle->stopTrackingFirmwareVehicleTypeChanges();
    m_vehicle->_offlineFirmwareTypeSettingChanged(planFileFirmwareType);
    m_vehicle->_offlineVehicleTypeSettingChanged(planFileVehicleType);

    QGeoCoordinate homeCoordinate;
    if (!JsonHelper::loadGeoCoordinate(json[_jsonPlannedHomePositionKey], true , homeCoordinate, errorString)) {
        return false;
    }

    qCDebug(MavlinkMissionManagerDialog) << "plannedHomePosition" << homeCoordinate;

    int nextSequenceNumber = 0;
    const QJsonArray rgMissionItems(json[_jsonItemsKey].toArray());
    for (int i=0; i<rgMissionItems.count(); i++) {

        const QJsonValue& itemValue = rgMissionItems[i];
        if (!itemValue.isObject()) {
            errorString = tr("Mission item %1 is not an object").arg(i);
            return false;
        }
        const QJsonObject itemObject = itemValue.toObject();

        QList<JsonHelper::KeyValidateInfo> itemKeyInfoList = {
            { VisualMissionItem::jsonTypeKey,  QJsonValue::String, true },
        };
        if (!JsonHelper::validateKeys(itemObject, itemKeyInfoList, errorString)) {
            return false;
        }
        QString itemType = itemObject[VisualMissionItem::jsonTypeKey].toString();

        if (itemType == VisualMissionItem::jsonTypeSimpleItemValue) {
            MissionItem* simpleItem = new MissionItem();
            simpleItem->_param1Fact._setName("Param1");
            simpleItem->_param1Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            simpleItem->_param2Fact._setName("Param2");
            simpleItem->_param2Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            simpleItem->_param3Fact._setName("Param3");
            simpleItem->_param3Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            simpleItem->_param4Fact._setName("Param4");
            simpleItem->_param4Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            simpleItem->_param5Fact._setName("Lat/X");
            simpleItem->_param5Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            simpleItem->_param6Fact._setName("Lon/Y");
            simpleItem->_param6Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            simpleItem->_param7Fact._setName("Alt/Z");
            simpleItem->_param7Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

            if (simpleItem->load(itemObject, nextSequenceNumber, errorString)) {
                qCDebug(MavlinkMissionManagerDialog) << "Loading simple item: nextSequenceNumber:command" << nextSequenceNumber << simpleItem->command();
                nextSequenceNumber = simpleItem->sequenceNumber() + 1;
                visualItems->append(simpleItem);
            } else {
                return false;
            }
        } else {
            errorString = tr("Unknown item type: %1").arg(itemType);
            return false;
        }
    }

    for (int i=0; i<visualItems->count(); i++) {
        if (visualItems->at(i)) {
            MissionItem* doJumpItem = visualItems->at(i);
            if (doJumpItem->command() == MAV_CMD_DO_JUMP) {
                bool found = false;
                int findDoJumpId = static_cast<int>(doJumpItem->param1());
                for (int j=0; j<visualItems->count(); j++) {
                    if (visualItems->at(j)) {
                        MissionItem* targetItem = visualItems->at(j);
                        if (targetItem->doJumpId() == findDoJumpId) {
                            doJumpItem->setParam1(targetItem->sequenceNumber());
                            found = true;
                            break;
                        }
                    }
                }
                if (!found) {
                    errorString = tr("Could not find doJumpId: %1").arg(findDoJumpId);
                    return false;
                }
            }
        }
    }

    return true;
}

void UAVFlyManage_MavlinkMissionManagerDialog::save(QJsonObject& json)
{
    json[JsonHelper::jsonVersionKey] = _missionFileVersion;

    if (m_MissionItems.size() <= 0) {
        qWarning() << "First item is not MissionSettingsItem";
        return;
    }
    QJsonValue coordinateValue;
    JsonHelper::saveGeoCoordinate(m_vehicle->coordinate(), true , coordinateValue);
    json[_jsonPlannedHomePositionKey]       = coordinateValue;
    json[_jsonFirmwareTypeKey]              = m_vehicle->firmwareType();
    json[_jsonVehicleTypeKey]               = m_vehicle->vehicleType();
    json[_jsonCruiseSpeedKey]               = m_vehicle->defaultCruiseSpeed();
    json[_jsonHoverSpeedKey]                = m_vehicle->defaultHoverSpeed();
    json[_jsonGlobalPlanAltitudeModeKey]    = _globalAltMode;

    QJsonArray rgJsonMissionItems;
    for (int i=0; i<m_MissionItems.count(); i++) {
        MissionItem* visualItem = m_MissionItems.at(i);

        QJsonObject rgJsonMissionItemsObject;
        visualItem->save(rgJsonMissionItemsObject);
        rgJsonMissionItems.append(rgJsonMissionItemsObject);
    }

    json[_jsonItemsKey] = rgJsonMissionItems;
}

void UAVFlyManage_MavlinkMissionManagerDialog::_clearAndDeleteWriteMissionItems(void)
{
    for (int i=0; i<m_MissionItems.size(); i++) {

        delete m_MissionItems[i];
    }
    m_MissionItems.clear();
}

bool UAVFlyManage_MavlinkMissionManagerDialog::_convertToMissionItems(QList<MissionItem *> *visualMissionItems, QList<MissionItem *> &rgMissionItems, QObject *missionItemParent)
{
    if (visualMissionItems->size() == 0) {
        return false;
    }

    bool endActionSet = false;
    int lastSeqNum = 0;

    for (int i=0; i<visualMissionItems->size(); i++) {

        MissionItem* visualItem = qobject_cast<MissionItem*>(visualMissionItems->at(i));
        lastSeqNum = visualItem->sequenceNumber();
        rgMissionItems.append(new MissionItem(*visualItem, missionItemParent));

        qCDebug(MavlinkMissionManagerDialog) << "_convertToMissionItems seqNum:lastSeqNum:command"
                                             << visualItem->sequenceNumber()
                                             << lastSeqNum
                                             << visualItem->command();
    }

    return endActionSet;
}

UAVFlyManage_MavlinkMissionManagerDialog::~UAVFlyManage_MavlinkMissionManagerDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkMissionManagerDialog::DrawWayPointImage(QPainter &painter, const QPoint& pos, const bool currentSelect, const bool Selected)
{
    if (Selected == false)
    {
        if(currentSelect){
            painter.drawPixmap(QPointF(pos.x() - 12, pos.y() - 33), m_WayPointImage32);
        }else{
            painter.drawPixmap(QPointF(pos.x() - 9, pos.y() - 21), m_WayPointImage);
        }
    }
    else
    {
        if(currentSelect){
            painter.drawPixmap(QPointF(pos.x() - 10, pos.y() - 33), m_WayPointImage32Selected);
        }else{
            painter.drawPixmap(QPointF(pos.x() - 7, pos.y() - 21), m_WayPointImageSelected);
        }
    }
}

bool UAVFlyManage_MavlinkMissionManagerDialog::DrawDynamic(QPainter &painter, IProj *pProj)
{
    if(!m_MissionLineShow){
        return false;
    }

    QPoint ptFirst;
    const QColor&  penColor      = getColor();
    const QColor&  penColorPoint = QColor::fromRgba(getColor() | 0X5F000000);
    QPen penText(QColor(255,255,255));
    QPen pen(penColor);

    const bool     selected                  = getSelected();
    const int      sequenceNumberSelected    = getSequenceNumberSelected();
    bool           firstItem                 = true;
    for(auto* missionItems : getMissionItems())
    {
        if(missionItems->command() == MAV_CMD::MAV_CMD_NAV_RETURN_TO_LAUNCH){
            if(getVehicle() && m_homePosition.isValid()){
                const double Lat_X = m_homePosition.latitude();
                const double Lon_Y = m_homePosition.longitude();
                const double Alt_Z = m_homePosition.altitude();
                const int    seq   = missionItems->sequenceNumber();

                const QPoint& pos = pProj->Jwd2Screen(Lon_Y, Lat_X);
                painter.drawLine(ptFirst, pos);
            }
        }
        else

        {

            {

                {

                    if(_specifiesCoordinate(missionItems)){
                        const double Lat_X = missionItems->param5();
                        const double Lon_Y = missionItems->param6();
                        const double Alt_Z = missionItems->param7();
                        const int    seq   = missionItems->sequenceNumber();

                        const QPoint& pos = pProj->Jwd2Screen(Lon_Y, Lat_X);

                        const int rx = (selected ? (pointSelectSize + 2 + ((seq == sequenceNumberSelected) ? 2 : 0)) : pointSelectSize);
                        painter.setPen(pen);
                        painter.setBrush(penColorPoint);
                        painter.drawEllipse(pos, rx, rx);

                        DrawWayPointImage(painter, pos, selected, (seq == sequenceNumberSelected));

                        painter.setPen(pen);
                        if (firstItem)
                        {
                            firstItem = false;
                            ptFirst = pos;

                            const QString& strID = QString("%1 %2").arg(seq + 1).arg(m_vehicle->id());
                            painter.setPen(penText);
                            painter.drawText(QPoint(pos.x() - 5, pos.y() - 5), strID);
                        }
                        else
                        {

                            painter.setPen(penText);
                            QPoint ptTmp(pos.x() - ( (seq >= 10) ? 9 : 4), pos.y() - 5);
                            painter.drawText(ptTmp, QString::number(seq + 1));

                            painter.setPen(pen);
                            painter.drawLine(ptFirst, pos);
                            ptFirst = pos;
                        }

                    }
                }
            }

        }
    }
    return false;
}

bool UAVFlyManage_MavlinkMissionManagerDialog::OnMouseButtonDown(QMouseEvent *e, IProj *pProj)
{
    if (e->button() == Qt::RightButton || !pProj) {
        return false;
    }

    for(auto* missionItems : getMissionItems())
    {

        {

            {

                {

                    {

                        const double Lat_X = missionItems->param5();
                        const double Lon_Y = missionItems->param6();
                        const double Alt_Z = missionItems->param7();
                        const int    seq   = missionItems->sequenceNumber();

                        const QPoint& pos = pProj->Jwd2Screen(Lon_Y, Lat_X);

                        if(QRect(pos.x() - pointSelectSize, pos.y() - pointSelectSize, 2 * pointSelectSize, 2 * pointSelectSize).contains(e->pos())){
                            setSequenceNumberSelected(seq);
                            return false;
                        }

                    }
                }

            }
        }
    }
    return false;
}

bool UAVFlyManage_MavlinkMissionManagerDialog::OnMouseButtonRelease(QMouseEvent *e, IProj *pProj)
{
    return false;
}

bool UAVFlyManage_MavlinkMissionManagerDialog::OnMouseDoubleClick(QMouseEvent *e, IProj *pProj)
{
    return false;
}

bool UAVFlyManage_MavlinkMissionManagerDialog::OnMouseMove(QMouseEvent *e, IProj *pProj)
{
    if (e->button() == Qt::RightButton || !pProj) {
        return false;
    }

    const int sequenceNumberSelected = getSequenceNumberSelected();
    if(sequenceNumberSelected < 0){
        return false;
    }

    const QPoint& pos = e->pos();
    double dLongitude = 0, dLatitude = 0;
    pProj->Screen2Jwd(pos, dLongitude, dLatitude);

    for(auto* missionItems : getMissionItems())
    {
        if(sequenceNumberSelected == missionItems->sequenceNumber()){

            {

                {

                    {

                        {
                            if(sequenceNumberSelected == missionItems->sequenceNumber()){
                                missionItems->setParam5(dLatitude);
                                missionItems->setParam6(dLongitude);
                                updateSimpleMissionItemWayPoint(sequenceNumberSelected);
                                return false;
                            }

                        }
                    }

                }
            }
            break;
        }
    }
    return false;
}

void UAVFlyManage_MavlinkMissionManagerDialog::Draw3DLinesWeaponID()
{
    Clear3DlinesWeaponID();

    if (m_vehicle == nullptr || m_MissionItems.size() <= 0) {
        return;
    }

    qDebug() << "Draw3DLinesWeaponID:" << m_vehicle->weaponId() << m_vehicle->id();

    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth == nullptr || pEarth->GetGraphics() == nullptr) {
        return;
    }

    if(!m_MissionLineShow){
        return ;
    }

    QColor colorBombTrack(m_Color);
    colorBombTrack.setAlpha(255);
    QColor colorTemp;
    colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorBombTrack.alpha());

    IUserMesh1      *pUM = nullptr;

    m_UAV3D.strID = QString("%1%2").arg(m_vehicle->weaponId()).arg(m_vehicle->id()).toStdString();
    m_UAV3D.colorLine = colorTemp.rgba();

    short elevation = 0;
    std::vector<double> pdLon;
    std::vector<double> pdLat;
    std::vector<double> pdHeight;
    unsigned int PointIndex = 0;
    if(m_MissionItems.size() > 0 && s_pCGetHeight != nullptr && m_FlightTaskManageEditLineElevation){
        short int elevationTemp = s_pCGetHeight->GetHeight(m_MissionItems.at(0)->param6(), m_MissionItems.at(0)->param5());
        if(elevationTemp != -32768){
            elevation = elevationTemp;
        }
    }

    const QString strPathOld = "../data/Temp_Line/marker48X48_3D.png";

    QString strLable = QString("%1 %2").arg(1).arg(m_vehicle->id());

    if(!_pLabelMap3D){
        _pLabelMap3D = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strPathOld.toStdString().c_str(), 0,
                                                          0, 0, 24, 24, 0);
        if(_pLabelMap3D != nullptr){
            _pLabelMap3D->SetColor(0xffffffff, 0xffffffff, 0xffffffff);
            _pLabelMap3D->SetFontSize(15);
            _pLabelMap3D->SetTagState(false);
            _pLabelMap3D->SetText(strLable.toLocal8Bit());

        }
    }

    for (const auto* missionItem : m_MissionItems)
    {
        if(_specifiesCoordinate(missionItem)){
            pdLon.push_back(0);
            pdLat.push_back(0);
            pdHeight.push_back(0);

            const double dLongitude        = missionItem->param6();
            const double dLatitude         = missionItem->param5();
            const double dHeightElevation  = missionItem->param7() + elevation;

            pdLon[PointIndex]              = dLongitude;
            pdLat[PointIndex]              = dLatitude;
            pdHeight[PointIndex]           = dHeightElevation;

            short int elevationTemp = 0;
            if(s_pCGetHeight != nullptr && m_FlightTaskManageEditLineElevation){
                short int heightTemp = s_pCGetHeight->GetHeight(dLongitude, dLatitude);
                if(heightTemp == -32768){
                    heightTemp = 0;
                }
                elevationTemp = heightTemp;
                if(elevationTemp > dHeightElevation){
                    elevationTemp = dHeightElevation;
                }
            }

            double pdLon1[2]    = { dLongitude    , dLongitude };
            double pdLat1[2]    = { dLatitude     , dLatitude  };
            double pdHeight1[2] = { dHeightElevation * 1.0 , elevationTemp * 1.0 };
            CCreateShape cs1;
            SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
            if(pShp1 != nullptr){
                IUserMesh1* pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
                if(pUM1 != nullptr){
                    pUM1->SetColor(colorTemp.rgba());
                    pUM1->SetState(IUserMesh::POINT, IUserMesh::USERMESH_LINE);
                    pUM1->SetState(IUserMesh::LINEWIDTH, 1);
					pUM1->SetState(GL_DEPTH_TEST, 1);
                    m_UAV3D.pMesh1Vec.push_back(pUM1);
                }
            }

            if (PointIndex != 0) {
                strLable = QString::number(missionItem->sequenceNumber() + 1);
            }

			ILabel *pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strPathOld.toStdString().c_str(), dLongitude,
				dLatitude, dHeightElevation, 24, 24, 0);
			if (pLabel != nullptr) {
				pLabel->SetColor(0xffffffff, 0xffffffff, 0xffffffff);
				pLabel->SetFontSize(15);
				pLabel->SetTagState(false);
				pLabel->SetText(strLable.toLocal8Bit());
				m_UAV3D.pLabelVec.push_back(pLabel);
			}
            PointIndex++;
        }
    }

    CCreateShape cs;
    SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), PointIndex, 100, false, nullptr, pdHeight.data());
    if(pShp != nullptr){
        pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
        if(pUM != nullptr){
            pUM->SetColor(colorTemp.rgba());
            pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
			pUM->SetState(IUserMesh::LINEWIDTH, 2);
			pUM->SetState(GL_DEPTH_TEST, 1);
            m_UAV3D.pMesh1Vec.push_back(pUM);
        }
    }

    pdLon.clear();
    pdLat.clear();
    pdHeight.clear();
}

void UAVFlyManage_MavlinkMissionManagerDialog::Clear3DlinesWeaponID()
{
    IEarth *pEarth = IEarth::GetEarth();

    if(pEarth && pEarth->GetGraphics())
    {
        for (auto& pMes1 : m_UAV3D.pMesh1Vec)
        {
            pEarth->GetGraphics()->RemoveObject(pMes1);
            pMes1 = nullptr;
        }
        m_UAV3D.pMesh1Vec.clear();

        for (auto & pLabel : m_UAV3D.pLabelVec)
        {
            pEarth->GetGraphics()->RemoveObject(pLabel);
            pLabel = nullptr;
        }
        m_UAV3D.pLabelVec.clear();
    }
}

QList<MissionItem *> UAVFlyManage_MavlinkMissionManagerDialog::getMissionItems() const
{
    return m_MissionItems;
}

void UAVFlyManage_MavlinkMissionManagerDialog::startMissionPlan()
{
    if(!ui){
        return ;
    }

    if(!ui->groupBox_UAVFlyManage_MavlinkTaskManage_LineSettings){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    ui->groupBox_UAVFlyManage_MavlinkTaskManage_LineSettings->setEnabled(true);

    tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

    tableWidget->viewport()->setEnabled(true);

}

void UAVFlyManage_MavlinkMissionManagerDialog::stopMissionPlan()
{
    if(!ui){
        return ;
    }

    if(!ui->groupBox_UAVFlyManage_MavlinkTaskManage_LineSettings){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    //ui->groupBox_UAVFlyManage_MavlinkTaskManage_LineSettings->setEnabled(false);

    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableWidget->viewport()->setEnabled(false);

    if(m_vehicle && m_vehicle->missionManager()){
        m_vehicle->missionManager()->stopLoadOrWriteVehicleMission();
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::stopMissionPlanNoStopLoadOrWriteVehicleMission()
{
    if(!ui){
        return ;
    }

    if(!ui->groupBox_UAVFlyManage_MavlinkTaskManage_LineSettings){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    ui->groupBox_UAVFlyManage_MavlinkTaskManage_LineSettings->setEnabled(false);

    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableWidget->viewport()->setEnabled(false);

    if(m_vehicle && m_vehicle->missionManager()){
        if(!QGCApplication::upDownVehicleAllMissionEnabled()){
            m_vehicle->missionManager()->stopLoadOrWriteVehicleMission();
        }
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::clearMissionItems()
{
    for(auto* missionItem : m_MissionItems){
        if(missionItem){
            missionItem->deleteLater();
        }
    }

    m_MissionItems.clear();

    MissionManagerTableWidgetUpdate();
}

void UAVFlyManage_MavlinkMissionManagerDialog::removeMissionItem(int index)
{
    if(index >= 0 && index < m_MissionItems.size()){
        auto* missionItem = m_MissionItems.at(index);
        m_MissionItems.removeAt(index);
        if(missionItem){
            missionItem->deleteLater();
        }

        for(int i = index; i < m_MissionItems.size(); i++){
            auto* missionItem = m_MissionItems.at(i);
            if(missionItem){
                missionItem->setSequenceNumber(i);
            }
        }
    }

    MissionManagerTableWidgetUpdate();
}

void UAVFlyManage_MavlinkMissionManagerDialog::removeSelectedMissionItem()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    const int currentRow = tableWidget->currentRow();
    if(currentRow < 0 || currentRow >= tableWidget->rowCount()){
        return ;
    }

    if(currentRow >= m_MissionItems.size()){
        return ;
    }

    for(int i = 0; i < m_MissionItems.size(); i++){
        auto* missionItem = m_MissionItems.at(i);
        if(missionItem && missionItem->sequenceNumber() == currentRow){
            m_MissionItems.removeOne(missionItem) ;
            break;
        }
    }

    for(int i = currentRow; i < m_MissionItems.size(); i++){
        auto* missionItem = m_MissionItems.at(i);
        if(missionItem){
            missionItem->setSequenceNumber(i);
        }
    }

    tableWidget->removeRow(currentRow);

    MissionManagerTableWidgetUpdate();

    return ;
}

void UAVFlyManage_MavlinkMissionManagerDialog::appendMissionItem(MissionItem *missionItem)
{
    if(!missionItem){
        return ;
    }

    insertMissionItem(m_MissionItems.size(), missionItem);
}

void UAVFlyManage_MavlinkMissionManagerDialog::insertMissionItem(int index, MissionItem *missionItem)
{
    if(index >= 0 && index < m_MissionItems.size() && missionItem){
        missionItem->setSequenceNumber(index);
        m_MissionItems.insert(index, missionItem);

        for(int i = index + 1; i < m_MissionItems.size(); i++){
            auto* missionItem = m_MissionItems.at(i);
            if(missionItem){
                missionItem->setSequenceNumber(i);
            }
        }

        MissionManagerTableWidgetUpdate();
    }else if(index == m_MissionItems.size() && missionItem){
        missionItem->setSequenceNumber(m_MissionItems.size());
        m_MissionItems.append(missionItem);

        appendMissionManagerTableWidget();
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::updateMissionItemFromVehicle()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    const int currentRow = tableWidget->currentRow();
    if(currentRow < 0 || currentRow >= tableWidget->rowCount()){
        return ;
    }

    if(currentRow >= m_MissionItems.size()){
        return ;
    }

    const auto vehicleCoordinate = m_vehicle->coordinate();
    if(vehicleCoordinate.isValid()){
        auto* missionItem = m_MissionItems.at(currentRow);
        if(missionItem){
            missionItem->setParam5(vehicleCoordinate.latitude());
            missionItem->setParam6(vehicleCoordinate.longitude());
            if(currentRow == 0){
                missionItem->setParam7(vehicleCoordinate.altitude());
            }else{

            }

            MissionManagerTableWidgetDisconnect();

            MissionManagerTableWidgetTableWidgetItem(tableWidget, currentRow);

            MissionManagerTableWidgetConnect();

            //Draw3DLinesWeaponID();
        }
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::appendSimpleMissionItemWayPoint(const double lat, const double lon, const double alt)
{
    insertSimpleMissionItemWayPoint(m_MissionItems.size(), lat, lon, alt);
}

void UAVFlyManage_MavlinkMissionManagerDialog::insertSimpleMissionItemWayPoint(int index, const double lat, const double lon, const double alt)
{

    MissionItem *missionItem = new MissionItem;
    if(missionItem){
        if(m_MissionItems.size() == 0){
            if(m_vehicle->apmFirmware()){
                missionItem->setCommand(MAV_CMD::MAV_CMD_NAV_WAYPOINT);
            }else if(m_vehicle->px4Firmware()){
                if(m_vehicle->vtol()){
                    missionItem->setCommand(MAV_CMD::MAV_CMD_NAV_VTOL_TAKEOFF);
                }else{
                    missionItem->setCommand(MAV_CMD::MAV_CMD_NAV_TAKEOFF);
                }
            }else{
                missionItem->setCommand(MAV_CMD::MAV_CMD_NAV_WAYPOINT);
            }
        } if(m_MissionItems.size() == 1){
            if(m_vehicle->apmFirmware()){
                if(m_vehicle->vtol()){
                    missionItem->setCommand(MAV_CMD::MAV_CMD_NAV_VTOL_TAKEOFF);
                }else{
                    missionItem->setCommand(MAV_CMD::MAV_CMD_NAV_TAKEOFF);
                }
            }else if(m_vehicle->px4Firmware()){
                missionItem->setCommand(MAV_CMD::MAV_CMD_NAV_WAYPOINT);
            }else{
                missionItem->setCommand(MAV_CMD::MAV_CMD_NAV_WAYPOINT);
            }
        }else {
            missionItem->setCommand(MAV_CMD::MAV_CMD_NAV_WAYPOINT);
        }
        missionItem->setFrame(getMavFrameDefault());

        missionItem->setAutoContinue(true);

        missionItem->_param1Fact._setName("Param1");
        missionItem->_param1Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        missionItem->_param2Fact._setName("Param2");
        missionItem->_param2Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        missionItem->_param3Fact._setName("Param3");
        missionItem->_param3Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        missionItem->_param4Fact._setName("Param4");
        missionItem->_param4Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());
        missionItem->setParam4(std::numeric_limits<double>::quiet_NaN());

        missionItem->_param5Fact._setName("Lat/X");
        missionItem->_param5Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        missionItem->_param6Fact._setName("Lon/Y");
        missionItem->_param6Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        missionItem->_param7Fact._setName("Alt/Z");
        missionItem->_param7Fact.setMetaData(SimpleMissionItem::defaultParamMetaData());

        missionItem->setParam5(lat);
        missionItem->setParam6(lon);
        missionItem->setParam7(alt);

        qDebug() << __FILE__ << __LINE__
                 << lat
                 << lon
                 << lat
                 << lon
                 << missionItem->command         ()
                 << missionItem->isCurrentItem   ()
                 << missionItem->sequenceNumber  ()
                 << missionItem->frame           ()
                 << missionItem->autoContinue    ()
                 << missionItem->param1          ()
                 << missionItem->param2          ()
                 << missionItem->param3          ()
                 << missionItem->param4          ()
                 << missionItem->param5          ()
                 << missionItem->param6          ()
                 << missionItem->param7          ()
                 << missionItem->coordinate      ()
                    ;

        insertMissionItem(index, missionItem);
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::updateSimpleMissionItemWayPoint(int index)
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    const int& current = index;
    if(current < 0 || current > tableWidget->rowCount()){
        return ;
    }

    if(current >= m_MissionItems.size()){
        return ;
    }

    MissionManagerTableWidgetDisconnect();

    MissionManagerTableWidgetTableWidgetItem(tableWidget, current);

    MissionManagerTableWidgetConnect();

    //Draw3DLinesWeaponID();

    _recalcMissionFlightStatus();
}

MissionItem *UAVFlyManage_MavlinkMissionManagerDialog::getSelecteMissionItem()
{
    if(!ui){
        return nullptr;
    }

    if(!ui->tableWidget){
        return nullptr;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return nullptr;
    }

    const int currentRow = tableWidget->currentRow();
    if(currentRow < 0 || currentRow > tableWidget->rowCount()){
        return nullptr;
    }

    if(currentRow >= m_MissionItems.size()){
        return nullptr;
    }

    for(int i = 0; i < m_MissionItems.size(); i++){
        auto* missionItem = m_MissionItems.at(i);
        if(missionItem && missionItem->sequenceNumber() == currentRow){
            return missionItem;
        }
    }

    return nullptr;
}

void UAVFlyManage_MavlinkMissionManagerDialog::MissionManagerTableWidgetDisconnect()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    disconnect(ui->tableWidget, &QTableWidget::cellChanged, this, &UAVFlyManage_MavlinkMissionManagerDialog::MissionManager_tableWidget_cellChanged);
}

void UAVFlyManage_MavlinkMissionManagerDialog::MissionManagerTableWidgetConnect()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }
    connect(tableWidget, &QTableWidget::cellChanged, this, &UAVFlyManage_MavlinkMissionManagerDialog::MissionManager_tableWidget_cellChanged);
}

void UAVFlyManage_MavlinkMissionManagerDialog::MissionManagerTableWidgetUpdate()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    _recalcMissionFlightStatus();

    tableWidget->setVisible(false);

    tableWidget->clearContents();
    tableWidget->clear();
    tableWidget->setRowCount(m_MissionItems.size());
    {
        const QStringList titleList =
        {

            u8"指令",
            u8"高度类型",
            u8"参数1",
            u8"参数2",
            u8"参数3",
            u8"参数4",
            u8"纬度",
            u8"经度",
            u8"高度",

            u8"自动继续",
            u8"当前点",
            u8"距离",
        };

        tableWidget->setColumnCount(titleList.size());

        tableWidget->setHorizontalHeaderLabels(titleList);
    }

    MissionManagerTableWidgetDisconnect();

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    for(int row = 0; row < m_MissionItems.size(); row++){
        MissionManagerTableWidgetTableWidgetItem(tableWidget, row);
    }

    tableWidget->setColumnWidth(0,  170);
    tableWidget->setColumnWidth(1,  245);
    tableWidget->setColumnWidth(2,  90);
    tableWidget->setColumnWidth(3,  90);
    tableWidget->setColumnWidth(4,  90);
    tableWidget->setColumnWidth(5,  90);
    tableWidget->setColumnWidth(6,  110);
    tableWidget->setColumnWidth(7,  110);
    tableWidget->setColumnWidth(8,  90);

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Interactive);

    MissionManagerTableWidgetConnect();

    tableWidget->setVisible(true);
    //Draw3DLinesWeaponID();
}

void UAVFlyManage_MavlinkMissionManagerDialog::appendMissionManagerTableWidget()
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    if(!tableWidget->horizontalHeader()){
        return ;
    }

    if(tableWidget->horizontalHeader()->count() <= 0){
        return ;
    }

    _recalcMissionFlightStatus();

    tableWidget->setRowCount(m_MissionItems.size());
    const int row = (m_MissionItems.size() - 1);

    MissionManagerTableWidgetDisconnect();

    MissionManagerTableWidgetTableWidgetItem(tableWidget, row);

    MissionManagerTableWidgetConnect();

   //Draw3DLinesWeaponID();
}

void UAVFlyManage_MavlinkMissionManagerDialog::MissionManagerTableWidgetTableWidgetItem(QTableWidget *tableWidget, int row)
{
    if(!tableWidget->horizontalHeader()){
        return ;
    }

    if(tableWidget->horizontalHeader()->count() <= 0){
        return ;
    }

    if(row >= m_MissionItems.size() || row < 0){
        return ;
    }

    const auto* missionItem = m_MissionItems.at(row);

    int column = 0;

    QTableWidgetItem* seqItem = new QTableWidgetItem(QString::number(missionItem->sequenceNumber() + 1));
    if(seqItem){
        seqItem->setData(Qt::UserRole, missionItem->sequenceNumber());
        tableWidget->setVerticalHeaderItem(row, seqItem);
        seqItem->setToolTip(_commandToolTipDescription(missionItem));
    }

    QComboBox* comboxCommand = new QComboBox;
    if(comboxCommand){
        comboxCommand->setObjectName(QString::fromUtf8("comboxCommand"));

        bool findMavCmd = false;
        for (const auto& command: SupportMissionMavCmdList()) {
            comboxCommand->addItem(command.mav_cmd_friendlyName, command.mav_cmd);
            if(missionItem->command() == command.mav_cmd){
                findMavCmd = true;
            }
        }

        if(!findMavCmd){
            comboxCommand->addItem(SupportMissionMavCmdFriendlyName(missionItem->command()), missionItem->command());
        }

        tableWidget->setCellWidget(row, column, comboxCommand);

        qDebug() << SupportMissionMavCmdFriendlyName(missionItem->command());

        comboxCommand->setCurrentText(SupportMissionMavCmdFriendlyName(missionItem->command()));

        comboxCommand->setMaxVisibleItems(20);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboxCommand->sizePolicy().hasHeightForWidth());
        comboxCommand->setSizePolicy(sizePolicy);
        comboxCommand->setFrame(false);

    }else{
        QTableWidgetItem* itemCommand = new QTableWidgetItem(QString::number(missionItem->command()));
        if(itemCommand){
            itemCommand->setData(QTableWidgetItem::UserType, missionItem->command());
        }
        tableWidget->setItem(row, column, itemCommand);
    }
    column++;

    QComboBox* comboxFrame = new QComboBox;
    if(comboxFrame){

        comboxFrame->setObjectName(QString::fromUtf8("comboxFrame"));

        QString currentText;
        for (const auto& mavFrameInfo : SupportMissionMavFrameInfoList()) {
            comboxFrame->addItem(mavFrameInfo.label, mavFrameInfo.frame);
            if(mavFrameInfo.frame == missionItem->frame()){
                currentText = mavFrameInfo.label;
            }
        }

        tableWidget->setCellWidget(row, column, comboxFrame);

        if(currentText.isEmpty()){
            currentText = QString("Unknow frame(%1)").arg(missionItem->frame());
            comboxFrame->addItem(currentText, missionItem->frame());
        }

        comboxFrame->setCurrentText(currentText);

        comboxFrame->setMaxVisibleItems(20);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboxFrame->sizePolicy().hasHeightForWidth());
        comboxFrame->setSizePolicy(sizePolicy);
        comboxFrame->setFrame(false);

    }else{
        QTableWidgetItem* itemFrame = new QTableWidgetItem(QString::number(missionItem->frame()));
        if(itemFrame){
            itemFrame->setData(QTableWidgetItem::UserType, missionItem->frame());
        }
        tableWidget->setItem(row, column, itemFrame);
    }
    column++;

    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(missionItem->param1())));
    column++;

    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(missionItem->param2())));
    column++;

    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(missionItem->param3())));
    column++;

    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(missionItem->param4())));
    column++;

    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(missionItem->param5(), 'f', 7)));
    column++;

    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(missionItem->param6(), 'f', 7)));
    column++;

    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(missionItem->param7())));
    column++;

    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(missionItem->autoContinue())));
    column++;

    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(missionItem->isCurrentItem())));
    column++;

    double azimuth = 0, distance = 0, altDifference = 0;
    if(row != 0 && row < m_MissionItems.size() && _specifiesCoordinate(missionItem)){
        for(int prevItemIndex = row - 1; prevItemIndex >= 0; prevItemIndex--){
            MissionItem* prevItem = m_MissionItems.at(prevItemIndex);
            if(_specifiesCoordinate(prevItem)){
                _calcPrevWaypointValues(missionItem, prevItem, &azimuth, &distance, &altDifference);
                break;
            }
        }

        if((row + 1) < tableWidget->rowCount() && (row + 1) < m_MissionItems.size()){
            QTableWidgetItem* nextItemDist = tableWidget->item(row + 1, column);
            if(nextItemDist && _specifiesCoordinate(m_MissionItems.at(row + 1))){
                nextItemDist->setText("");
                double nextAzimuth       = 0;
                double nextDistance      = 0;
                double nextAltDifference = 0;
                for(int nextItemIndex = row + 1; nextItemIndex < m_MissionItems.size(); nextItemIndex++){
                    MissionItem* nextItem = m_MissionItems.at(nextItemIndex);
                    if(_specifiesCoordinate(nextItem)){
                        _calcPrevWaypointValues(nextItem, missionItem, &nextAzimuth, &nextDistance, &nextAltDifference);
                        nextItemDist->setText(QString::number(nextDistance, 'f', 2));
                        break;
                    }
                }
            }
        }
    }
    tableWidget->setItem(row, column, new QTableWidgetItem(QString::number(distance, 'f', 2)));

    column++;

    qDebug() << __FILE__ << __LINE__
             << missionItem->command         ()
             << missionItem->isCurrentItem   ()
             << missionItem->sequenceNumber  ()
             << missionItem->frame           ()
             << missionItem->autoContinue    ()
             << missionItem->param1          ()
             << missionItem->param2          ()
             << missionItem->param3          ()
             << missionItem->param4          ()
             << missionItem->param5          ()
             << missionItem->param6          ()
             << missionItem->param7          ()
             << missionItem->coordinate      ()
                ;

    connect(comboxCommand, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UAVFlyManage_MavlinkMissionManagerDialog::MissionManagerTableWidgetComboxChanged);
    connect(comboxFrame, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UAVFlyManage_MavlinkMissionManagerDialog::MissionManagerTableWidgetComboxChanged);
}

void UAVFlyManage_MavlinkMissionManagerDialog::UpdateMissionManagerTableWidgetTableHorizontalHeader(int row)
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    const auto currentRow = tableWidget->currentRow();
    if(currentRow != row){
        return ;
    }

    if(m_MissionItems.size() <= row || row < 0){
        return ;
    }

    {
        for(int paraColumn1_7 = 2; paraColumn1_7 < 9; paraColumn1_7++){
            QTableWidgetItem * itemParm1_7 = tableWidget->horizontalHeaderItem(paraColumn1_7);
            if(itemParm1_7){

                itemParm1_7->setText(QString(u8"参数%1").arg(paraColumn1_7 - 1));
            }
        }
    }

    const auto* item = m_MissionItems.at(row);
    const MissionCommandUIInfo* uiInfo = QGCApplication::missionCommandTree()->getUIInfo(m_vehicle, m_vehicle->vehicleClass(), item->command());

    if(m_MavlinkMissionManagerMissionCommandListEnable && m_MavlinkMissionManagerMissionCommandList != nullptr){
        const auto* commandUIInfo = m_MavlinkMissionManagerMissionCommandList->getUIInfo(item->command());
        if(commandUIInfo != nullptr){
            uiInfo = commandUIInfo;
        }
    }

    if (uiInfo) {
        QString toolTipDescription = uiInfo->category();
        toolTipDescription += QString(":") + uiInfo->description();
        toolTipDescription += QString("\n") + uiInfo->friendlyName();
        toolTipDescription += QString(":") +uiInfo->rawName();

        bool showUI = true;

        int paraColumn1_7 = 2;

        for(int index = 1; index <= 7; index++){
            const MissionCmdParamInfo* paramInfo = uiInfo->getParamInfo(index, showUI);
            if(paramInfo != nullptr){
                toolTipDescription += QString("\n") + QString("param") + QString::number(paramInfo->param());
                toolTipDescription += QString("") + QString(":") + paramInfo->label();
                toolTipDescription += QString(", ") + QString("decimalPlaces:") + QString::number(paramInfo->decimalPlaces());
                toolTipDescription += QString(", ") + QString("defaultValue:") + QString::number(paramInfo->defaultValue(), 'f', 7);

                QString enumStringsList("");
                for(const QString& enumString : paramInfo->enumStrings()){
                    enumStringsList += enumString + QString(",");
                }

                toolTipDescription += QString(", ") + QString("enumStrings:") + enumStringsList;

                QString enumValuesList("");
                for(const auto& enumValue : paramInfo->enumValues()){
                    enumValuesList += enumValue.toString() + QString(",");
                }
                toolTipDescription += QString(", ") + QString("enumValues:") + enumValuesList;

                toolTipDescription += QString(", ") + QString("units:") + paramInfo->units();
                toolTipDescription += QString(", ") + QString("nanUnchanged:") + QString::number(paramInfo->nanUnchanged());

                QTableWidgetItem * itemParm1_7 = tableWidget->horizontalHeaderItem(paraColumn1_7);
                if(itemParm1_7){
                    if(paramInfo->units().isEmpty()){
                        itemParm1_7->setText(paramInfo->label());
                    }else{
                        itemParm1_7->setText(paramInfo->label() + "("+ paramInfo->units() +")");
                    }
                }
            }

            paraColumn1_7++;
        }

        if(uiInfo->isStandaloneCoordinate() || uiInfo->specifiesCoordinate() || uiInfo->specifiesCoordinate()){
            int paraColumn5_7 = 6;

            QTableWidgetItem * itemParm5_7 = tableWidget->horizontalHeaderItem(paraColumn5_7);
            if(itemParm5_7){

                itemParm5_7->setText(QString(u8"纬度"));
            }
            paraColumn5_7++;

            itemParm5_7 = tableWidget->horizontalHeaderItem(paraColumn5_7);
            if(itemParm5_7){

                itemParm5_7->setText(QString(u8"经度"));
            }
            paraColumn5_7++;

            itemParm5_7 = tableWidget->horizontalHeaderItem(paraColumn5_7);
            if(itemParm5_7){

                itemParm5_7->setText(QString(u8"高度"));
            }
        }

        qWarning() << "toolTipDescription:"<<toolTipDescription;
    } else {
        qWarning() << "Should not ask for command description on unknown command";
    }
}

void UAVFlyManage_MavlinkMissionManagerDialog::changeEvent(QEvent *e)
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

void UAVFlyManage_MavlinkMissionManagerDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}

void UAVFlyManage_MavlinkMissionManagerDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    m_Selected = false;
    m_SequenceNumberSelected = -1;

    if(QGCApplication::upDownVehicleAllMissionEnabled()){
        stopMissionPlanNoStopLoadOrWriteVehicleMission();
    }
    else{
        stopMissionPlan();
    }
}

Vehicle *UAVFlyManage_MavlinkMissionManagerDialog::getVehicle() const
{
    return m_vehicle;
}

QString UAVFlyManage_MavlinkMissionManagerDialog::getVehicleInfo() const
{
    if(getVehicle()){
        return (" " + getVehicle()->firmwareTypeString() + " : " + getVehicle()->vehicleTypeString());
    }
    return QString("No Vehicle!");
}

bool UAVFlyManage_MavlinkMissionManagerDialog::getSelected() const
{
    return m_Selected;
}

void UAVFlyManage_MavlinkMissionManagerDialog::setSelected(bool Selected)
{
    m_Selected = Selected;
}

unsigned int UAVFlyManage_MavlinkMissionManagerDialog::getColor() const
{
    return m_Color;
}

void UAVFlyManage_MavlinkMissionManagerDialog::setColor(unsigned int Color)
{
    m_Color = Color;
}

bool UAVFlyManage_MavlinkMissionManagerDialog::MissionLineShow() const
{
    return m_MissionLineShow;
}

void UAVFlyManage_MavlinkMissionManagerDialog::setMissionLineShow(bool newMissionLineShow)
{
	ui->groupBox_UAVFlyManage_MavlinkTaskManage_LineSettings->setEnabled(true);

    if(!ui){
        return ;
    }

    if(!ui->checkBox_VehicleUpDownMissionHide){
        return ;
    }

    ui->checkBox_VehicleUpDownMissionHide->setCheckState( (newMissionLineShow ? Qt::CheckState::Unchecked : Qt::CheckState::Checked));
}

int UAVFlyManage_MavlinkMissionManagerDialog::getSequenceNumberSelected() const
{
    return m_SequenceNumberSelected;
}

void UAVFlyManage_MavlinkMissionManagerDialog::setSequenceNumberSelected(int sequenceNumberSelected)
{
    m_SequenceNumberSelected = sequenceNumberSelected;
}

MAV_FRAME UAVFlyManage_MavlinkMissionManagerDialog::getMavFrameDefault() const
{
    return m_MavFrameDefault;
}

void UAVFlyManage_MavlinkMissionManagerDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
    qDebug() << "on_tableWidget_cellDoubleClicked:" << row << column;

    if(column != 0){
        return ;
    }

    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    if(m_MissionItems.size() <= row || row < 0){
        return ;
    }

    const auto currentRow = tableWidget->currentRow();
    if(currentRow != row){
        qDebug() << "on_tableWidget_currentCellChanged:" << row << column;
    }

    bool ok = false;
    int sequenceNumber = 0;
    QTableWidgetItem* seqItem = tableWidget->verticalHeaderItem(row);
    if(seqItem){
        sequenceNumber = seqItem->data(Qt::UserRole).toInt(&ok);
    }

    if(currentRow != sequenceNumber){
        qDebug() << "on_tableWidget_currentCellChanged:" << row << column;
    }

    auto* missionItem = m_MissionItems.at(row);

    LY_UAVFlyControl_Mavlink::Get().SetScreenCenter(missionItem->param6(), missionItem->param5());
}

void UAVFlyManage_MavlinkMissionManagerDialog::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    qDebug() << "on_tableWidget_currentCellChanged:" << currentRow << currentColumn << previousRow << previousColumn;

    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    const auto row    = tableWidget->currentRow();
    if(currentRow != row){
        return ;
    }

    if(m_MissionItems.size() <= row){
        return ;
    }

    UpdateMissionManagerTableWidgetTableHorizontalHeader(row);
}

void UAVFlyManage_MavlinkMissionManagerDialog::MissionManager_tableWidget_cellChanged(int row, int column)
{
    qDebug() << "on_tableWidget_cellChanged:" << row << column << sender()->metaObject()->className() << sender();
    MissionManagerTableWidgetEditCellChanged(row, column);
    //Draw3DLinesWeaponID();

    _recalcMissionFlightStatus();
}

void UAVFlyManage_MavlinkMissionManagerDialog::MissionManagerTableWidgetComboxChanged(int index)
{
    qDebug() << "MissionManagerTableWidgetComboxChanged:" << index;
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    const auto row    = tableWidget->currentRow();
    const auto column = tableWidget->currentColumn();
    if(m_MissionItems.size() <= row){
        return ;
    }

    MissionManagerTableWidgetEditCellChanged(row, column);

    if(tableWidget){

        if(m_MissionItems.size() <= row || row < 0){
            return ;
        }

        const auto currentRow = tableWidget->currentRow();
        if(currentRow != row){
            qDebug() << "on_tableWidget_currentCellChanged:" << row << column;
        }

        bool ok = false;
        int sequenceNumber = 0;
        QTableWidgetItem* seqItem = tableWidget->verticalHeaderItem(row);
        if(seqItem){
            sequenceNumber = seqItem->data(Qt::UserRole).toInt(&ok);
        }

        if(currentRow != sequenceNumber){
            qDebug() << "on_tableWidget_currentCellChanged:" << row << column;
        }

        auto* missionItem = m_MissionItems.at(row);

        int columnItem = 0;

        bool find_mav_cmd_nav_return_to_launch = false;
        QComboBox* comboxCommand = qobject_cast<QComboBox*>(tableWidget->cellWidget(row, columnItem));
        if(comboxCommand){
            const auto command = comboxCommand->currentData(Qt::UserRole).toUInt(&ok);
            if(ok && HasSupportMissionMavCmd((MAV_CMD)command) && (MAV_CMD)command == MAV_CMD::MAV_CMD_NAV_RETURN_TO_LAUNCH){
                find_mav_cmd_nav_return_to_launch = true;
            }
        }

        if(find_mav_cmd_nav_return_to_launch){
            columnItem++;

            QComboBox* comboxFrame = qobject_cast<QComboBox*>(tableWidget->cellWidget(row, columnItem));
            if(comboxFrame){
                const auto frame = comboxFrame->currentData(Qt::UserRole).toUInt(&ok);
                if(ok && HasSupportMissionMavFrameInfo((MAV_FRAME)frame)){

                }
            }
            columnItem++;

            QTableWidgetItem* itemParam1 = tableWidget->item(row, columnItem);
            if(itemParam1){
                const double param1 = itemParam1->text().toDouble(&ok);
                if(ok){

                }
            }
            columnItem++;

            QTableWidgetItem* itemParam2 = tableWidget->item(row, columnItem);
            if(itemParam2){
                const double param2 = itemParam2->text().toDouble(&ok);
                if(ok){

                }
            }
            columnItem++;

            QTableWidgetItem* itemParam3 = tableWidget->item(row, columnItem);
            if(itemParam3){
                const double param3 = itemParam3->text().toDouble(&ok);
                if(ok){

                }
            }
            columnItem++;

            QTableWidgetItem* itemParam4 = tableWidget->item(row, columnItem);
            if(itemParam4){
                const double param4 = itemParam4->text().toDouble(&ok);
                if(ok){
                    missionItem->setParam4(param4);
                }

                if(ok && qIsNaN(param4)){
                    missionItem->setParam4(0);
                    itemParam4->setText("0");
                }
            }
            columnItem++;
        }

    }
    //Draw3DLinesWeaponID();

    _recalcMissionFlightStatus();
}

void UAVFlyManage_MavlinkMissionManagerDialog::MissionManagerTableWidgetEditCellChanged(int row, int column)
{
    if(!ui){
        return ;
    }

    if(!ui->tableWidget){
        return ;
    }

    auto *tableWidget = ui->tableWidget;
    if(!tableWidget){
        return ;
    }

    if(m_MissionItems.size() <= row || row < 0){
        return ;
    }

    const auto currentRow = tableWidget->currentRow();
    if(currentRow != row){
        qDebug() << "on_tableWidget_currentCellChanged:" << row << column;
    }

    bool ok = false;
    int sequenceNumber = 0;
    QTableWidgetItem* seqItem = tableWidget->verticalHeaderItem(row);
    if(seqItem){
        sequenceNumber = seqItem->data(Qt::UserRole).toInt(&ok);
    }

    if(currentRow != sequenceNumber){
        qDebug() << "on_tableWidget_currentCellChanged:" << row << column;
    }

    auto* missionItem = m_MissionItems.at(row);

    int columnItem = 0;

    QComboBox* comboxCommand = qobject_cast<QComboBox*>(tableWidget->cellWidget(row, columnItem));
    if(comboxCommand){
        const auto command = comboxCommand->currentData(Qt::UserRole).toUInt(&ok);
        if(ok && HasSupportMissionMavCmd((MAV_CMD)command)){
            missionItem->setCommand((MAV_CMD)command);
        }
    }
    columnItem++;

    QComboBox* comboxFrame = qobject_cast<QComboBox*>(tableWidget->cellWidget(row, columnItem));
    if(comboxFrame){
        const auto frame = comboxFrame->currentData(Qt::UserRole).toUInt(&ok);
        if(ok && HasSupportMissionMavFrameInfo((MAV_FRAME)frame)){
            missionItem->setFrame((MAV_FRAME)frame);
        }
    }
    columnItem++;

    QTableWidgetItem* itemParam1 = tableWidget->item(row, columnItem);
    if(itemParam1){
        const double param1 = itemParam1->text().toDouble(&ok);
        if(ok){
            missionItem->setParam1(param1);
        }
    }
    columnItem++;

    QTableWidgetItem* itemParam2 = tableWidget->item(row, columnItem);
    if(itemParam2){
        const double param2 = itemParam2->text().toDouble(&ok);
        if(ok){
            missionItem->setParam2(param2);
        }
    }
    columnItem++;

    QTableWidgetItem* itemParam3 = tableWidget->item(row, columnItem);
    if(itemParam3){
        const double param3 = itemParam3->text().toDouble(&ok);
        if(ok){
            missionItem->setParam3(param3);
        }
    }
    columnItem++;

    QTableWidgetItem* itemParam4 = tableWidget->item(row, columnItem);
    if(itemParam4){
        const double param4 = itemParam4->text().toDouble(&ok);
        if(ok){
            missionItem->setParam4(param4);
        }
    }
    columnItem++;

    QTableWidgetItem* itemLatX = tableWidget->item(row, columnItem);
    if(itemLatX){
        const double latX = itemLatX->text().toDouble(&ok);
        if(ok){
            missionItem->setParam5(latX);
        }
    }
    columnItem++;

    QTableWidgetItem* itemLonY = tableWidget->item(row, columnItem);
    if(itemLonY){
        const double lonY = itemLonY->text().toDouble(&ok);
        if(ok){
            missionItem->setParam6(lonY);
        }
    }
    columnItem++;

    QTableWidgetItem* itemAltZ = tableWidget->item(row, columnItem);
    if(itemAltZ){
        const double altZ = itemAltZ->text().toDouble(&ok);
        if(ok){
            missionItem->setParam7(altZ);
        }
    }
    columnItem++;

    QTableWidgetItem* itemAutoContinue = tableWidget->item(row, columnItem);
    if(itemAutoContinue){
        const int autoContinue = itemAutoContinue->text().toInt(&ok);
        if(ok){
            missionItem->setAutoContinue((autoContinue != 0));
        }
    }
    columnItem++;

    QTableWidgetItem* itemIsCurrentItem = tableWidget->item(row, columnItem);
    if(itemIsCurrentItem){
        const int isCurrentItem = itemIsCurrentItem->text().toInt(&ok);
        if(ok){
            missionItem->setIsCurrentItem((isCurrentItem != 0));
        }
    }
    columnItem++;

    QTableWidgetItem* itemDist = tableWidget->item(row, columnItem);
    if(itemDist && row != 0 && row < m_MissionItems.size() && _specifiesCoordinate(missionItem)){
        double azimuth = 0, distance = 0, altDifference = 0;
        for(int prevItemIndex = row - 1; prevItemIndex >= 0; prevItemIndex--){
            MissionItem* prevItem = m_MissionItems.at(prevItemIndex);
            if(_specifiesCoordinate(prevItem)){
                _calcPrevWaypointValues(missionItem, prevItem, &azimuth, &distance, &altDifference);
                break;
            }
        }
        itemDist->setText(QString::number(distance, 'f', 2));

        QTableWidgetItem* nextItemDist = tableWidget->item(row + 1, columnItem);
        if(nextItemDist  && (row + 1) < tableWidget->rowCount() && (row + 1) < m_MissionItems.size() && _specifiesCoordinate(m_MissionItems.at(row + 1))){
            azimuth       = 0;
            distance      = 0;
            altDifference = 0;
            for(int nextItemIndex = row + 1; nextItemIndex < m_MissionItems.size(); nextItemIndex++){
                MissionItem* nextItem = m_MissionItems.at(nextItemIndex);
                if(_specifiesCoordinate(nextItem)){
                    _calcPrevWaypointValues(nextItem, missionItem, &azimuth, &distance, &altDifference);
                    break;
                }
            }
            nextItemDist->setText(QString::number(distance, 'f', 2));
        }
    }
    columnItem++;

    if(seqItem){
        seqItem->setToolTip(_commandToolTipDescription(missionItem));
    }
}

