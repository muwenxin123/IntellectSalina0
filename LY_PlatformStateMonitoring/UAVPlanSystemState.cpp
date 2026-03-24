#include "UAVPlanSystemState.h"
#include "ui_UAVPlanSystemState.h"
#include <QCString.h>

#include "UAVPlanTeleInformation.h"

#include "IPlatformStateMonitoringActivator.h"
#include "LY_IPlatformStateMonitoring.h"

#include <QObject>
#include <QTimer>
#include <QFile>
#include <QDateTime>
#include <QSettings>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "LY_IPlatformStateMonitoringInterface.h"
#include <QtMath>
#include "CommonStruct.h"
#include "lyUI/LyMessageBox.h"
#define UAVMOMENTPOSITION_SAFETY_DIS 10

const char* const UAVPlanSystemState_Vehicle_WeaponTelemetryInfoWarning           = "WeaponTelemetryInfoWarning";
const char* const UAVPlanSystemState_Vehicle_WeaponID                             = "WeaponID";
const char* const UAVPlanSystemState_PowervoltageWarningMin                       = "PowervoltageWarningMin";
const char* const UAVPlanSystemState_GpsNumberWarningMin                          = "GpsNumberWarningMin";
const char* const UAVPlanSystemState_AngleofPitchWarningMin                       = "AngleofPitchWarningMin";
const char* const UAVPlanSystemState_AngleofRollWarningMin                        = "AngleofRollWarningMin";

const char* const UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableSettings     = "WeaponTelemetryInfoTableSettings";
const char* const UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumn       = "TableColumn";
const char* const UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumnWidth  = "TableColumnWidth";

UAVPlanSystemState::UAVPlanSystemState(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVPlanSystemState)
    , m_Heartbeats(true)
    , m_msec(0)
    , m_msecHeartbeats(500)
    , m_CXYWeaponManage()
    , m_show(true)
{
	//this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	//this->setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
	resize(1160, 380);
    QSettings settings("../data/conf/FlightLogConfig.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.beginGroup("Heartbeats");
    m_Heartbeats = settings.value("heartbeats", "true").toBool();
    if(settings.contains("TimeMSceondHeartbeats") && settings.value("TimeMSceondHeartbeats").canConvert(QVariant::Type::UInt)){
        bool ok = false;
        unsigned int tempMsecHeartbeats = settings.value("TimeMSceondHeartbeats", "500").toUInt(&ok);
        if(tempMsecHeartbeats >= 500 && tempMsecHeartbeats <= 100000 && (m_msecHeartbeats % 100 == 0)){
            m_msecHeartbeats = tempMsecHeartbeats;
        }
    }
    settings.endGroup();

    m_WeaponComponentMap.clear();
    m_WeaponComponenTelemetryInformationMap.clear();
    m_WeaponTelemetryInfoTableSettingsMap.clear();

    loadWeaponTelemetryInfoWarningSetting();

    ui->setupUi(this);
    setTableWidget();

	m_pix_G.load(QString(":/images/StateGrey.png"));
	m_pix_B.load(QString(":/images/StateGreen.png"));
	m_pix_R.load(QString(":/images/StateRed.png"));

	m_proj = LY_IPlatformStateMonitoring::Get().GetProj();

	m_pITaskManageService = IPlatformStateMonitoringActivator::Get().getPTaskManageService();

	m_pIWeaponManageService = IPlatformStateMonitoringActivator::Get().getPIWeaponManageService();

	m_pIZoneManageService = IPlatformStateMonitoringActivator::Get().getPIZoneManageService();

	localWeaponNameBindToId();

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]()
	{
        qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService* pIUAVRealTimeStateManageService = IPlatformStateMonitoringActivator::Get().getPIUAVRealTimeStateManageService();
        if(pIUAVRealTimeStateManageService == nullptr){
            return ;
        }

        m_msec += 100;

        qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stateInfo;
        std::string strFoucsUAVID = "" , strProtocolType = "";
        const bool getUAVRealTimeState = pIUAVRealTimeStateManageService->GetDefaultUAVRealTimeState(strFoucsUAVID, strProtocolType, stateInfo);
        if(getUAVRealTimeState){
            const QString& strWeaponComponentID   = QString::fromStdString(strFoucsUAVID);
            QString strWeaponComponentName        = strWeaponComponentID;
            QMap<QString, QString>::const_iterator iter = m_WeaponComponentMap.find(strWeaponComponentID);
            if(iter != m_WeaponComponentMap.end()){
                strWeaponComponentName = iter.value();
            }

            QString strWeaponComponentcontrolMode(UAVPlatformStateDialog::tr("[Invalid]"));
            if(stateInfo.telemetryInfo.controlMode == ECtrlMode::ECtrlMode_Custom){
                strWeaponComponentcontrolMode = QString::fromStdString(stateInfo.strflightMode);
            }
            else {
                strWeaponComponentcontrolMode = (ECtrlModeToString(ECtrlMode(stateInfo.telemetryInfo.controlMode)));
            }
            UAVPlaneStatesDialog * pUAVPlaneStatesDialog = LY_IPlatformStateMonitoring::Get().getPUAVPlaneStatesDialog();
            if (pUAVPlaneStatesDialog) {
                pUAVPlaneStatesDialog->updateTelemetryInformation(strWeaponComponentName, strWeaponComponentID, stateInfo.telemetryInfo);
            }

            UAVPlatformStateDialog * pUAVPlatformStateDialog = LY_IPlatformStateMonitoring::Get().getPUAVPlatformStateDialog();
            if (pUAVPlatformStateDialog) {
                pUAVPlatformStateDialog->updateTelemetryInformation(strWeaponComponentName, strWeaponComponentID, strWeaponComponentcontrolMode, stateInfo.telemetryInfo);
                pUAVPlatformStateDialog->updateAttitude();
            }

			UAVPlaneStare_ForFire * pUAVPlanStare_forFire = LY_IPlatformStateMonitoring::Get().getPUAVPlanStare_ForFire();
			if (pUAVPlanStare_forFire) {
				pUAVPlanStare_forFire->updateTelemetryInformation(strWeaponComponentName, strWeaponComponentID, stateInfo.telemetryInfo);
			}

#ifdef QWT_CONFIG_ENABLE_DEFINES
            if(m_msec == 500){
                UAVPlanRealTimeAlititudeDialog * pUAVPlanRealTimeAlititudeDialog = LY_IPlatformStateMonitoring::Get().getPUAVPlanRealTimeAlititudeDialog();
                if (pUAVPlanRealTimeAlititudeDialog) {
                    pUAVPlanRealTimeAlititudeDialog->updateTelemetryInformation(strWeaponComponentName, strWeaponComponentID, stateInfo.telemetryInfo);
                }
            }
#endif
        }

        if(m_msec >= 1000){
            m_msec = 0;
#ifdef QWT_CONFIG_ENABLE_DEFINES
            UAVPlanRealTimeAlititudeDialog * pUAVPlanRealTimeAlititudeDialog = LY_IPlatformStateMonitoring::Get().getPUAVPlanRealTimeAlititudeDialog();
            if (getUAVRealTimeState && pUAVPlanRealTimeAlititudeDialog) {
                pUAVPlanRealTimeAlititudeDialog->updateRealTimeAlititude();
            }
#endif

            qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap nebulaUAVRealTimeStateMap;
            if(pIUAVRealTimeStateManageService->GetUAVRealTimeStateList(&nebulaUAVRealTimeStateMap)){

                for(const auto& nebulaUAVRealTimeState : nebulaUAVRealTimeStateMap){
                   const QString &strWeaponComponentID = QString::fromStdString(nebulaUAVRealTimeState.first);
                   QString strWeaponComponentcontrolMode(UAVPlatformStateDialog::tr("[Invalid]"));
                   if(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.controlMode == ECtrlMode::ECtrlMode_Custom){
                       strWeaponComponentcontrolMode = QString::fromStdString(nebulaUAVRealTimeState.second.m_telemetryInfo.strflightMode);
                   }
                   else {
                       strWeaponComponentcontrolMode = (ECtrlModeToString(ECtrlMode(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.controlMode)));
                   }
                    if(nebulaUAVRealTimeState.second.m_telemetryInfo.isout){

						updateTelemetryInformationTimeoutTree(strWeaponComponentID);

                    }else{

						updateTelemetryInformationTree(strWeaponComponentID, strWeaponComponentcontrolMode, nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo);
					}

                    updateTelemetryInformationWarningTips(strWeaponComponentID, nebulaUAVRealTimeState.second.m_telemetryInfo);
                }
            }
        }

        if(m_Heartbeats && m_msec == m_msecHeartbeats){
            qnzkna::SystemConfig::IConfigProjectControlService*  pIConfigProjectControlService  = IPlatformStateMonitoringActivator::Get().getPIConfigProjectControlService();
            qnzkna::WeaponManage::IWeaponManageService*          pIWeaponManageService          = IPlatformStateMonitoringActivator::Get().getPIWeaponManageService();
            qnzkna::PositionManage::IPositionManageService*      pIPositionManageService        = IPlatformStateMonitoringActivator::Get().getPIPositionManageService();
            qnzkna::framework::IRegistry*                        pIRegistry                     = IPlatformStateMonitoringActivator::Get().getIRegistryImpl();

            if(!pIConfigProjectControlService || !pIWeaponManageService || !pIPositionManageService || !pIRegistry){
                return ;
            }

            const qnzkna::SystemConfig::SystemSeatInfo& systemSeatInfo = pIConfigProjectControlService->systemSeatInfo();
            qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
            std::string                          strNodeId = systemSeatInfo.SystemNodeId();

            qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
            if(!pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap)){
                return ;
            }
            for(const auto& weaponSystemIter : weaponSystemMap){

                const auto& mapWeaponComponent = weaponSystemIter.second.getMapWeaponComponent();
                for(const auto& mapWeaponComponentIter : mapWeaponComponent){
                    QString strWeaponComponentID   = QString::fromStdString(mapWeaponComponentIter.second.getComponentID());
                    QString strWeaponComponentName = QString::fromStdString(mapWeaponComponentIter.second.getComponentName());

                    const std::string& strProtocolType(mapWeaponComponentIter.second.getProtocolType());
                    XYWeaponIDSetting xyWeaponIDSetting;
                    if(m_CXYWeaponManage.GetXYWeaponIDSetting(mapWeaponComponentIter.second.getComponentID(), xyWeaponIDSetting)
                            && (strProtocolType == std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
                                || strProtocolType == std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54))){
                        qnzkna::framework::MessageReferenceProps  props;
                        props.put("SourceType",   std::to_string(nNodeType));
                        props.put("SourceID",     strNodeId);
                        props.put("TargetType",   std::to_string(NODE_TYPE_WEAPONCOMPONENT));
                        props.put("TargetID",     xyWeaponIDSetting.strChannelID);
                        props.put("WeaponID",     mapWeaponComponentIter.second.getComponentID());
                        props.put("ProtocolType", strProtocolType);
                        xygsJL_A3A3 xygsjl_A3A3;
                        xygsjl_A3A3.flags = 0xA3A3;
                        xygsjl_A3A3.groupIndex = 0;
                        xygsjl_A3A3.planeIndex = 0;
                        pIRegistry->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", (const char*)(&xygsjl_A3A3), sizeof(xygsjl_A3A3), props));
                    }
                }
            }
        }
    });
    timer->start(100);

	ui->tableWidget_UAVPlanSystemState->hide();
}

UAVPlanSystemState::~UAVPlanSystemState()
{
    if(ui && ui->tableWidget_UAVPlanSystemState){
        for(int i = 0;i < ui->tableWidget_UAVPlanSystemState->columnCount();i++){
            WeaponTelemetryInfoTableSettings_t weaponTelemetryInfoTableSettings_t;
            weaponTelemetryInfoTableSettings_t.tableColumn         = i;
            weaponTelemetryInfoTableSettings_t.tableColumnWidth    = ui->tableWidget_UAVPlanSystemState->columnWidth(i);

            m_WeaponTelemetryInfoTableSettingsMap.insert(weaponTelemetryInfoTableSettings_t.tableColumn, weaponTelemetryInfoTableSettings_t);
        }
    }

    saveWeaponTelemetryInfoWarningSetting();
    delete ui;
}

void UAVPlanSystemState::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    switch (event->type()) {
    case QEvent::LanguageChange:
    {
        ui->retranslateUi(this);
        if(ui->tableWidget_UAVPlanSystemState){
            QStringList listheader;
            listheader<<UAVPlanSystemState::tr("Name")<<UAVPlanSystemState::tr("Link")<<UAVPlanSystemState::tr("State")<<UAVPlanSystemState::tr("Control")<<UAVPlanSystemState::tr("Phase")<<UAVPlanSystemState::tr("Voltage")
                     <<UAVPlanSystemState::tr("Current")<<UAVPlanSystemState::tr("Lat and Lng")<<UAVPlanSystemState::tr("Height")<<UAVPlanSystemState::tr("Speed")<<UAVPlanSystemState::tr("Posture")
                    <<UAVPlanSystemState::tr("Direction")<<UAVPlanSystemState::tr("Lock")<<UAVPlanSystemState::tr("Distance")<<UAVPlanSystemState::tr("Time");

            ui->tableWidget_UAVPlanSystemState->setHorizontalHeaderLabels(listheader);
        }
    }
        break;
    default:
        break;
    }
}

void UAVPlanSystemState::showEvent(QShowEvent *event)
{
    m_show = true;
}

void UAVPlanSystemState::hideEvent(QHideEvent *event)
{
    m_show = false;
}

void UAVPlanSystemState::loadWeaponTelemetryInfoWarningSetting()
{
    const QString& weaponTelemetryInfoWarningJsonFilename(QStringLiteral("../data/conf/WeaponTelemetryInfoWarning.json"));

    QFile loadFile(weaponTelemetryInfoWarningJsonFilename);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open read file.");
        return ;
    }

    QByteArray weaponTelemetryInfoWarning = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(weaponTelemetryInfoWarning));

    const QJsonObject &json = loadDoc.object();
    if (!json.contains(UAVPlanSystemState_Vehicle_WeaponTelemetryInfoWarning)){
        return ;
    }

    const QJsonArray& jsonArray = json[UAVPlanSystemState_Vehicle_WeaponTelemetryInfoWarning].toArray();
    {
        for (int i=0; i<jsonArray.count(); i++) {
            QJsonValue jsonValue = jsonArray.at(i);
            if (!jsonValue.isObject()) {
                qWarning() << QStringLiteral("JsonValue at index %1 not an object").arg(i);
                continue;
            }
            QJsonObject jsonObject = jsonValue.toObject();

            if( jsonObject.contains(UAVPlanSystemState_Vehicle_WeaponID) && jsonObject.value(UAVPlanSystemState_Vehicle_WeaponID).isString() &&
                    jsonObject.contains(UAVPlanSystemState_PowervoltageWarningMin) && jsonObject.value(UAVPlanSystemState_PowervoltageWarningMin).isDouble() &&
                    jsonObject.contains(UAVPlanSystemState_GpsNumberWarningMin) && jsonObject.value(UAVPlanSystemState_GpsNumberWarningMin).isDouble() &&
                    jsonObject.contains(UAVPlanSystemState_AngleofPitchWarningMin) && jsonObject.value(UAVPlanSystemState_AngleofPitchWarningMin).isDouble() &&
                    jsonObject.contains(UAVPlanSystemState_AngleofRollWarningMin) && jsonObject.value(UAVPlanSystemState_AngleofRollWarningMin).isDouble()
                    ){
                const auto& weaponID = jsonObject.value(UAVPlanSystemState_Vehicle_WeaponID).toString();
                if(weaponID.isEmpty()){
                    continue ;
                }

                const double powervoltageWarningMin = jsonObject.value(UAVPlanSystemState_PowervoltageWarningMin).toDouble(-1);
                if(powervoltageWarningMin < 0){
                    continue ;
                }

                const double gpsNumberWarningMin = jsonObject.value(UAVPlanSystemState_GpsNumberWarningMin).toDouble(-1);
                if(gpsNumberWarningMin <= 0){
                    continue ;
                }

                const double angleofPitchWarningMin = jsonObject.value(UAVPlanSystemState_AngleofPitchWarningMin).toDouble(-1);
                if(angleofPitchWarningMin <= 0){
                    continue ;
                }

                const double angleofRollWarningMin = jsonObject.value(UAVPlanSystemState_AngleofRollWarningMin).toDouble(-1);
                if(angleofRollWarningMin <= 0){
                    continue ;
                }

                Information_Warning_t information_Warning_t;
                information_Warning_t.powervoltageWarningMin = powervoltageWarningMin;
                information_Warning_t.gpsNumberWarningMin    = gpsNumberWarningMin;
                information_Warning_t.angleofPitchWarningMin = angleofPitchWarningMin;
                information_Warning_t.angleofRollWarningMin  = angleofRollWarningMin;

                m_WeaponComponenTelemetryInformationMap.insert(weaponID, information_Warning_t);
            }
        }
    }

    if(json.contains(UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableSettings)){
        const QJsonArray& jsonArrayTableSettings = json[UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableSettings].toArray();
        for (int i = 0; i < jsonArrayTableSettings.count(); i++) {
            QJsonValue jsonValue = jsonArrayTableSettings.at(i);
            if (!jsonValue.isObject()) {
                qWarning() << QStringLiteral("JsonValue at index %1 not an object").arg(i);
                continue;
            }
            QJsonObject jsonObject = jsonValue.toObject();

            if( jsonObject.contains(UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumn) && jsonObject.value(UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumn).isDouble() &&
                    jsonObject.contains(UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumnWidth) && jsonObject.value(UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumnWidth).isDouble()
                    ){

                const double tableColumn = jsonObject.value(UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumn).toDouble(-1);
                if(tableColumn < 0){
                    continue ;
                }

                const double tableColumnWidth = jsonObject.value(UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumnWidth).toDouble(-1);
                if(tableColumnWidth <= 0){
                    continue ;
                }

                WeaponTelemetryInfoTableSettings_t weaponTelemetryInfoTableSettings_t;
                weaponTelemetryInfoTableSettings_t.tableColumn         = static_cast<int>(tableColumn);
                weaponTelemetryInfoTableSettings_t.tableColumnWidth    = static_cast<int>(tableColumnWidth);

                m_WeaponTelemetryInfoTableSettingsMap.insert(weaponTelemetryInfoTableSettings_t.tableColumn, weaponTelemetryInfoTableSettings_t);
            }
        }
    }
}

void UAVPlanSystemState::saveWeaponTelemetryInfoWarningSetting()
{
     const QString& weaponTelemetryInfoWarningJsonFilename(QStringLiteral("../data/conf/WeaponTelemetryInfoWarning.json"));

     QJsonArray arrayWeaponComponenTelemetryInformation;
     auto telemetryInfoIter = m_WeaponComponenTelemetryInformationMap.constBegin();
     for(;telemetryInfoIter != m_WeaponComponenTelemetryInformationMap.constEnd(); telemetryInfoIter++){
         QJsonObject objectDeflectionDistYawPitch;
         objectDeflectionDistYawPitch[UAVPlanSystemState_Vehicle_WeaponID]                = telemetryInfoIter.key();
         objectDeflectionDistYawPitch[UAVPlanSystemState_PowervoltageWarningMin]          = telemetryInfoIter.value().powervoltageWarningMin;
         objectDeflectionDistYawPitch[UAVPlanSystemState_GpsNumberWarningMin]             = (int)telemetryInfoIter.value().gpsNumberWarningMin;
         objectDeflectionDistYawPitch[UAVPlanSystemState_AngleofPitchWarningMin]          = telemetryInfoIter.value().angleofPitchWarningMin;
         objectDeflectionDistYawPitch[UAVPlanSystemState_AngleofRollWarningMin]           = telemetryInfoIter.value().angleofRollWarningMin;
         arrayWeaponComponenTelemetryInformation.append(objectDeflectionDistYawPitch);
     }

     QJsonArray arrayWeaponComponenTelemetryInfoTableSettings;
     auto tableSettingsIter = m_WeaponTelemetryInfoTableSettingsMap.constBegin();
     for(;tableSettingsIter != m_WeaponTelemetryInfoTableSettingsMap.constEnd(); tableSettingsIter++){
         QJsonObject objectTableSettings;
         objectTableSettings[UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumn]          = tableSettingsIter.value().tableColumn;
         objectTableSettings[UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableColumnWidth]     = tableSettingsIter.value().tableColumnWidth;
         arrayWeaponComponenTelemetryInfoTableSettings.append(objectTableSettings);
     }

     QJsonObject json;
     json[UAVPlanSystemState_Vehicle_WeaponTelemetryInfoWarning]       = arrayWeaponComponenTelemetryInformation;
     if(!arrayWeaponComponenTelemetryInfoTableSettings.isEmpty()){
         json[UAVPlanSystemState_Vehicle_WeaponTelemetryInfoTableSettings] = arrayWeaponComponenTelemetryInfoTableSettings;
     }

     QFile saveFile(weaponTelemetryInfoWarningJsonFilename);

     if (!saveFile.open(QIODevice::WriteOnly)) {
         qWarning("Couldn't open save file.");
         return ;
     }

     QJsonDocument saveDoc(json);
     saveFile.write(saveDoc.toJson());
}

void UAVPlanSystemState::InitAllState()
{
    qnzkna::SystemConfig::IConfigProjectControlService*  pIConfigProjectControlService  =  IPlatformStateMonitoringActivator::Get().getPIConfigProjectControlService();
    qnzkna::WeaponManage::IWeaponManageService*          pIWeaponManageService          =  IPlatformStateMonitoringActivator::Get().getPIWeaponManageService();
    qnzkna::PositionManage::IPositionManageService*      pIPositionManageService        =  IPlatformStateMonitoringActivator::Get().getPIPositionManageService();

    if(!pIConfigProjectControlService || !pIWeaponManageService || !pIPositionManageService){
        return ;
    }

    qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
    if(!pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap)){
        return ;
    }

    const qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = pIConfigProjectControlService->systemSeatInfo().SystemNodeType();
    std::string strNodeId =	pIConfigProjectControlService->systemSeatInfo().SystemNodeId();
    {
        qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
        if(!pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap)){
            return ;
        }

        int i = 0;
        int rowCount = 1;
        for(const auto& weaponSystemIter : weaponSystemMap){
            const auto& mapWeaponComponent = weaponSystemIter.second.getMapWeaponComponent();
            rowCount += mapWeaponComponent.size();
        }
        ui->tableWidget_UAVPlanSystemState->setRowCount(rowCount);
        ui->tableWidget_UAVPlanSystemState->setColumnCount(15);
        for(const auto& weaponSystemIter : weaponSystemMap){
            const auto& mapWeaponComponent = weaponSystemIter.second.getMapWeaponComponent();

            for(const auto& mapWeaponComponentIter : mapWeaponComponent){
                QString strWeaponComponentID   = QString::fromStdString(mapWeaponComponentIter.second.getComponentID());
                QString strWeaponComponentName = QString::fromStdString(mapWeaponComponentIter.second.getComponentName());
                if (mapWeaponComponentIter.second.getPWeaponModel() && mapWeaponComponentIter.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_RotorUAV) {
                    {
                        m_WeaponComponentMap.insert(strWeaponComponentID, strWeaponComponentName);
                        QTableWidgetItem *genderitem  = new QTableWidgetItem(strWeaponComponentName);
                        genderitem->setData(Qt::UserRole, strWeaponComponentID);

                        QTableWidgetItem *genderitem1 = new QTableWidgetItem(UAVPlanSystemState::tr("Disconnect"));
                        QTableWidgetItem *genderitem2 = new QTableWidgetItem(EPlatformStateToString(EPlatformState::EPlatformState_Null));
                        QTableWidgetItem *genderitem3 = new QTableWidgetItem(ECtrlModeToString((ECtrlMode)4));
                        QTableWidgetItem *genderitem4 = new QTableWidgetItem(EFlyPhaseToString((EFlyPhase)0));
                        QTableWidgetItem *genderitem5 = new QTableWidgetItem(QString::number(0));
                        QTableWidgetItem *genderitem6 = new QTableWidgetItem(QString::number(0));
                        QTableWidgetItem *genderitem7 = new QTableWidgetItem(QString("--"));
                        QTableWidgetItem *genderitem8 = new QTableWidgetItem(QString::number(0));
                        QTableWidgetItem *genderitem9 = new QTableWidgetItem(QString::number(0));
                        QTableWidgetItem *genderitem10 = new QTableWidgetItem(QString("--"));
                        QTableWidgetItem *genderitem11 = new QTableWidgetItem(QString("--"));
                        QTableWidgetItem *genderitem12 = new QTableWidgetItem(GetLockState((ELockState)4));
                        QTableWidgetItem *genderitem13 = new QTableWidgetItem(QString("--"));
                        QTableWidgetItem *genderitem14 = new QTableWidgetItem(QString("--"));

                        ui->tableWidget_UAVPlanSystemState->setItem(i,0,genderitem);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,1,genderitem1);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,2,genderitem2);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,3,genderitem3);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,4,genderitem4);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,5,genderitem5);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,6,genderitem6);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,7,genderitem7);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,8,genderitem8);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,9,genderitem9);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,10,genderitem10);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,11,genderitem11);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,12,genderitem12);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,13,genderitem13);
                        ui->tableWidget_UAVPlanSystemState->setItem(i,14,genderitem14);

                        genderitem1->setTextColor(Qt::red);
                        genderitem2->setTextColor(Qt::gray);
                        genderitem12->setTextColor(QColor(60,86,190));

                        genderitem1->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));

                        genderitem2->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));

                        genderitem1->setText(tr("--"));
                        genderitem2->setText(tr("--"));
                        genderitem3->setText(tr("--"));
                        genderitem4->setText(tr("--"));
                        genderitem5->setText(tr("--"));
                        genderitem6->setText(tr("--"));
                        genderitem7->setText(tr("--"));
                        genderitem8->setText(tr("--"));
                        genderitem9->setText(tr("--"));
                        genderitem10->setText(tr("--"));
                        genderitem11->setText(tr("--"));
                        genderitem12->setText(tr("--"));
                        genderitem13->setText(tr("--"));
                        genderitem14->setText(tr("--"));

                        genderitem->setTextAlignment(Qt::AlignCenter);
                        genderitem1->setTextAlignment(Qt::AlignCenter);
                        genderitem2->setTextAlignment(Qt::AlignCenter);
                        genderitem3->setTextAlignment(Qt::AlignCenter);
                        genderitem4->setTextAlignment(Qt::AlignCenter);
                        genderitem5->setTextAlignment(Qt::AlignCenter);
                        genderitem6->setTextAlignment(Qt::AlignCenter);
                        genderitem7->setTextAlignment(Qt::AlignCenter);
                        genderitem8->setTextAlignment(Qt::AlignCenter);
                        genderitem9->setTextAlignment(Qt::AlignCenter);
                        genderitem10->setTextAlignment(Qt::AlignCenter);
                        genderitem11->setTextAlignment(Qt::AlignCenter);
                        genderitem12->setTextAlignment(Qt::AlignCenter);
                        genderitem13->setTextAlignment(Qt::AlignCenter);
                        genderitem14->setTextAlignment(Qt::AlignCenter);
                        i++;
                    }
                }
            }
        }
    }
}

void UAVPlanSystemState::CreateTree()
{
	ui->treeWidget->clear();
	m_UAVTreeWidgetItemMap.clear();
	m_TreeHeader.clear();

	m_TreeHeader << u8"装备平台" << QString("挂载") << QString("通信状态") << QString("RC状态") << QString("模式") << QString("GPS状态") << QString("卫星数") << QString("阶段");
	m_TreeHeader << QString("电压") << QString("电流") << QString("经纬") << QString("高度") << QString("速度") << QString("姿态") << QString("任务阶段");
	ui->treeWidget->setHeaderLabels(m_TreeHeader);
	ui->treeWidget->setColumnCount(m_TreeHeader.size());

	for (int i = 0; i < m_TreeHeader.size(); i++)
	{
		if (i == 10 || i == 13 || i == 14)
		{
			ui->treeWidget->header()->setSectionResizeMode(i, QHeaderView::Fixed);
		}
		else
		{
			ui->treeWidget->header()->setSectionResizeMode(i, QHeaderView::Interactive);
		}
	}

	ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);

	ui->treeWidget->setColumnWidth(10, 200);
	ui->treeWidget->setColumnWidth(13, 200);
	ui->treeWidget->setColumnWidth(14, 500);

	auto weaponSystemItor = m_WeaponComponentMap.begin();
	while (weaponSystemItor != m_WeaponComponentMap.end())
	{

		QTreeWidgetItem* pRootItem = new QTreeWidgetItem(ui->treeWidget);
		pRootItem->setText(0, weaponSystemItor.value());
		pRootItem->setData(0, Qt::UserRole, weaponSystemItor.key());
		pRootItem->setTextAlignment(0, Qt::AlignCenter);

		QLabel *pic = new QLabel();
		pic->setPixmap(m_pix_G);
		pic->setAlignment(Qt::AlignCenter);
		ui->treeWidget->setItemWidget(pRootItem, 2, pic);

		for (int i = 3;i<m_TreeHeader.size() - 1;i++)
		{
			pRootItem->setText(i, tr("-"));
			pRootItem->setTextAlignment(i, Qt::AlignCenter);
		}

		QLabel *pLabel14 = new QLabel();
		pLabel14->setText("-");
		pLabel14->setAlignment(Qt::AlignCenter);
		ui->treeWidget->setItemWidget(pRootItem, 14, pLabel14);

		m_UAVTreeWidgetItemMap.insert(weaponSystemItor.key(), pRootItem);

		QTreeWidgetItem* CarChildItem = new QTreeWidgetItem(pRootItem);
		CarChildItem->setText(1, QString("吊舱"));
		CarChildItem->setTextAlignment(1, Qt::AlignCenter);
		QLabel *pic_3 = new QLabel();
		pic_3->setPixmap(m_pix_G);
		pic_3->setAlignment(Qt::AlignCenter);
		ui->treeWidget->setItemWidget(CarChildItem, 2, pic_3);

		//if (weaponSystemItor.key() == "110302")
		//{

		//	QTreeWidgetItem* DanChildItem = new QTreeWidgetItem(pRootItem);
		//	DanChildItem->setText(1, QString("灭火罐"));
		//	DanChildItem->setTextAlignment(1, Qt::AlignCenter);
		//	QLabel *pic2 = new QLabel();
		//	pic2->setPixmap(m_pix_G);
		//	pic2->setAlignment(Qt::AlignCenter);
		//	ui->treeWidget->setItemWidget(DanChildItem, 2, pic2);
		//}
		//else if (weaponSystemItor.key() == "110303")
		//{

		//	QTreeWidgetItem* DanChildItem = new QTreeWidgetItem(pRootItem);
		//	DanChildItem->setText(1, QString("灭火弹"));
		//	DanChildItem->setTextAlignment(1, Qt::AlignCenter);
		//	QLabel *pic2 = new QLabel();
		//	pic2->setPixmap(m_pix_G);
		//	pic2->setAlignment(Qt::AlignCenter);
		//	ui->treeWidget->setItemWidget(DanChildItem, 2, pic2);
		//}

		weaponSystemItor++;
	}

}

void UAVPlanSystemState::UpdateTreeComunicateStates(QTreeWidgetItem* pItem, int nStates)
{
	QLabel* plabel = qobject_cast<QLabel*>(ui->treeWidget->itemWidget(pItem, 2));
	if (plabel == nullptr)
	{
		return;
	}

	if (nStates == 1)
	{
		plabel->setPixmap(m_pix_B);
	}
	else if (nStates == 2)
	{
		plabel->setPixmap(m_pix_R);
	}
	else
	{
		plabel->setPixmap(m_pix_G);
	}

	plabel->setAlignment(Qt::AlignCenter);
}

void UAVPlanSystemState::UpdateTreeEventStates(const QString& strID, QTreeWidgetItem* pItem)
{
	QLabel* plabel = qobject_cast<QLabel*>(ui->treeWidget->itemWidget(pItem, 14));
	if (plabel == nullptr)
	{
		return;
	}

	auto itr = m_UAVEventMap.find(strID);
	if (itr == m_UAVEventMap.end())
	{
		return;
	}

	auto itrRealTime = m_UAVRealTimeEventMap.find(strID);
	if (itrRealTime == m_UAVRealTimeEventMap.end())
	{
		return;
	}

	QString strTipsStart,strRed,strEnd;
	int nNo = 0;
	bool bFinded = false;
	for (auto itrEvent:itr->second)
	{
		if (itrEvent == itrRealTime->second)
		{
			if (nNo == 0)
			{
				strRed += QString::fromLocal8Bit(EFireProEventToString((FireProEvent_Enum)itrEvent));
			}
			else
			{
				strRed += tr("→") + QString::fromLocal8Bit(EFireProEventToString((FireProEvent_Enum)itrEvent));
			}

			bFinded = true;
		}
		else
		{
			if (bFinded == true)
			{
				if (nNo == 0)
				{
					strEnd += QString::fromLocal8Bit(EFireProEventToString((FireProEvent_Enum)itrEvent));
				}
				else
				{
					strEnd += tr("→") + QString::fromLocal8Bit(EFireProEventToString((FireProEvent_Enum)itrEvent));
				}
			}
			else
			{
				if (nNo == 0)
				{
					strTipsStart += QString::fromLocal8Bit(EFireProEventToString((FireProEvent_Enum)itrEvent));
				}
				else
				{
					strTipsStart += tr("→") + QString::fromLocal8Bit(EFireProEventToString((FireProEvent_Enum)itrEvent));
				}
			}
		}

		nNo++;
	}

	QString strColor = "<html><head/><body><p align=\"center\">"+strTipsStart+"<span style=\" color:#00ff00;\">"+ strRed +"</span>"+ strEnd +"</p></body></html>";
	plabel->setText(strColor);
}

void UAVPlanSystemState::UpdateMissionStates(const EventInfo& stInfo)
{
	QMutexLocker locker(&sMutex);

	QString strID = stInfo.strID;
	int nType = (int)stInfo.eEventType;

	auto itr = m_UAVRealTimeEventMap.find(strID);
	if (itr != m_UAVRealTimeEventMap.end())
	{
		itr->second = nType;
	}
	else
	{
		m_UAVRealTimeEventMap.insert(std::make_pair(strID, nType));
	}
}

void UAVPlanSystemState::InitUAVEventStates()
{
	EventVec EventVec1, EventVec2, EventVec3;

	EventVec1.push_back((int)FireProEvent_Enum_None);
	EventVec1.push_back((int)FireProEvent_Enum_SYSTEM_Line);
	EventVec1.push_back((int)FireProEvent_Enum_SYSTEM_Unlock);
	EventVec1.push_back((int)FireProEvent_Enum_SYSTEM_Fly);
	EventVec1.push_back((int)FireProEvent_Enum_SYSTEM_Seek);
	EventVec1.push_back((int)FireProEvent_Enum_SYSTEM_FindTarget);
	EventVec1.push_back((int)FireProEvent_Enum_SYSTEM_RTL);
	EventVec1.push_back((int)FireProEvent_Enum_SYSTEM_Land);
	EventVec1.push_back((int)FireProEvent_Enum_SYSTEM_Lock);

	EventVec2.push_back((int)FireProEvent_Enum_None);
	EventVec2.push_back((int)FireProEvent_Enum_SYSTEM_Line);
	EventVec2.push_back((int)FireProEvent_Enum_SYSTEM_Unlock);
	EventVec2.push_back((int)FireProEvent_Enum_SYSTEM_Fly);
	EventVec2.push_back((int)FireProEvent_Enum_SYSTEM_StartPlet);
	EventVec2.push_back((int)FireProEvent_Enum_SYSTEM_Plet);
	EventVec2.push_back((int)FireProEvent_Enum_SYSTEM_RTL);
	EventVec2.push_back((int)FireProEvent_Enum_SYSTEM_Land);
	EventVec2.push_back((int)FireProEvent_Enum_SYSTEM_Lock);

	EventVec3.push_back((int)FireProEvent_Enum_None);
	EventVec3.push_back((int)FireProEvent_Enum_SYSTEM_Line);
	EventVec3.push_back((int)FireProEvent_Enum_SYSTEM_Unlock);
	EventVec3.push_back((int)FireProEvent_Enum_SYSTEM_Fly);
	EventVec3.push_back((int)FireProEvent_Enum_SYSTEM_StartFire);
	EventVec3.push_back((int)FireProEvent_Enum_SYSTEM_Fire);
	EventVec3.push_back((int)FireProEvent_Enum_SYSTEM_RTL);
	EventVec3.push_back((int)FireProEvent_Enum_SYSTEM_Land);
	EventVec3.push_back((int)FireProEvent_Enum_SYSTEM_Lock);

	m_UAVEventMap.insert(std::make_pair(tr("110301"), EventVec1));
	m_UAVEventMap.insert(std::make_pair(tr("110302"), EventVec2));
	m_UAVEventMap.insert(std::make_pair(tr("110303"), EventVec3));

	m_UAVRealTimeEventMap.insert(std::make_pair(tr("110301"), 0));
	m_UAVRealTimeEventMap.insert(std::make_pair(tr("110302"), 0));
	m_UAVRealTimeEventMap.insert(std::make_pair(tr("110303"), 0));
}

void UAVPlanSystemState::setTableWidget()
{
    ui->tableWidget_UAVPlanSystemState->setRowCount(1);
    ui->tableWidget_UAVPlanSystemState->setColumnCount(15);
    QStringList listheader;
    listheader<<UAVPlanSystemState::tr("Name")<<UAVPlanSystemState::tr("Link")<<UAVPlanSystemState::tr("State")<<UAVPlanSystemState::tr("Control")<<UAVPlanSystemState::tr("Phase")<<UAVPlanSystemState::tr("Voltage")
             <<UAVPlanSystemState::tr("Current")<<UAVPlanSystemState::tr("Lat and Lng")<<UAVPlanSystemState::tr("Height")<<UAVPlanSystemState::tr("Speed")<<UAVPlanSystemState::tr("Posture")
            <<UAVPlanSystemState::tr("Direction")<<UAVPlanSystemState::tr("Lock")<<UAVPlanSystemState::tr("Distance")<<UAVPlanSystemState::tr("Time");

    ui->tableWidget_UAVPlanSystemState->setHorizontalHeaderLabels(listheader);

    ui->tableWidget_UAVPlanSystemState->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableWidget_UAVPlanSystemState->verticalHeader()->setHidden(true);

    ui->tableWidget_UAVPlanSystemState->setShowGrid(false);
    ui->tableWidget_UAVPlanSystemState->setStyleSheet("QTableWidget::Item{border:0px solid rgb(70,82,106); border-bottom:1px solid rgb(70,82,106);}");

    ui->tableWidget_UAVPlanSystemState->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);

    ui->tableWidget_UAVPlanSystemState->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableWidget_UAVPlanSystemState->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableWidget_UAVPlanSystemState->setAlternatingRowColors(true);

    if(ui && ui->tableWidget_UAVPlanSystemState){
        for(int i = 0;i < ui->tableWidget_UAVPlanSystemState->columnCount();i++){
            auto tableSettingsIter = m_WeaponTelemetryInfoTableSettingsMap.constFind(i);
            if(tableSettingsIter != m_WeaponTelemetryInfoTableSettingsMap.constEnd()){
                if(tableSettingsIter.value().tableColumn == i && tableSettingsIter.value().tableColumnWidth > 0){
                    ui->tableWidget_UAVPlanSystemState->setColumnWidth(i, tableSettingsIter.value().tableColumnWidth);
                }
            }
        }
    }
}

void UAVPlanSystemState::updateTelemetryInformation(const QString &strWeaponComponentID, const QString &strWeaponComponentcontrolMode, const telemetryInformation &telemetryInfor)
{
    bool find = false;
    for(int i = 0;i < ui->tableWidget_UAVPlanSystemState->rowCount();i++)
    {
        QTableWidgetItem *genderitem = ui->tableWidget_UAVPlanSystemState->item(i, 0);
        if(genderitem == nullptr){
            continue;
        }
        if((genderitem == nullptr) || (genderitem && genderitem->data(Qt::UserRole) != strWeaponComponentID)){
            continue;
        }

        find = true;

        QTableWidgetItem *genderitem1 =  ui->tableWidget_UAVPlanSystemState->item(i,1);
        QTableWidgetItem *genderitem2 =  ui->tableWidget_UAVPlanSystemState->item(i,2);
        QTableWidgetItem *genderitem3 =  ui->tableWidget_UAVPlanSystemState->item(i,3);
        QTableWidgetItem *genderitem4 =  ui->tableWidget_UAVPlanSystemState->item(i,4);
        QTableWidgetItem *genderitem5 =  ui->tableWidget_UAVPlanSystemState->item(i,5);
        QTableWidgetItem *genderitem6 =  ui->tableWidget_UAVPlanSystemState->item(i,6);
        QTableWidgetItem *genderitem7 =  ui->tableWidget_UAVPlanSystemState->item(i,7);
        QTableWidgetItem *genderitem8 =  ui->tableWidget_UAVPlanSystemState->item(i,8);
        QTableWidgetItem *genderitem9 =  ui->tableWidget_UAVPlanSystemState->item(i,9);
        QTableWidgetItem *genderitem10 = ui->tableWidget_UAVPlanSystemState->item(i,10);
        QTableWidgetItem *genderitem11 = ui->tableWidget_UAVPlanSystemState->item(i,11);
        QTableWidgetItem *genderitem12 = ui->tableWidget_UAVPlanSystemState->item(i,12);
        QTableWidgetItem *genderitem13 = ui->tableWidget_UAVPlanSystemState->item(i,13);
        QTableWidgetItem *genderitem14 = ui->tableWidget_UAVPlanSystemState->item(i,14);

        genderitem1->setText(ECommStateToString(ECommState(telemetryInfor._status)));
        genderitem2->setText(EPlatformStateToString(GetPlatformState(telemetryInfor._status)));

        genderitem3->setText(strWeaponComponentcontrolMode);
        genderitem4->setText(EFlyPhaseToString(EFlyPhase(telemetryInfor.flyPeriod)));
        genderitem5->setText(QString("P:%1, %2").arg(telemetryInfor.powervoltage, 0, 'f', 2).arg(telemetryInfor.fkvoltage, 0, 'f', 2));
        genderitem6->setText(QString::number(telemetryInfor._current / 10.0,'f',1));
        genderitem7->setText(QString("G:%1, %2 %3").arg(telemetryInfor.gpsNumber).arg(telemetryInfor.position.lng, 0, 'f', 7).arg(telemetryInfor.position.lat, 0, 'f', 7));
        genderitem8->setText(QString("R:%1, %2").arg(telemetryInfor.relativeOriginHeigth, 0, 'f',2).arg(telemetryInfor.gpsHeigth * 10.0, 0, 'f',2));
        genderitem9->setText(QString::number(telemetryInfor.targetVelocity,'f',2));
        genderitem10->setText(QString("P:%1, R:%2, Y:%3").arg(telemetryInfor.angleofPitch, 0, 'f', 2).arg(telemetryInfor.angleofRoll, 0, 'f', 2).arg(telemetryInfor.angleofRudder, 0, 'f', 2));
        genderitem11->setText(QString::number(telemetryInfor._rudder,'f',2));

        genderitem12->setText(ELockStateToString(telemetryInfor.lockStatus));
        genderitem13->setText(QString::number(telemetryInfor.flyDistance));
        genderitem14->setText(QString::number(telemetryInfor.flyTime));

        const ECommState commState = ECommState::ECommState_Normal;
        if(ECommState::ECommState_Normal == ECommState(commState)){

            genderitem1->setTextColor(Qt::green);
            genderitem1->setText(ECommStateToString(commState));
            genderitem1->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_blue.png")));
        }
        else if(ECommState::ECommState_Disconnect == ECommState(commState)){

            genderitem1->setTextColor(Qt::gray);
            genderitem1->setText(ECommStateToString(commState));
            genderitem1->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));
        }
        else if(ECommState::ECommState_TimeOut == ECommState(commState)){

            genderitem1->setTextColor(Qt::red);
            genderitem1->setText(ECommStateToString(commState));
            genderitem1->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_red.png")));
        }else{
            genderitem1->setTextColor(Qt::gray);
            genderitem1->setText(ECommStateToString(commState));
            genderitem1->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));
        }

        if(EPlatformState::EPlatformState_Null == GetPlatformState(telemetryInfor._status)) {

            genderitem2->setTextColor(Qt::gray);
            genderitem2->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));
        }else if(EPlatformState::EPlatformState_Normal == GetPlatformState(telemetryInfor._status)){

            genderitem2->setTextColor(Qt::green);
            genderitem2->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_blue.png")));
        }else if (EPlatformState::EPlatformState_Err == GetPlatformState(telemetryInfor._status)) {

            genderitem2->setTextColor(Qt::red);
            genderitem2->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_red.png")));
        }else{
            genderitem2->setTextColor(Qt::gray);
            genderitem2->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));
        }

    }

    if(!find){
        const int rowCount = ui->tableWidget_UAVPlanSystemState->rowCount();
        ui->tableWidget_UAVPlanSystemState->insertRow(0);
        const int rowCountAfterInsert = ui->tableWidget_UAVPlanSystemState->rowCount();
        if ((rowCountAfterInsert == (rowCount + 1)) && m_WeaponComponentMap.find(strWeaponComponentID) == m_WeaponComponentMap.end()) {
            const int row = 0;
            QString strWeaponComponentName = (strWeaponComponentID);
            m_WeaponComponentMap.insert(strWeaponComponentID, strWeaponComponentName);
            QTableWidgetItem *genderitem  = new QTableWidgetItem(strWeaponComponentName);
            genderitem->setData(Qt::UserRole, strWeaponComponentID);

            QTableWidgetItem *genderitem1 = new QTableWidgetItem(UAVPlanSystemState::tr("Disconnect"));
            QTableWidgetItem *genderitem2 = new QTableWidgetItem(EPlatformStateToString(EPlatformState::EPlatformState_Null));
            QTableWidgetItem *genderitem3 = new QTableWidgetItem(ECtrlModeToString((ECtrlMode)4));
            QTableWidgetItem *genderitem4 = new QTableWidgetItem(EFlyPhaseToString((EFlyPhase)0));
            QTableWidgetItem *genderitem5 = new QTableWidgetItem(QString::number(0));
            QTableWidgetItem *genderitem6 = new QTableWidgetItem(QString::number(0));
            QTableWidgetItem *genderitem7 = new QTableWidgetItem(QString("--"));
            QTableWidgetItem *genderitem8 = new QTableWidgetItem(QString::number(0));
            QTableWidgetItem *genderitem9 = new QTableWidgetItem(QString::number(0));
            QTableWidgetItem *genderitem10 = new QTableWidgetItem(QString("--"));
            QTableWidgetItem *genderitem11 = new QTableWidgetItem(QString("--"));
            QTableWidgetItem *genderitem12 = new QTableWidgetItem(GetLockState((ELockState)4));
            QTableWidgetItem *genderitem13 = new QTableWidgetItem(QString("--"));
            QTableWidgetItem *genderitem14 = new QTableWidgetItem(QString("--"));

            ui->tableWidget_UAVPlanSystemState->setItem(row, 0,genderitem);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 1,genderitem1);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 2,genderitem2);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 3,genderitem3);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 4,genderitem4);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 5,genderitem5);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 6,genderitem6);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 7,genderitem7);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 8,genderitem8);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 9,genderitem9);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 10,genderitem10);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 11,genderitem11);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 12,genderitem12);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 13,genderitem13);
            ui->tableWidget_UAVPlanSystemState->setItem(row, 14,genderitem14);

            genderitem1->setTextColor(Qt::red);
            genderitem2->setTextColor(Qt::gray);
            genderitem12->setTextColor(QColor(60,86,190));

            genderitem1->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));

            genderitem2->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));

            genderitem1->setText(tr("--"));
            genderitem2->setText(tr("--"));
            genderitem3->setText(tr("--"));
            genderitem4->setText(tr("--"));
            genderitem5->setText(tr("--"));
            genderitem6->setText(tr("--"));
            genderitem7->setText(tr("--"));
            genderitem8->setText(tr("--"));
            genderitem9->setText(tr("--"));
            genderitem10->setText(tr("--"));
            genderitem11->setText(tr("--"));
            genderitem12->setText(tr("--"));
            genderitem13->setText(tr("--"));
            genderitem14->setText(tr("--"));

            genderitem->setTextAlignment(Qt::AlignCenter);
            genderitem1->setTextAlignment(Qt::AlignCenter);
            genderitem2->setTextAlignment(Qt::AlignCenter);
            genderitem3->setTextAlignment(Qt::AlignCenter);
            genderitem4->setTextAlignment(Qt::AlignCenter);
            genderitem5->setTextAlignment(Qt::AlignCenter);
            genderitem6->setTextAlignment(Qt::AlignCenter);
            genderitem7->setTextAlignment(Qt::AlignCenter);
            genderitem8->setTextAlignment(Qt::AlignCenter);
            genderitem9->setTextAlignment(Qt::AlignCenter);
            genderitem10->setTextAlignment(Qt::AlignCenter);
            genderitem11->setTextAlignment(Qt::AlignCenter);
            genderitem12->setTextAlignment(Qt::AlignCenter);
            genderitem13->setTextAlignment(Qt::AlignCenter);
            genderitem14->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void UAVPlanSystemState::updateTelemetryInformationTimeout(const QString &strWeaponComponentID)
{
    if(!m_show){
        return ;
    }

    for(int i = 0;i < ui->tableWidget_UAVPlanSystemState->rowCount();i++)
    {
        QTableWidgetItem *genderitem = ui->tableWidget_UAVPlanSystemState->item(i, 0);
        if(genderitem == nullptr){
            continue;
        }
        if((genderitem == nullptr) || (genderitem && genderitem->data(Qt::UserRole) != strWeaponComponentID)){
            continue;
        }

        QTableWidgetItem *genderitem1 =  ui->tableWidget_UAVPlanSystemState->item(i,1);
        QTableWidgetItem *genderitem2 =  ui->tableWidget_UAVPlanSystemState->item(i,2);
        QTableWidgetItem *genderitem3 =  ui->tableWidget_UAVPlanSystemState->item(i,3);
        QTableWidgetItem *genderitem4 =  ui->tableWidget_UAVPlanSystemState->item(i,4);
        QTableWidgetItem *genderitem5 =  ui->tableWidget_UAVPlanSystemState->item(i,5);
        QTableWidgetItem *genderitem6 =  ui->tableWidget_UAVPlanSystemState->item(i,6);
        QTableWidgetItem *genderitem7 =  ui->tableWidget_UAVPlanSystemState->item(i,7);
        QTableWidgetItem *genderitem8 =  ui->tableWidget_UAVPlanSystemState->item(i,8);
        QTableWidgetItem *genderitem9 =  ui->tableWidget_UAVPlanSystemState->item(i,9);
        QTableWidgetItem *genderitem10 = ui->tableWidget_UAVPlanSystemState->item(i,10);
        QTableWidgetItem *genderitem11 = ui->tableWidget_UAVPlanSystemState->item(i,11);
        QTableWidgetItem *genderitem12 = ui->tableWidget_UAVPlanSystemState->item(i,12);
        QTableWidgetItem *genderitem13 = ui->tableWidget_UAVPlanSystemState->item(i,13);
        QTableWidgetItem *genderitem14 = ui->tableWidget_UAVPlanSystemState->item(i,14);

        const ECommState commState = ECommState::ECommState_TimeOut;
        if(ECommState::ECommState_TimeOut == ECommState(commState)){

            genderitem1->setTextColor(Qt::red);
            genderitem1->setText(ECommStateToString(commState));
            genderitem1->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_red.png")));
        }else{
            genderitem1->setTextColor(Qt::gray);
            genderitem1->setText(ECommStateToString(commState));
            genderitem1->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));
        }

        genderitem2->setText(EPlatformStateToString(EPlatformState::EPlatformState_Null));
        genderitem3->setText(ECtrlModeToString((ECtrlMode)4));
        genderitem4->setText(EFlyPhaseToString((EFlyPhase)0));
        genderitem5->setText(QString::number(0));
        genderitem6->setText(QString::number(0));
        genderitem7->setText(QString("--"));
        genderitem8->setText(QString::number(0));
        genderitem9->setText(QString::number(0));
        genderitem10->setText(QString("--"));
        genderitem11->setText(QString("--"));
        genderitem12->setText(ELockStateToString((ELockState)4));
        genderitem13->setText(QString("--"));
        genderitem14->setText(QString("--"));

        genderitem1->setTextColor(Qt::red);
        genderitem2->setTextColor(Qt::gray);

        genderitem1->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));

        genderitem2->setIcon(QIcon(QPixmap(":/PlatformStateMonitoring/state_grey.png")));
    }
}

void UAVPlanSystemState::updateTelemetryInformationTree(const QString &strWeaponComponentID, const QString &strWeaponComponentcontrolMode, const telemetryInformation & telemetryInfor)
{
	QMutexLocker locker(&sMutex);

	if (!m_show)
	{
		return;
	}

	auto itr = m_UAVTreeWidgetItemMap.find(strWeaponComponentID);
	if (itr == m_UAVTreeWidgetItemMap.end())
	{
		return;
	}

	QTreeWidgetItem* pItem = itr.value();
	if (pItem == nullptr)
	{
		return;
	}

	UpdateTreeComunicateStates(pItem, 1);

	for (int i = 0;i<pItem->childCount();i++)
	{
		QTreeWidgetItem* pChild = pItem->child(i);
		if (pChild != nullptr)
		{
			UpdateTreeComunicateStates(pChild, 1);
		}
	}

	QString strTmp;
	if ((telemetryInfor._status & NebulaUAVState_RC))
	{
		strTmp = u8"开启";
		pItem->setText(3, strTmp);
	}
	else
	{
		strTmp = u8"关闭";
		pItem->setText(3, strTmp);
	}

	pItem->setText(4, strWeaponComponentcontrolMode);

	pItem->setText(5, QString(EDiffStateToString(EDiffState(telemetryInfor.differenceStatus))));

	pItem->setText(6, QString::number(telemetryInfor.gpsNumber));

	pItem->setText(7, EFlyPhaseToString(EFlyPhase(telemetryInfor.flyPeriod)));

	if ( EFlyPhase(telemetryInfor.flyPeriod) == EFlyPhase_LockBarred)
	{
		auto itrRealTime = m_UAVRealTimeEventMap.find(strWeaponComponentID);
		if (itrRealTime != m_UAVRealTimeEventMap.end())
		{
			itrRealTime->second = (int)FireProEvent_Enum_SYSTEM_Lock;
		}
	}
	else if (EFlyPhase(telemetryInfor.flyPeriod) == EFlyPhase_Ready)
	{
		if (strWeaponComponentcontrolMode == "RTL")
		{
			auto itrRealTime = m_UAVRealTimeEventMap.find(strWeaponComponentID);
			if (itrRealTime != m_UAVRealTimeEventMap.end())
			{
				itrRealTime->second = (int)FireProEvent_Enum_SYSTEM_RTL;
			}
		}
		else if (strWeaponComponentcontrolMode == "Land")
		{
			auto itrRealTime = m_UAVRealTimeEventMap.find(strWeaponComponentID);
			if (itrRealTime != m_UAVRealTimeEventMap.end())
			{
				itrRealTime->second = (int)FireProEvent_Enum_SYSTEM_Land;
			}
		}
		else
		{
			auto itrRealTime = m_UAVRealTimeEventMap.find(strWeaponComponentID);
			if (itrRealTime != m_UAVRealTimeEventMap.end())
			{
				itrRealTime->second = (int)FireProEvent_Enum_SYSTEM_Unlock;
			}
		}
	}

	pItem->setText(8, QString("P:%1, %2").arg(telemetryInfor.powervoltage, 0, 'f', 2).arg(telemetryInfor.fkvoltage, 0, 'f', 2));

	pItem->setText(9, QString::number(telemetryInfor._current / 10.0, 'f', 1));

	pItem->setText(10, QString("%1 %2").arg(telemetryInfor.position.lng, 0, 'f', 7).arg(telemetryInfor.position.lat, 0, 'f', 7));

	pItem->setText(11, QString("R:%1, %2").arg(telemetryInfor.relativeOriginHeigth, 0, 'f', 2).arg(telemetryInfor.gpsHeigth * 10.0, 0, 'f', 2));

    pItem->setText(12, QString::number(telemetryInfor.groundVelocity, 'f', 2));

	pItem->setText(13, QString("P:%1, R:%2, Y:%3").arg(telemetryInfor.angleofPitch, 0, 'f', 2).arg(telemetryInfor.angleofRoll, 0, 'f', 2).arg(telemetryInfor.angleofRudder, 0, 'f', 2));

	UpdateTreeEventStates(strWeaponComponentID, pItem);
}

void UAVPlanSystemState::updateTelemetryInformationTimeoutTree(const QString &strWeaponComponentID)
{
	if (!m_show)
	{
		return;
	}

	auto itr = m_UAVTreeWidgetItemMap.find(strWeaponComponentID);
	if (itr == m_UAVTreeWidgetItemMap.end())
	{
		return;
	}

	QTreeWidgetItem* pItem = itr.value();
	if (pItem == nullptr)
	{
		return;
	}

	UpdateTreeComunicateStates(pItem, 2);

	for (int i = 0; i < pItem->childCount(); i++)
	{
		QTreeWidgetItem* pChild = pItem->child(i);
		if (pChild != nullptr)
		{
			UpdateTreeComunicateStates(pChild, 2);
		}
	}

	for (int i = 3;i<m_TreeHeader.size()-1;i++)
	{
		pItem->setText(i, tr("-"));
	}
}

void UAVPlanSystemState::updateTelemetryInformationWarningTips(const QString &strWeaponComponentID, const qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t &telemetryInfo)
{

    const auto& telemetryInfoIter = m_WeaponComponenTelemetryInformationMap.find(strWeaponComponentID);

    if(telemetryInfoIter != m_WeaponComponenTelemetryInformationMap.end()){
        auto& telemetryInfoWaringTemp = telemetryInfoIter.value();
        QStringList textWarningTips;
        textWarningTips.clear();

        if(telemetryInfoWaringTemp.information.isout != telemetryInfo.isout){
            if(telemetryInfo.isout){
                textWarningTips.append(tr(" connect Out time, Connect lost!"));
            }else{
                textWarningTips.append(tr(" reconnect"));
            }
        }

        if(telemetryInfo.telemetryInfo.powervoltage < telemetryInfoWaringTemp.powervoltageWarningMin){
            textWarningTips.append(QString(tr(" power voltage less %1!")).arg(telemetryInfoWaringTemp.powervoltageWarningMin));
        }

        if(telemetryInfo.telemetryInfo.gpsNumber < telemetryInfoWaringTemp.gpsNumberWarningMin){
            textWarningTips.append(QString(tr(" gps number less %1!")).arg(telemetryInfoWaringTemp.gpsNumberWarningMin));
        }

        if(!(telemetryInfo.telemetryInfo._status & NebulaUAVState_SatelliteDirection)){
            textWarningTips.append(tr(" satellite direction abnormal!"));
        }

        if(!(telemetryInfo.telemetryInfo._status & NebulaUAVState_GPS)){
            textWarningTips.append(tr(" GPS abnormal!"));
        }

        if(telemetryInfoWaringTemp.information.telemetryInfo.differenceStatus != telemetryInfo.telemetryInfo.differenceStatus){
            if(telemetryInfo.telemetryInfo.differenceStatus != EDiffState::EDiffState_GPS &&
                    telemetryInfo.telemetryInfo.differenceStatus != EDiffState::EDiffState_RTKFixedSolution &&
                    telemetryInfo.telemetryInfo.differenceStatus != EDiffState::EDiffState_RTKFFloatingSolution ){
                textWarningTips.append(tr(" GPS not RTK: ") + EDiffStateToString(EDiffState(telemetryInfo.telemetryInfo.differenceStatus)));
            }
        }

        if(telemetryInfo.telemetryInfo.angleofPitch  >= telemetryInfoWaringTemp.angleofPitchWarningMin){
            textWarningTips.append(QString(tr(" Pitch more %1!")).arg(telemetryInfoWaringTemp.angleofPitchWarningMin));
        }
        if(telemetryInfo.telemetryInfo.angleofRoll  >= telemetryInfoWaringTemp.angleofRollWarningMin){
            textWarningTips.append(QString(tr(" Roll more %1!")).arg(telemetryInfoWaringTemp.angleofRollWarningMin));
        }

        if(textWarningTips.size() > 0){
            const QString& weaponComponentName = m_WeaponComponentMap[strWeaponComponentID];
            qnzkna::TextToSpeechManage::ITextToSpeechManageService* pITextToSpeechManageService = IPlatformStateMonitoringActivator::Get().getPITextToSpeechManageService();
            if(pITextToSpeechManageService){
                for(const QString& textToSpeech : textWarningTips)
                pITextToSpeechManageService->textToSpeech(weaponComponentName + textToSpeech);
            }
        }

        telemetryInfoWaringTemp.information = telemetryInfo;
    }else{
        Information_Warning_t information_Warning;
        information_Warning.information            = telemetryInfo;
        information_Warning.powervoltageWarningMin = 43.5;
        information_Warning.gpsNumberWarningMin    = 12;
        information_Warning.angleofPitchWarningMin = 25;
        information_Warning.angleofRollWarningMin  = 25;

        m_WeaponComponenTelemetryInformationMap[strWeaponComponentID] = information_Warning;
    }
	m_mapLocatonCoverUAV[strWeaponComponentID] = QGeoCoordinate(telemetryInfo.telemetryInfo.position.lat, telemetryInfo.telemetryInfo.position.lng, telemetryInfo.telemetryInfo.gpsHeigth);

	int RouteID;
	bool isSuccess = MatchToAccessRouteID_UAVName(strWeaponComponentID, RouteID);
	if (m_pIZoneManageService) {
		QStringList ZoneListID;
		if (!m_pIZoneManageService->GetZoneListByAirLineID(RouteID, ZoneListID))
			return;
		bool bIsExit = false;
		for (auto zone : ZoneListID)
		{
			qnzkna::ZoneManage::CZoneBase stZoneData;
			if(!m_pIZoneManageService->GetZoneBase(zone, stZoneData))
				continue;
			if (CheckRoutePointOfZoneInside(stZoneData, QGeoCoordinate(telemetryInfo.telemetryInfo.position.lat, telemetryInfo.telemetryInfo.position.lng, telemetryInfo.telemetryInfo.gpsHeigth)))
			{
				bIsExit = true;
				break;
			}
		}
		if (!bIsExit)
		{
			if (!n_DeviationVoyageCount || !(n_DeviationVoyageCount % 5))
			{
				char pBuf[MAX_BUFFER_LEN] = { 0 };
				int nBufLen = 0;
				std::string strUAVName = m_SystemSysNameAndID[tr3(strWeaponComponentID)];
				qnzkna::framework::MessageReferenceProps  props;
				props.put("UAVName", strUAVName);
				IPlatformStateMonitoringActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("DeviationVoyageRoute", pBuf, nBufLen, props));
			}
			++n_DeviationVoyageCount;
		}
	}
}

bool UAVPlanSystemState::CheckRoutePointOfZoneInside(qnzkna::ZoneManage::CZoneBase zone, QGeoCoordinate position)
{
	bool InsitdeZone = false;
	if (nullptr == m_proj)
		return InsitdeZone;

	QPolygonF ZonePolygon;
	switch (zone.nZoneType)
	{
	case 0:
	{
		QPointF screenPos = m_proj->Jwd2Screen(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat);
		QRectF rect(screenPos.x(), screenPos.y(), zone.stRoundParameter.nRadius, zone.stRoundParameter.nRadius);
		for (int i = 0; i < 90; i++)
		{
			double angle = i * (360.0 / 90);
			QGeoCoordinate point = QGeoCoordinate(screenPos.y(),screenPos.x()).atDistanceAndAzimuth(zone.stRoundParameter.nRadius, angle);
			ZonePolygon << QPointF(point.longitude(), point.latitude());
		}
		QPointF positionF = QPointF(position.longitude(), position.latitude());
		if (ZonePolygon.containsPoint(positionF, Qt::OddEvenFill))
		{
			if (zone.nAboveAlt >= position.altitude() && zone.nBottomhAlt <= position.altitude())
				InsitdeZone = true;
		}
	}
	break;
	case 1:
	{

	}
	break;
	case 2:
	{

	}
	break;
	case 3:
	{
		for (int i = 0; i < zone.stZonePointVec.size(); i++)
		{
			QPointF screenPos = m_proj->Jwd2Screen(zone.stZonePointVec[i].dLon, zone.stZonePointVec[i].dLat);
			ZonePolygon << QPointF(zone.stZonePointVec[i].dLon, zone.stZonePointVec[i].dLat);
		}
		QPointF positionF = QPointF(position.longitude(), position.latitude());
		if (ZonePolygon.containsPoint(positionF, Qt::OddEvenFill))
		{
			if (zone.nAboveAlt >= position.altitude() && zone.nBottomhAlt <= position.altitude())
				InsitdeZone = true;
		}
	}
	break;
	case 4:
	{
		qnzkna::ZoneManage::ZonePointVec rectPoint;
		bool bZoneTransformRectang = m_pIZoneManageService->TransformRectangleToPoints(tr2(zone.strID), rectPoint);
		if(bZoneTransformRectang)
		{
			for (auto rect : rectPoint)
			{
				QPointF screenPos = m_proj->Jwd2Screen(rect.dLon, rect.dLat);
				ZonePolygon << QPointF(rect.dLon,rect.dLat);
			}
		}
		QPointF positionF = QPointF(position.longitude(), position.latitude());
		if (ZonePolygon.containsPoint(positionF, Qt::OddEvenFill))
		{
			if (zone.nAboveAlt >= position.altitude() && zone.nBottomhAlt <= position.altitude())
				InsitdeZone = true;
		}
	}
	break;
	case 8:
	{
		auto calculateRectangleArea = [&](double lat1, double lon1, double lat2, double lon2, double width)-> QPolygonF {
			QGeoCoordinate A(lat1, lon1);
			QGeoCoordinate B(lat2, lon2);
			int azim = A.azimuthTo(B);
			QGeoCoordinate rect1 = A.atDistanceAndAzimuth(zone.nLineWidth, azim + 90);
			QGeoCoordinate rect2 = A.atDistanceAndAzimuth(zone.nLineWidth, azim + 90 + 180);
			QGeoCoordinate rect3 = B.atDistanceAndAzimuth(zone.nLineWidth, azim - 90);
			QGeoCoordinate rect4 = B.atDistanceAndAzimuth(zone.nLineWidth, azim + 90);
			return QPolygonF() << QPointF(rect1.longitude(), rect1.latitude()) << QPointF(rect2.longitude(), rect2.latitude()) <<
				QPointF(rect3.longitude(), rect3.latitude()) << QPointF(rect4.longitude(), rect4.latitude());
		};

		for (int i = 0; i < zone.stZonePointVec.size(); i++)
		{
			if(i + 1 >= zone.stZonePointVec.size())
				break;
			QPolygonF zoneTextValue;
			QPointF A = QPointF(zone.stZonePointVec[i].dLon, zone.stZonePointVec[i].dLat);
			QPointF B = QPointF(zone.stZonePointVec[i + 1].dLon, zone.stZonePointVec[i + 1].dLat);
			zoneTextValue << calculateRectangleArea(A.y(), A.x(), B.y(), B.x(), zone.nLineWidth);
			QPointF positionF = QPointF(position.longitude(), position.latitude());
			if (zoneTextValue.containsPoint(positionF, Qt::OddEvenFill))
			{
				if (zone.nAboveAlt >= position.altitude() && zone.nBottomhAlt <= position.altitude())
				{
					InsitdeZone = true;
					break;
				}
			}
		}
	}
	break;
	default:
		break;
	}

	return InsitdeZone;
}
std::string UAVPlanSystemState::LineMatchPlatform(const std::string &strMissionID, const int &nLineID, const int &nNo)
{
	std::string strPlatID = "";
	std::string missionid = QString::fromLocal8Bit(strMissionID.c_str()).toStdString();
	CNeubulaMissionLineVec LineVec = m_pITaskManageService->GetMissionLinesByTask(missionid);

	if (LineVec.size() == 0)
		return strPlatID;

	std::vector<std::string> reStrs = m_pITaskManageService->readPlatFormMatch(strMissionID);

	if (reStrs.size() == 0)
		return strPlatID;

	auto itrPlat = reStrs.begin();
	auto itrPlat2 = reStrs.begin();
	for (auto line : LineVec)
	{
		if (line.nID != nLineID)
			continue;

		WndRouteLineInfo *pPoint = new WndRouteLineInfo;
		pPoint->bFormState = false;

		if (nNo < reStrs.size())
			itrPlat2 = itrPlat + nNo;
		else
			continue;

		return *itrPlat2;
	}
	return strPlatID;
}
bool UAVPlanSystemState::MatchToAccessRouteID_UAVName(const QString& strWeaponComponentID,int& nAirLineID)
{
	if (nullptr == m_pITaskManageService)
		return false;

	nAirLineID = -1;

	CNeubulaMissionVec m_Missions = m_pITaskManageService->GetActivatingMissions();
	CNeubulaMissionLineVec m_MissionLineShowVec;
	for (int i = 0; i < m_Missions.size(); i++)
	{
		std::string missionId = m_Missions[i].GetID();
		auto newLines = m_pITaskManageService->GetMissionLinesByTask(missionId);
		for (int j = 0; j < newLines.size(); j++)
		{
			m_MissionLineShowVec.push_back(newLines[j]);
		}
	}
	if (m_MissionLineShowVec.empty())
		return false;

	for (auto data : m_MissionLineShowVec)
	{
		QString strTask;
		strTask = QString::fromLocal8Bit(data.strMissionID);

		QString strMissionIDTemp = "";
		int nNo(0), intMissionLineNo(0);
		if (strMissionIDTemp == strTask)
		{
			intMissionLineNo++;
		}
		else
		{
			strMissionIDTemp = strTask;
			intMissionLineNo = 0;
		}
		std::string stdstrPlatform = LineMatchPlatform(data.strMissionID, data.nID, intMissionLineNo);
		string str = tr3(strWeaponComponentID);
		if (strWeaponComponentID == stdstrPlatform.c_str())
		{
			nAirLineID = data.nID;
			return true;
		}
	}
	return false;
}

void UAVPlanSystemState::localWeaponNameBindToId()
{
	m_SystemSysNameAndID.clear();
	qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
	m_pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap);
	for (auto sysItr : weaponSystemMap)
	{
		if (sysItr.second.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_UAV)
		{
			auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
			for (auto weapon : *weaponComPonentMap)
			{
				m_SystemSysNameAndID.insert(std::make_pair(weapon.second.getComponentID(), weapon.second.getComponentName()));
			}
		}
	}
}

void UAVPlanSystemState::on_tableWidget_UAVPlanSystemState_itemDoubleClicked(QTableWidgetItem *item)
{
    if(item == nullptr){
        return ;
    }

    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService* pIUAVRealTimeStateManageService = IPlatformStateMonitoringActivator::Get().getPIUAVRealTimeStateManageService();
    if(pIUAVRealTimeStateManageService == nullptr){
        return ;
    }

    const int row    = item->row();
    const QTableWidgetItem * const genderitem =  ui->tableWidget_UAVPlanSystemState->item( row, 0);
    if((genderitem == nullptr) || (genderitem && genderitem->data(Qt::UserRole).isValid())){
        pIUAVRealTimeStateManageService->SetCurFocusUAV(genderitem->data(Qt::UserRole).toString().toStdString());
        if(item->column() == 0){
            const QTableWidgetItem * const genderitem7 =  ui->tableWidget_UAVPlanSystemState->item(row, 7);
            if(!genderitem7){
                return ;
            }

            const QStringList& strlngLat = genderitem7->text().split(' ');
            if(strlngLat.size() != 3){
                return ;
            }

            bool ok = false;
            const double lng = strlngLat.at(1).toDouble(&ok);
            if(!ok){
                return ;
            }

            ok = false;
            const double lat = strlngLat.at(2).toDouble(&ok);
            if(!ok){
                return ;
            }

            //LY_IPlatformStateMonitoring::Get().SetScreenCenter(lng, lat);
        }
    }
}

void UAVPlanSystemState::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item == nullptr)
	{
		return;
	}

	if (column == -1)
	{
		return;
	}

	QString strID = item->data(0, Qt::UserRole).toString();
	if (strID == "")
	{
		return;
	}

	qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService* pIUAVRealTimeStateManageService = IPlatformStateMonitoringActivator::Get().getPIUAVRealTimeStateManageService();
	if (pIUAVRealTimeStateManageService == nullptr) {
		return;
	}

	auto itr = m_UAVTreeWidgetItemMap.find(strID);
	if (itr == m_UAVTreeWidgetItemMap.end())
	{
		return;
	}

	QTreeWidgetItem* pItem = itr.value();
	if (pItem == nullptr)
	{
		return;
	}

	pItem->text(10);

	pIUAVRealTimeStateManageService->SetCurFocusUAV(strID.toStdString());

	const QStringList& strlngLat = pItem->text(10).split(' ');
	if (strlngLat.size() != 2) {
		return;
	}

	bool ok = false;
	const double lng = strlngLat.at(0).toDouble(&ok);
	if (!ok) {
		return;
	}

	ok = false;
	const double lat = strlngLat.at(1).toDouble(&ok);
	if (!ok) {
		return;
	}

	//LY_IPlatformStateMonitoring::Get().SetScreenCenter(lng, lat);
}
