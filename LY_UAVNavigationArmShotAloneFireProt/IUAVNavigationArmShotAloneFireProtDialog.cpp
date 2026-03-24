#include "IUAVNavigationArmShotAloneFireProtDialog.h"

#include "ui_IUAVNavigationArmShotAloneFireProtDialog.h"

#include "IUAVNavigationArmShotAloneFireProtActivator.h"

#include "ImageStatusWidget.h"

#ifdef VIDEOHUDDIALOG_SHOW
#include "VideoHUDDialog.h"
#endif
#define PLAY_VIDEO_STREAM_MANAGE_DEFINES
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES

#include "PlayVideoManage/PlayVideoManageWidget.h"

#ifdef PLAY_YOLO_VIDEO_STREAM_MANAGE_DEFINES

#include "PlayVideoManage/PlayYoloVideoStreamManageWidget.h"

#endif

#ifdef PLAY_VIDEO_STREAM_MANAGE_PLAY_VIDEO_MANAGE_FIREPROT_DEFINES
#include "PlayVideoManage/PlayVideoManageWidget54.h"
#endif

#endif

#include "lyUI/LyMessageBox.h"

#include <QCString.h>
#include "QtSql/QSqlQuery"
#include "QDateTime"

#define KEY_COMPONT_NAME "KEY_COMPONT_NAME"

#define KEY_VIDEO_DEVICE_TYPE     "KEY_VIDEO_DEVICE_TYPE"
#define KEY_VIDEO_DEVICE_URL      "KEY_VIDEO_DEVICE_URL"
#define KEY_VIDEO_DEVICE_PORT     "KEY_VIDEO_DEVICE_PORT"
#define KEY_VIDEO_DEVICE_CHANNEL  "KEY_VIDEO_DEVICE_CHANNEL"
#define KEY_VIDEO_DEVICE_USERNAME "KEY_VIDEO_DEVICE_USERNAME"
#define KEY_VIDEO_DEVICE_PASSWORD "KEY_VIDEO_DEVICE_PASSWORD"

#define KEY_DEVICE_ID    "KEY_DEVICE_ID"
#define KEY_BOMB_HANG_ID "KEY_BOMB_HANG_ID"

#define KEY_DEVICE_A_ID    "KEY_DEVICE_A_ID"
#define KEY_BOMB_HANG_A_ID "KEY_BOMB_HANG_A_ID"

#define KEY_DEVICE_B_ID    "KEY_DEVICE_B_ID"
#define KEY_BOMB_HANG_B_ID "KEY_BOMB_HANG_B_ID"

#define ID_EVENT_FIRE_CONTROL       WM_USER + 300
#define TIMER_FIRE_CONTROL_REFRESH  (1000 * 12)

#define __DEBUG__LOGINFO__

#if defined(__DEBUG__LOGINFO__)
#include <QDebug>
#define logInfoFormat(format, ...)  qDebug(format, ##__VA_ARGS__)
#define logInfo                     qDebug
#else
#define logInfoFormat(format, ...)
#define logInfo() /##/
#endif

#define USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE

#include "LY_UAVNavigationArmShotAloneFireProtInterface.h"
#include "DataManage/SystemInfo/SystemInternalInterface.h"

void SystemFireProtControl_SendMsgToAll(const unsigned long long int    vehicle_id,
                                        const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType,
                                        const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType
                                        )
{
    if(vehicle_id == 0){
        return ;
    }

    if(fireProtType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Nona){
        return ;
    }

    if(fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Nona){
        return ;
    }

    qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl systemFireProtControl;
    systemFireProtControl.head.id             = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SYSTEMINTERNALINTERFACEHEAD_ID_SYSTEMFIREPROTCONTROL;
    systemFireProtControl.head.time           = QDateTime::currentMSecsSinceEpoch();
    systemFireProtControl.vehicle_id          = vehicle_id;
    systemFireProtControl.fireProtType        = fireProtType;
    systemFireProtControl.fireProtOperType    = fireProtOperType;

    void *lParam = (void *)(&systemFireProtControl);
    if(lParam == nullptr){
        return ;
    }

    LY_UAVNavigationArmShotAloneFireProtInterface::Get().SendMsgToAll(IInterface::MSG_Command, QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_CONTROL_ID, lParam);
}

#else

void SendMsgToAll(const char * const message, const unsigned int length)
{
    if(message == nullptr){
        return ;
    }

    if(length != sizeof(qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl)){
        return ;
    }

    void *lParam = (void *)(message);
    if(lParam == nullptr){
        return ;
    }
}
#endif

const char* IUAVNavigationArmShotAloneFireProtDialog::QPushButton_Property_ComponentName = "QPushButton_Property_ComponentNameDYFireProt";
const char* IUAVNavigationArmShotAloneFireProtDialog::QPushButton_Property_ComponentID   = "QPushButton_Property_ComponentIDDYFireProt";

IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog(const QString &currentComponentID, const QString &currentComponentName, QWidget* parent)
    : QWidget(parent)
      , ui(new Ui::IUAVNavigationArmShotAloneFireProtDialog)
      , m_WindowTitle("")
      , m_VerticalBoxLayout(nullptr)
      , m_IPlayVideoManage(nullptr)

      , m_CurrentTime(0)
      , m_Timer()
      , m_Pill_CheckBox_All_Shot_Checked(false)
      , m_singleSelectedBomb(true)
      , m_CurrentSensorID("")
      , m_TimeOutBomb(0)
      , m_TimeOut_DY_ZC_HUD_ControlInfo(0)
      , m_currentComponentID(currentComponentID)
      , m_currentComponentName(currentComponentName)
      , m_pButtonCurrentComponent(nullptr)
      , m_pButtonGroupComponent(nullptr)
      , m_videoDeviceType("")
      , m_videoDeviceURL("")
      , m_videoDeviceURLInfrared("")
      , m_videoDevicePort("")
      , m_videoDeviceChannel("")
      , m_videoDeviceUserName("")
      , m_videoDevicePassWord("")
      , m_DYFireProt_BombType(DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Detect)
#ifdef VIDEOHUDDIALOG_SHOW
      ,m_VideoHUDDialog(nullptr)
#endif
{
    m_DYFireProt_ImageStatusList_Pill_1.clear();
    m_DYFireProt_ImageStatusList_Pill_2.clear();

    m_pButtonGroupComponent = new QButtonGroup(this);
    m_pButtonGroupComponent->setObjectName(QString::fromUtf8("Button_DYFireProt_GroupComponent"));

    ui->setupUi(this);

    ui->label_IUAVNavigationArmShotAloneFireProtDialog_DY_ZC_HUD_ControlInfo->setEnabled(false);
    ui->label_IUAVNavigationArmShotAloneFireProtDialog_DY_ZC_HUD_ControlInfo->setVisible(false);
    ui->label_IUAVNavigationArmShotAloneFireProtDialog_DY_ZC_HUD_ControlInfo->hide();

    ui->widget_DYFireProt_BombType_Rocket->setVisible(true);
    ui->widget_DYFireProt_BombType_Rocket->hide();

    ui->groupBox_DYFireProt_BombType_A->setVisible(false);
    ui->groupBox_DYFireProt_BombType_A->hide();
    ui->groupBox_DYFireProt_BombType_B->setVisible(false);
    ui->groupBox_DYFireProt_BombType_B->hide();

    ui->widget_DYFireProt_BombType_A_1->setVisible(false);
    ui->widget_DYFireProt_BombType_A_1->hide();
    ui->widget_DYFireProt_BombType_B_1->setVisible(false);
    ui->widget_DYFireProt_BombType_B_1->hide();

    ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(false);
    ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(false);
    ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

    ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(false);
    ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(false);
    ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);

    ui->groupBox_DYFireProt_BombType_A->setAlignment(Qt::AlignCenter);
    ui->groupBox_DYFireProt_BombType_B->setAlignment(Qt::AlignCenter);

    ui->groupBox_DYFireProt_BombType_A->setStyleSheet(QString("QGroupBox::title { subcontrol-origin: margin; margin-left: 5; margin-right: 5; top: -3; color: #ffb848; }"));
    ui->groupBox_DYFireProt_BombType_B->setStyleSheet(QString("QGroupBox::title { subcontrol-origin: margin; margin-left: 5; margin-right: 5; top: -3; color: #ffb848; }"));

    {
        m_VerticalBoxLayout = new UAVNavigationArmShotAloneFireProtBoxLayout(this, this);
        m_VerticalBoxLayout->setSpacing(2);
        m_VerticalBoxLayout->setObjectName(QString::fromUtf8("UAVNavigationArmShotAloneFireProtBoxLayout"));
        m_VerticalBoxLayout->setContentsMargins(2, 2, 2, 2);
        m_VerticalBoxLayout->setSpacing(2);

        m_VerticalBoxLayout->addWidget(ui->widget_DYFireProt_Top_Video);

        ui->groupBox_DYFireProt_Middle->setFixedHeight(210);
        ui->groupBox_DYFireProt_Middle->setFixedHeight(150);
        ui->groupBox_DYFireProt_Middle->setFixedHeight(100);
        m_VerticalBoxLayout->addWidget(ui->groupBox_DYFireProt_Middle);

    }

    m_DYFireProt_ImageStatusList_Pill_1.push_back(ui->ImageStatusWidget_DYFireProt_Pill_1_Command_Insure_Status);
    m_DYFireProt_ImageStatusList_Pill_1.push_back(ui->ImageStatusWidget_DYFireProt_Pill_1_Command_Percussion_Status);
    m_DYFireProt_ImageStatusList_Pill_1.push_back(ui->ImageStatusWidget_DYFireProt_Pill_1_Command_Reset_Status);
    m_DYFireProt_ImageStatusList_Pill_2.push_back(ui->ImageStatusWidget_DYFireProt_Pill_2_Command_Insure_Status);
    m_DYFireProt_ImageStatusList_Pill_2.push_back(ui->ImageStatusWidget_DYFireProt_Pill_2_Command_Percussion_Status);
    m_DYFireProt_ImageStatusList_Pill_2.push_back(ui->ImageStatusWidget_DYFireProt_Pill_2_Command_Reset_Status);

    for(auto ImageStatus : m_DYFireProt_ImageStatusList_Pill_1){
        ImageStatus->setIUAVNavigationArmShotAloneFireProtDialog(this);
        ImageStatus->setImageStatusPixmap(QPixmap(":/resources/gallery/24x24@1.3/02.png"),
                                          QPixmap(":/resources/gallery/24x24@1.3/01.png"),
                                          QPixmap(":/resources/gallery/24x24@1.3/03.png"));
        ImageStatus->setBombStatusMiddlePixmap(QPixmap(":/resources/gallery/24x24/02.png"),
                                               QPixmap(":/resources/gallery/24x24/01.png"),
                                               QPixmap(":/resources/gallery/24x24/03.png"));

        ImageStatus->setVisible(false);
        ImageStatus->hide();
    }

    for(auto ImageStatus : m_DYFireProt_ImageStatusList_Pill_2){
        ImageStatus->setIUAVNavigationArmShotAloneFireProtDialog(this);
        ImageStatus->setImageStatusPixmap(QPixmap(":/resources/gallery/24x24@1.3/02.png"),
                                          QPixmap(":/resources/gallery/24x24@1.3/01.png"),
                                          QPixmap(":/resources/gallery/24x24@1.3/03.png"));
        ImageStatus->setBombStatusMiddlePixmap(QPixmap(":/resources/gallery/24x24/02.png"),
                                               QPixmap(":/resources/gallery/24x24/01.png"),
                                               QPixmap(":/resources/gallery/24x24/03.png"));

        ImageStatus->setVisible(false);
        ImageStatus->hide();
    }

    {

        {

            ui->widget_DYFireProt_BombType_A_1->initBombData(BombStatusWidget::BombData(QT_TRANSLATE_NOOP("IUAVNavigationArmShotAloneFireProtDialog","The rockA"),
                                                                                        BombStatusWidget::BombHangType::BombHangType_A,
                                                                                        QT_TRANSLATE_NOOP("IUAVNavigationArmShotAloneFireProtDialog","The RocketsA2"),
                                                                                        BombStatusWidget::BombType::BombType_A2,
                                                                                        0x0001),
                                                             this,
                                                             bomBCallBackFunction);
            ui->widget_DYFireProt_BombType_A_1->setBombStatusPixmap(QPixmap(":/resources/gallery/24x24@2/e1.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e2.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e3.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e4.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e5.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e6.png"));
            ui->widget_DYFireProt_BombType_A_1->setBombStatusMiddlePixmap(QPixmap(":/resources/gallery/24x24@1.3/e1.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e2.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e3.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e4.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e5.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e6.png"));
        }

    }

    {

        {

            ui->widget_DYFireProt_BombType_B_1->initBombData(BombStatusWidget::BombData( QT_TRANSLATE_NOOP("IUAVNavigationArmShotAloneFireProtDialog","The rockA"),
                                                                                        BombStatusWidget::BombHangType::BombHangType_B,
                                                                                        QT_TRANSLATE_NOOP("IUAVNavigationArmShotAloneFireProtDialog","The rockA"),
                                                                                        BombStatusWidget::BombType::BombType_B2,
                                                                                        0x0001),
                                                             this,
                                                             bomBCallBackFunction);
            ui->widget_DYFireProt_BombType_B_1->setBombStatusPixmap(QPixmap(":/resources/gallery/24x24@2/e1.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e2.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e3.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e4.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e5.png"),
                                                                    QPixmap(":/resources/gallery/24x24@2/e6.png"));
            ui->widget_DYFireProt_BombType_B_1->setBombStatusMiddlePixmap(QPixmap(":/resources/gallery/24x24@1.3/e1.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e2.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e3.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e4.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e5.png"),
                                                                          QPixmap(":/resources/gallery/24x24@1.3/e6.png"));
        }

    }

    InitBombState(true);

    {
        QPushButton* weaponComponentButton = new QPushButton(m_currentComponentName, this);
        if(weaponComponentButton != nullptr){
            weaponComponentButton->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed));
            weaponComponentButton->setMinimumSize(QSize(2, 2));
            weaponComponentButton->setCheckable(true);
            weaponComponentButton->setChecked(true);

            weaponComponentButton->setProperty(QPushButton_Property_ComponentName, m_currentComponentName);
            weaponComponentButton->setProperty(QPushButton_Property_ComponentID,   m_currentComponentID);
            this->ui->horizontalLayout_Vehicle->insertWidget(this->ui->horizontalLayout_Vehicle->count() - 1, weaponComponentButton);

            if(m_pButtonGroupComponent != nullptr){
                m_pButtonGroupComponent->addButton(weaponComponentButton);
            }
        }
    }

    connect(this, &IUAVNavigationArmShotAloneFireProtDialog::ShowXygsDYFireProtKZMsgStatusInfoSig, this, &IUAVNavigationArmShotAloneFireProtDialog::ShowXygsDYFireProtKZMsgStatusInfo, Qt::QueuedConnection);
    connect(this, &IUAVNavigationArmShotAloneFireProtDialog::ShowDY_ZC_HUD_ControlInfoSig,   this, &IUAVNavigationArmShotAloneFireProtDialog::ShowDY_ZC_HUD_ControlInfo,   Qt::QueuedConnection);

}

IUAVNavigationArmShotAloneFireProtDialog::~IUAVNavigationArmShotAloneFireProtDialog()
{
    if(ui != nullptr && ui->widget_DYFireProt_Top_Video != nullptr && m_IPlayVideoManage != nullptr)
    {
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
        m_IPlayVideoManage->setStop(true);
        m_IPlayVideoManage->StopVideoManageThread();
#endif
    }

#ifdef VIDEOHUDDIALOG_SHOW
    if(m_VideoHUDDialog){
        delete m_VideoHUDDialog;
    }
#endif

    delete ui;
}

void IUAVNavigationArmShotAloneFireProtDialog::StopIUAVNavigationArmShotAloneFireProtVideoManage()
{
    if(ui != nullptr && ui->widget_DYFireProt_Top_Video != nullptr && m_IPlayVideoManage != nullptr)
    {
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
        m_IPlayVideoManage->setStop(true);

#endif
    }
}

void IUAVNavigationArmShotAloneFireProtDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

bool IUAVNavigationArmShotAloneFireProtDialog::event(QEvent *event)
{
    if(event->type() == QEvent::Type::ScreenChangeInternal)
    {
        moveVideoHUDDialog();
    }
    return QWidget::event(event);
}

void IUAVNavigationArmShotAloneFireProtDialog::moveEvent(QMoveEvent *event)
{
    moveVideoHUDDialog();

    QWidget::moveEvent(event);
}

void IUAVNavigationArmShotAloneFireProtDialog::resizeEvent(QResizeEvent *event)
{
    resizeVideoHUDDialog();

    QWidget::resizeEvent(event);
}

void IUAVNavigationArmShotAloneFireProtDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    moveVideoHUDDialog();

#ifdef VIDEOHUDDIALOG_SHOW
    if(m_VideoHUDDialog != nullptr){

        m_VideoHUDDialog->show();
    }
#endif
}

void IUAVNavigationArmShotAloneFireProtDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

#ifdef VIDEOHUDDIALOG_SHOW
    if(m_VideoHUDDialog != nullptr){
        m_VideoHUDDialog->hide();
    }
#endif
}

void IUAVNavigationArmShotAloneFireProtDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    switch (event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void IUAVNavigationArmShotAloneFireProtDialog::moveVideoHUDDialog()
{
#ifdef VIDEOHUDDIALOG_SHOW
    if(m_VideoHUDDialog != nullptr){
        if(ui && ui->widget_DYFireProt_Top_Video && m_IPlayVideoManage){

            m_VideoHUDDialog->move(ui->widget_DYFireProt_Top_Video->mapToGlobal(m_IPlayVideoManage->VideoWidgetPos()));
            m_VideoHUDDialog->resize(m_IPlayVideoManage->VideoWidgetSize());
        }else if(ui && ui->widget_DYFireProt_Top_Video){
            m_VideoHUDDialog->move(this->mapToGlobal(ui->widget_DYFireProt_Top_Video->pos()));
            m_VideoHUDDialog->resize(ui->widget_DYFireProt_Top_Video->size());
        }else{
            m_VideoHUDDialog->move(this->mapToGlobal(this->pos()));
            m_VideoHUDDialog->resize(size());
        }
    }
#endif
}

void IUAVNavigationArmShotAloneFireProtDialog::resizeVideoHUDDialog()
{
#ifdef VIDEOHUDDIALOG_SHOW
    if(m_VideoHUDDialog != nullptr){
        if(ui && ui->widget_DYFireProt_Top_Video && m_IPlayVideoManage){

            m_VideoHUDDialog->resize(m_IPlayVideoManage->VideoWidgetSize());
        }else if(ui && ui->widget_DYFireProt_Top_Video){
            m_VideoHUDDialog->resize(ui->widget_DYFireProt_Top_Video->size());
        }else{
            m_VideoHUDDialog->resize(size());
        }
    }
#endif
}

qnzkna::PlayVideoManage::IPlayVideoManage *IUAVNavigationArmShotAloneFireProtDialog::getPlayVideoManageWidget() const
{
    if(ui && ui->widget_DYFireProt_Top_Video && m_IPlayVideoManage){
        return m_IPlayVideoManage;
    }
    return nullptr;
}

std::string IUAVNavigationArmShotAloneFireProtDialog::getCurrentSensorInfo() const
{
    return m_CurrentSensorID;
}

std::string IUAVNavigationArmShotAloneFireProtDialog::getCurrentComponentInfo() const
{
    return m_currentComponentID.toStdString();
}

void IUAVNavigationArmShotAloneFireProtDialog::bomBCallBackFunction(BombStatusWidget *bombStatusWidget, IUAVNavigationArmShotAloneFireProtDialog *dialog)
{
    if (bombStatusWidget == nullptr || dialog == nullptr) {
        return;
    }

    IUAVNavigationArmShotAloneFireProtDialog * navigationArmShotDialog = dialog;
    if (navigationArmShotDialog == nullptr || navigationArmShotDialog->ui == nullptr) {
        return;
    }

    BombStatusWidget * bombStatus = bombStatusWidget;
    if (bombStatus == nullptr) {
        return;
    }
    navigationArmShotDialog->m_singleSelectedBomb = true;
}

bool IUAVNavigationArmShotAloneFireProtDialog::InitBombState(bool resetBomb)
{
    if(ui == nullptr){
        return false;
    }

    time_t timeNow = ::time(nullptr);
    if ((timeNow - m_TimeOutBomb >= TIMER_FIRE_CONTROL_REFRESH / 1000) || resetBomb) {
        m_TimeOutBomb = ::time(nullptr);

        ui->widget_DYFireProt_BombType_A_1->setUnReign();
        ui->widget_DYFireProt_BombType_B_1->setUnReign();

        ResetCommandStatus();

        ui->ImageStatusWidget_DYFireProt_Pill_1_Command_Percussion_Status->setStatusDark();
        ui->ImageStatusWidget_DYFireProt_Pill_1_Command_Reset_Status->setStatusDark();

        ResetCommandStatus();

        ui->ImageStatusWidget_DYFireProt_Pill_2_Command_Percussion_Status->setStatusDark();
        ui->ImageStatusWidget_DYFireProt_Pill_2_Command_Reset_Status->setStatusDark();
    }

    if ((timeNow - m_TimeOut_DY_ZC_HUD_ControlInfo >= TIMER_FIRE_CONTROL_REFRESH / 1000)) {
        m_TimeOut_DY_ZC_HUD_ControlInfo = ::time(nullptr);
        ui->label_IUAVNavigationArmShotAloneFireProtDialog_DY_ZC_HUD_ControlInfo->setText("");
    }

    return true;
}

void IUAVNavigationArmShotAloneFireProtDialog::ResetCommandStatus()
{
    for(auto ImageStatus : m_DYFireProt_ImageStatusList_Pill_1){
        ImageStatus->setStatusDark();
    }

    for(auto ImageStatus : m_DYFireProt_ImageStatusList_Pill_2){
        ImageStatus->setStatusDark();
    }
}

bool IUAVNavigationArmShotAloneFireProtDialog::initSensorAndBomb(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoFireControlMap)
{
    bool result = false;
    std::string StartType("");
    std::string AutoAimingDisable("");
    std::string VideoControlDetectEnable("");
    std::string VideoControlButtonEnable("");
    std::string FireControlDisable("");

    std::string strCruise_IO1_Enable_Cruise_IO1_Disable("");
    std::string strCruise_IO2_Enable_Cruise_IO2_Disable("");
    std::string strCruise_Command_UnLock_Cruise_Command_Lock("");
    std::string strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot("");
    if(tmpSensorInfoFireControlMap.size() == 1){

        std::map<std::string, qnzkna::SensorManage::CSensorInfo>::iterator itrMapFirst = tmpSensorInfoFireControlMap.begin();
        std::string DetectType("");
        itrMapFirst->second.GetSensorParam(std::string("Detect"), DetectType);

        if ("FireControlFireProt" != itrMapFirst->second.GetSensorType() || DetectType == "1") {
            return false;
        }
        const std::string strSensorIDFirst(itrMapFirst->second.GetSensorID());
        const std::string strSensorNameFirst(itrMapFirst->second.GetSensorName());
        const std::string strSensorSimpleNameFirst(itrMapFirst->second.GetSensorSimpleName());
        const std::string strSensorTypeFirst(itrMapFirst->second.GetSensorType());

        itrMapFirst->second.GetSensorParam(std::string("Start"), StartType);
        itrMapFirst->second.GetSensorParam(std::string("AutoAimingDisable"), AutoAimingDisable);
        itrMapFirst->second.GetSensorParam(std::string("VideoControlDetectEnable"), VideoControlDetectEnable);
        itrMapFirst->second.GetSensorParam(std::string("VideoControlButtonEnable"), VideoControlButtonEnable);
        itrMapFirst->second.GetSensorParam(std::string("FireControlDisable"), FireControlDisable);
        std::string bombTypeDYFireProtFirst("");
        itrMapFirst->second.GetSensorParam(std::string("弹药类型FireProt"), bombTypeDYFireProtFirst);
        std::string videoDeviceTypeFirst("");
        itrMapFirst->second.GetSensorParam(std::string("观瞄画面设备协议"), videoDeviceTypeFirst);
        std::string videoDeviceURLFirst("");
        itrMapFirst->second.GetSensorParam(std::string("观瞄画面设备地址"), videoDeviceURLFirst);
        std::string videoDeviceURLFirstInfrared("");
        itrMapFirst->second.GetSensorParam(std::string("红外观瞄画面设备地址"), videoDeviceURLFirstInfrared);
        std::string videoDevicePortFirst("");
        itrMapFirst->second.GetSensorParam(std::string("观瞄画面设备端口号"), videoDevicePortFirst);
        std::string videoDeviceChannelFirst("");
        itrMapFirst->second.GetSensorParam(std::string("观瞄画面设备通道号"), videoDeviceChannelFirst);
        std::string videoDeviceUserNameFirst("");
        itrMapFirst->second.GetSensorParam(std::string("观瞄画面设备用户名"), videoDeviceUserNameFirst);
        std::string videoDevicePassWordFirst("");
        itrMapFirst->second.GetSensorParam(std::string("观瞄画面设备密码"), videoDevicePassWordFirst);

        std::transform(bombTypeDYFireProtFirst.begin(),    bombTypeDYFireProtFirst.end(),    bombTypeDYFireProtFirst.begin(), ::tolower);
        std::transform(videoDeviceTypeFirst.begin(), videoDeviceTypeFirst.end(), videoDeviceTypeFirst.begin(), ::tolower);
        m_videoDeviceType        = videoDeviceTypeFirst;
        m_videoDeviceURL         = videoDeviceURLFirst;
        m_videoDeviceURLInfrared = videoDeviceURLFirstInfrared;
        m_videoDevicePort        = videoDevicePortFirst;
        m_videoDeviceChannel     = videoDeviceChannelFirst;
        m_videoDeviceUserName    = videoDeviceUserNameFirst;
        m_videoDevicePassWord    = videoDevicePassWordFirst;

        m_CurrentSensorID = strSensorIDFirst;
        result = true;

        if((bombTypeDYFireProtFirst == std::string("video"))
            || (bombTypeDYFireProtFirst == std::string("detect"))
            || (bombTypeDYFireProtFirst == std::string("视频"))
            || (bombTypeDYFireProtFirst == std::string("侦察"))
            || (bombTypeDYFireProtFirst == std::string("侦察视频"))
            || (bombTypeDYFireProtFirst == std::string("视频显示"))
            ){
            m_DYFireProt_BombType = DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Detect;
        }else if((bombTypeDYFireProtFirst == std::string("rocket"))
                 || (bombTypeDYFireProtFirst == std::string("消防火箭弹"))
                 || (bombTypeDYFireProtFirst == std::string("rocket 消防火箭弹"))
                 || (bombTypeDYFireProtFirst == std::string("rocket"))
                 || (bombTypeDYFireProtFirst == std::string("rocket 消防火箭弹"))
                 || (bombTypeDYFireProtFirst == std::string("消防火箭弹"))
                 ){
            m_DYFireProt_BombType = DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Rocket;
        }else if((bombTypeDYFireProtFirst == std::string("cruise"))
                 || (bombTypeDYFireProtFirst == std::string("消防罐"))
                 || (bombTypeDYFireProtFirst == std::string("cruise 消防罐"))
                 || (bombTypeDYFireProtFirst == std::string("消防罐"))
                 || (bombTypeDYFireProtFirst == std::string("cruise 消防罐"))
                 ){
            m_DYFireProt_BombType = DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Cruise;

            itrMapFirst->second.GetSensorParam(std::string("Cruise_IO1_Enable;Cruise_IO1_Disable"), strCruise_IO1_Enable_Cruise_IO1_Disable);
            itrMapFirst->second.GetSensorParam(std::string("Cruise_IO2_Enable;Cruise_IO2_Disable"), strCruise_IO2_Enable_Cruise_IO2_Disable);
            itrMapFirst->second.GetSensorParam(std::string("Cruise_Command_UnLock;Cruise_Command_Lock"), strCruise_Command_UnLock_Cruise_Command_Lock);
            itrMapFirst->second.GetSensorParam(std::string("Cruise_Pill_Command_Reset;Cruise_Pill_1_Command_Shot;Cruise_Pill_2_Command_Shot;Cruise_Pill_3_Command_Shot;Cruise_Pill_4_Command_Shot"),
                                               strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot);
        }
    }

    if(m_DYFireProt_BombType == DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_None){

        if(ui != nullptr && ui->widget_DYFireProt_BombType_Rocket != nullptr && ui->groupBox_DYFireProt_BombType_A != nullptr){

            ui->widget_DYFireProt_BombType_Rocket->setVisible(false);
            ui->widget_DYFireProt_BombType_Rocket->hide();

            ui->groupBox_DYFireProt_BombType_A->setVisible(false);
            ui->groupBox_DYFireProt_BombType_A->hide();
            ui->groupBox_DYFireProt_BombType_B->setVisible(false);
            ui->groupBox_DYFireProt_BombType_B->hide();

            ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

            ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);

            ui->groupBox_DYFireProt_Middle->setFixedHeight(40);

            m_WindowTitle = (m_currentComponentName + "\xe6\xb6\x88\xe9\x98\xb2\xe4\xbe\xa6\xe6\x9f\xa5\xef\xbc\x9a");

        }
    }else if(m_DYFireProt_BombType == DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Detect){

        if(ui != nullptr && ui->widget_DYFireProt_BombType_Rocket != nullptr && ui->groupBox_DYFireProt_BombType_A != nullptr){

            ui->widget_DYFireProt_BombType_Rocket->setVisible(false);
            ui->widget_DYFireProt_BombType_Rocket->hide();

            ui->groupBox_DYFireProt_BombType_A->setVisible(false);
            ui->groupBox_DYFireProt_BombType_A->hide();
            ui->groupBox_DYFireProt_BombType_B->setVisible(false);
            ui->groupBox_DYFireProt_BombType_B->hide();

            ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

            ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);

            ui->groupBox_DYFireProt_Middle->setFixedHeight(40);

			m_WindowTitle = (tr2("传感器实时视场")+m_currentComponentName );

        }
    }else if(m_DYFireProt_BombType == DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Rocket){

        if(ui != nullptr && ui->widget_DYFireProt_BombType_Rocket != nullptr && ui->groupBox_DYFireProt_BombType_A != nullptr){
            ui->widget_DYFireProt_BombType_Rocket->setVisible(true);
            ui->widget_DYFireProt_BombType_Rocket->show();

            ui->groupBox_DYFireProt_BombType_A->setVisible(true);
            ui->groupBox_DYFireProt_BombType_A->show();
            ui->groupBox_DYFireProt_BombType_B->setVisible(false);
            ui->groupBox_DYFireProt_BombType_B->hide();

            ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

            ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);

			m_WindowTitle = (tr2("传感器实时视场") + m_currentComponentName);

            connect(ui->pushButton_DYFireProt_Pill_1_Command_Insure,                      &QPushButton::clicked,    this, &IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_1_Command_Insure);
            connect(ui->pushButton_DYFireProt_Pill_1_Command_Percussion,                  &QPushButton::clicked,    this, &IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_1_Command_Percussion);
            connect(ui->pushButton_DYFireProt_Pill_1_Command_Reset,                       &QPushButton::clicked,    this, &IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_1_Command_Reset);

        }
    }else if(m_DYFireProt_BombType == DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Cruise){

        if(ui != nullptr && ui->widget_DYFireProt_BombType_Rocket != nullptr && ui->groupBox_DYFireProt_BombType_A != nullptr){

            ui->widget_DYFireProt_BombType_Rocket->setVisible(false);
            ui->widget_DYFireProt_BombType_Rocket->show();

            ui->groupBox_DYFireProt_BombType_A->setVisible(false);
            ui->groupBox_DYFireProt_BombType_A->hide();
            ui->groupBox_DYFireProt_BombType_B->setVisible(true);
            ui->groupBox_DYFireProt_BombType_B->show();

            ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

            ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);

			m_WindowTitle = (tr2("传感器实时视场") + m_currentComponentName);

            connect(ui->pushButton_DYFireProt_Pill_2_Command_Insure,                      &QPushButton::clicked,    this, &IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_2_Command_Insure);
            connect(ui->pushButton_DYFireProt_Pill_2_Command_Percussion,                  &QPushButton::clicked,    this, &IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_2_Command_Percussion);
            connect(ui->pushButton_DYFireProt_Pill_2_Command_Reset,                       &QPushButton::clicked,    this, &IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_2_Command_Reset);
        }
    }else{
        if(ui != nullptr && ui->widget_DYFireProt_BombType_Rocket != nullptr && ui->groupBox_DYFireProt_BombType_A != nullptr){
            ui->widget_DYFireProt_BombType_Rocket->setVisible(false);
            ui->widget_DYFireProt_BombType_Rocket->hide();

            ui->groupBox_DYFireProt_BombType_A->setVisible(false);
            ui->groupBox_DYFireProt_BombType_A->hide();
            ui->groupBox_DYFireProt_BombType_B->setVisible(false);
            ui->groupBox_DYFireProt_BombType_B->hide();

            ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

            ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);

        }
    }

#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
    if(result && ui != nullptr && ui->widget_DYFireProt_Top_Video != nullptr && m_IPlayVideoManage == nullptr)
    {
        ui->widget_DYFireProt_Top_Video->clearFocus();

        if(QString::fromStdString(m_videoDeviceType).compare("rtsp", Qt::CaseInsensitive) == 0
            || QString::fromStdString(m_videoDeviceType).compare("hk", Qt::CaseInsensitive) == 0
            )
        {
            m_IPlayVideoManage = new qnzkna::PlayVideoManage::PlayVideoManageWidget(ui->widget_DYFireProt_Top_Video);
            m_IPlayVideoManage->setWindowTitle(QString::fromStdString(m_videoDeviceType));
            ui->widget_DYFireProt_Top_Video->setWindowTitle(QString::fromStdString(m_videoDeviceType));
        }
        else if(QString::fromStdString(m_videoDeviceType).compare("opencv", Qt::CaseInsensitive) == 0
                 || QString::fromStdString(m_videoDeviceType).compare("opencvFireProt", Qt::CaseInsensitive) == 0)
        {
            m_IPlayVideoManage = nullptr;
#ifdef PLAY_YOLO_VIDEO_STREAM_MANAGE_DEFINES
            m_IPlayVideoManage = new qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget(ui->widget_DYFireProt_Top_Video);
            ui->widget_DYFireProt_Top_Video->setWindowTitle(QString::fromStdString(m_videoDeviceType));
            m_IPlayVideoManage->setWindowTitle(QString::fromStdString(m_videoDeviceType));
#endif
        }
        else if(QString::fromStdString(m_videoDeviceType).compare("rtspFireProt", Qt::CaseInsensitive) == 0)
        {
            m_IPlayVideoManage = nullptr;
#ifdef PLAY_VIDEO_STREAM_MANAGE_PLAY_VIDEO_MANAGE_FIREPROT_DEFINES
            m_IPlayVideoManage = new qnzkna::PlayVideoManage::PlayVideoManageWidget54(ui->widget_DYFireProt_Top_Video);
#endif
        }

        if(VideoControlDetectEnable == "true" || VideoControlDetectEnable == "1"){
            if(m_IPlayVideoManage != nullptr){
                m_IPlayVideoManage->SetVideoControlDetect(true);
            }
        }else if(VideoControlDetectEnable == "false" || VideoControlDetectEnable == "0"){
            if(m_IPlayVideoManage != nullptr){
                m_IPlayVideoManage->SetVideoControlDetect(false);
            }
        }

        if(VideoControlButtonEnable == "false" || VideoControlButtonEnable == "0"){
            if(m_IPlayVideoManage != nullptr){
                m_IPlayVideoManage->SetVideoControlButton(false);
            }
        }

        QVBoxLayout* verticalLayout = new QVBoxLayout(ui->widget_DYFireProt_Top_Video);
        if(verticalLayout && m_IPlayVideoManage != nullptr){
            verticalLayout->addWidget(m_IPlayVideoManage);
            ui->widget_DYFireProt_Top_Video->setLayout(verticalLayout);
            const qnzkna::PlayVideoManage::CVideoDevice videoDevice(m_currentComponentID.toStdString(), QString::fromStdString(m_videoDeviceType).toLower().toStdString(), m_videoDeviceURL, m_videoDeviceURLInfrared, m_videoDevicePort, m_videoDeviceChannel, m_videoDeviceUserName, m_videoDevicePassWord);
            m_IPlayVideoManage->StartVideoManageObjectHost(videoDevice);

            if(StartType == "Start")
            {
                m_IPlayVideoManage->hide();
                on_Button_DYFireProt_GroupComponent_buttonClicked(nullptr);
            }else if(QString::fromStdString(m_videoDeviceType).compare("rtspFireProt", Qt::CaseInsensitive) == 0){
                m_IPlayVideoManage->hide();
                on_Button_DYFireProt_GroupComponent_buttonClicked(nullptr);
            }
        }
    }
#endif

#ifdef VIDEOHUDDIALOG_SHOW
    if(m_VideoHUDDialog == nullptr){
        m_VideoHUDDialog = new VideoHUDDialog(m_currentComponentID,ui->widget_DYFireProt_Top_Video);
        m_VideoHUDDialog->hide();

    }

#endif

    if(ui && ui->groupBox_DYFireProt_Middle  && ui->groupBox_DYFireProt_Middle->layout() && ui->horizontalLayout_DYFireProt_Bottom_Oper && FireControlDisable == "1"){

        ui->horizontalLayout_DYFireProt_Bottom_Oper->setEnabled(false);

        ui->widget_DYFireProt_BombType_Rocket->setEnabled(false);
        ui->widget_DYFireProt_BombType_Rocket->setVisible(false);
        ui->widget_DYFireProt_BombType_Rocket->hide();

        ui->groupBox_DYFireProt_BombType_A->setVisible(false);
        ui->groupBox_DYFireProt_BombType_A->hide();
        ui->groupBox_DYFireProt_BombType_B->setVisible(false);
        ui->groupBox_DYFireProt_BombType_B->hide();

        ui->label_IUAVNavigationArmShotAloneFireProtDialog_DY_ZC_HUD_ControlInfo->setEnabled(false);
        ui->label_IUAVNavigationArmShotAloneFireProtDialog_DY_ZC_HUD_ControlInfo->setVisible(false);
        ui->label_IUAVNavigationArmShotAloneFireProtDialog_DY_ZC_HUD_ControlInfo->hide();

        ui->groupBox_DYFireProt_Middle->setFixedHeight(40);
    }

    return result;
}

const QString IUAVNavigationArmShotAloneFireProtDialog::GetWindowTitle()
{
    return m_WindowTitle;
}

void IUAVNavigationArmShotAloneFireProtDialog::OnCommand(int nID, void *lParam)
{

#define USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMWEAPONSELECTCONTROL_ENABLE
#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMWEAPONSELECTCONTROL_ENABLE

    if(QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_CONTROL_ID != nID){
        return ;
    }

    if(lParam == nullptr){
        return ;
    }

    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemWeaponSelectControl* const systemWeaponSelectControl = static_cast<const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemWeaponSelectControl* const>(lParam);
    if(systemWeaponSelectControl == nullptr){
        return ;
    }

    if(systemWeaponSelectControl->head.head1 != QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_HEAD1
        || systemWeaponSelectControl->head.head2 != QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_HEAD2
        || systemWeaponSelectControl->head.interface_msgid != QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_CONTROL_ID){
        return ;
    }

    if(systemWeaponSelectControl->head.id != qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SYSTEMINTERNALINTERFACEHEAD_ID_SYSTEMWEAPONSELECTCONTROL){
        return ;
    }

    if(systemWeaponSelectControl->weaponType != qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemWeaponSelectControl_WeaponType_Enum::SystemWeaponSelectControl_WeaponType_UAV){
        return ;
    }

    if(systemWeaponSelectControl->weaponId == 0 || systemWeaponSelectControl->vehicleID == 0){
        return ;
    }

    if((m_currentComponentID != systemWeaponSelectControl->strComponentID)
        || (m_currentComponentName != systemWeaponSelectControl->strComponentName)){
        return ;
    }

    if(QString::number(systemWeaponSelectControl->weaponId) != m_currentComponentID){
        return ;
    }

    if(ui != nullptr && ui->groupBox_DYFireProt_Middle != nullptr){
        setStyleSheet((systemWeaponSelectControl->select ? QString("background-color: rgba(0, 0, 255, 100);") : QString("")));
        ui->groupBox_DYFireProt_Middle->setTitle(systemWeaponSelectControl->select ? (systemWeaponSelectControl->strComponentName) : "");

    }

#endif
}

bool IUAVNavigationArmShotAloneFireProtDialog::initSensorAndOptoelectronic(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoOptoelectronicMap)
{
    bool result = false;
    return result;

    if(tmpSensorInfoOptoelectronicMap.size() == 1){

        std::map<std::string, qnzkna::SensorManage::CSensorInfo>::iterator itrMapFirst = tmpSensorInfoOptoelectronicMap.begin();
        std::string typeOptoelectronic("");
        itrMapFirst->second.GetSensorParam(std::string("Type"), typeOptoelectronic);

        if ("Optoelectronic" == itrMapFirst->second.GetSensorType() && typeOptoelectronic == "FireProt") {

            const QString& strSensorIDFirst(QString::fromStdString(itrMapFirst->second.GetSensorID()));
            const QString& strSensorNameFirst(QString::fromStdString(itrMapFirst->second.GetSensorName()));
            const QString& strComponentIDFirst(QString::fromStdString(itrMapFirst->second.GetComponentID()));
            const QString& strcurrentComponentNameFirst(m_currentComponentName);

            if(strComponentIDFirst == m_currentComponentID){

                result = true;
            }

        }else{

            result = false;
        }
    }

    return result;
}

void IUAVNavigationArmShotAloneFireProtDialog::InitWeaponInfoList()
{

}

void IUAVNavigationArmShotAloneFireProtDialog::on_Button_DYFireProt_GroupComponent_buttonClicked(QAbstractButton *button)
{

    const qint64 currect = QDateTime::currentSecsSinceEpoch();
    if((QString::fromStdString(m_videoDeviceType).compare("opencv", Qt::CaseInsensitive) == 0 )
        || (QString::fromStdString(m_videoDeviceType).compare("opencvFireProt", Qt::CaseInsensitive) == 0 )
        || (QString::fromStdString(m_videoDeviceType).compare("rtspFireProt", Qt::CaseInsensitive) == 0 )
        || (QString::fromStdString(m_videoDeviceType).compare("rtsp", Qt::CaseInsensitive) == 0 && m_CurrentTime == 0)
        || (QString::fromStdString(m_videoDeviceType).compare("hk", Qt::CaseInsensitive) == 0 && currect - m_CurrentTime > 30)
        )
    {
        m_CurrentTime = currect;
        show();
        if(ui != nullptr && ui->widget_DYFireProt_Top_Video != nullptr && m_IPlayVideoManage != nullptr)
        {
            ui->widget_DYFireProt_Top_Video->show();
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
            m_IPlayVideoManage->show();
            const qnzkna::PlayVideoManage::CVideoDevice videoDevice(m_currentComponentID.toStdString(), QString::fromStdString(m_videoDeviceType).toLower().toStdString(), m_videoDeviceURL, m_videoDeviceURLInfrared, m_videoDevicePort, m_videoDeviceChannel, m_videoDeviceUserName, m_videoDevicePassWord);

            m_IPlayVideoManage->StartVideoManageThread(videoDevice);

            logInfo() << __FILE__ << currect << __LINE__ << " " << m_currentComponentID.toStdString().c_str() <<" " << m_videoDeviceURL.c_str() << " " << m_videoDeviceURLInfrared.c_str() ;
#endif
        }
    }

#ifdef VIDEOHUDDIALOG_SHOW
    if(m_VideoHUDDialog != nullptr){
        moveVideoHUDDialog();
        m_VideoHUDDialog->show();
    }
#endif

    if(button == nullptr){
        return ;
    }
    logInfo() << __FILE__ <<__FUNCTION__ <<" " <<__LINE__ <<" " << m_videoDeviceURL.c_str() <<" " << m_videoDeviceURLInfrared.c_str() << " test " << button->property(QPushButton_Property_ComponentName) << button->property(QPushButton_Property_ComponentID);
}

void IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_1_Command_Insure()
{
    if(m_DYFireProt_BombType != DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Rocket){

        LyMessageBox::warning(nullptr, u8"警告", u8"消防弹药类型和指令操作不匹配，指令未发送！", QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, u8"警告", u8"消防弹 操作：加锁！确定加锁吗？", QMessageBox::Yes | QMessageBox::No))
        {

            ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

            ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
            bool ok = false;
            const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
            if(!ok){
                LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
                return ;
            }

            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType     = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_FireRocket;
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FireRocket_Lock;

            SystemFireProtControl_SendMsgToAll(componentID, fireProtType, fireProtOperType);
#endif
        }
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"消防弹 操作 加锁";
}

void IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_1_Command_Percussion()
{
    if(m_DYFireProt_BombType != DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Rocket){

        LyMessageBox::warning(nullptr, u8"警告", u8"消防弹药类型和指令操作不匹配，指令未发送！", QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, u8"警告", u8"消防弹 操作：解锁！确定解锁吗？", QMessageBox::Yes | QMessageBox::No))
        {

            ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(true);

            ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
            bool ok = false;
            const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
            if(!ok){
                LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
                return ;
            }

            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType     = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_FireRocket;
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FireRocket_UnLock;

            SystemFireProtControl_SendMsgToAll(componentID, fireProtType, fireProtOperType);

            m_Timer.singleShot(20000, this, [this](){

                ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

                ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(false);
                ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(false);
                ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);
            });
#endif
        }
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"消防弹 操作 解锁";
}

void IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_1_Command_Reset()
{
    if(m_DYFireProt_BombType != DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Rocket){

        LyMessageBox::warning(nullptr, u8"警告", u8"消防弹药类型和指令操作不匹配，指令未发送！", QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, u8"警告", u8"消防弹 操作：发射！确定发射吗？", QMessageBox::Yes | QMessageBox::No))
        {

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
            bool ok = false;
            const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
            if(!ok){
                LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
                return ;
            }

            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType     = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_FireRocket;
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FireRocket_Shoot;

            SystemFireProtControl_SendMsgToAll(componentID, fireProtType, fireProtOperType);

            m_Timer.singleShot(10000, this, [this](){

                ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

                ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(false);
                ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(false);
                ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);
            });
#endif
        }
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"消防弹 操作 发射";
}

void IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_2_Command_Insure()
{
    if(m_DYFireProt_BombType != DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Cruise){

        LyMessageBox::warning(nullptr, u8"警告", u8"消防弹药类型和指令操作不匹配，指令未发送！", QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, u8"警告", u8"消防罐 操作：加锁！确定加锁吗？", QMessageBox::Yes | QMessageBox::No))
        {

            ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

            ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
            bool ok = false;
            const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
            if(!ok){
                LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
                return ;
            }

            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType     = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_FirePot;
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FirePot_Lock;

            SystemFireProtControl_SendMsgToAll(componentID, fireProtType, fireProtOperType);
#endif
        }
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"消防罐 操作 加锁";
}

void IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_2_Command_Percussion()
{
    if(m_DYFireProt_BombType != DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Cruise){

        LyMessageBox::warning(nullptr, u8"警告", u8"消防弹药类型和指令操作不匹配，指令未发送！", QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, u8"警告", u8"消防罐 操作：解锁！确定解锁吗？", QMessageBox::Yes | QMessageBox::No))
        {

            ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(false);
            ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

            ui->pushButton_DYFireProt_Pill_2_Command_Insure->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_2_Command_Percussion->setEnabled(true);
            ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(true);

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
            bool ok = false;
            const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
            if(!ok){
                LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
                return ;
            }

            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType     = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_FirePot;
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FirePot_UnLock;

            SystemFireProtControl_SendMsgToAll(componentID, fireProtType, fireProtOperType);

            m_Timer.singleShot(20000, this, [this](){
                ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(false);
                ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(false);
                ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

                ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);
            });
#endif
        }
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"消防罐 操作 解锁";
}

void IUAVNavigationArmShotAloneFireProtDialog::IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_2_Command_Reset()
{
    if(m_DYFireProt_BombType != DYFireProt_BombType_Enum::DYFireProt_BombType_Enum_Cruise){

        LyMessageBox::warning(nullptr, u8"警告", u8"消防弹药类型和指令操作不匹配，指令未发送！", QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, u8"警告", u8"消防罐 操作：抛投！确定抛投吗？", QMessageBox::Yes | QMessageBox::No))
        {

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
            bool ok = false;
            const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
            if(!ok){
                LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
                return ;
            }

            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType     = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_FirePot;
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FirePot_Throw;

            SystemFireProtControl_SendMsgToAll(componentID, fireProtType, fireProtOperType);

            m_Timer.singleShot(10000, this, [this](){
                ui->pushButton_DYFireProt_Pill_1_Command_Insure->setEnabled(false);
                ui->pushButton_DYFireProt_Pill_1_Command_Percussion->setEnabled(false);
                ui->pushButton_DYFireProt_Pill_1_Command_Reset->setEnabled(false);

                ui->pushButton_DYFireProt_Pill_2_Command_Reset->setEnabled(false);
            });
#endif
        }
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"消防罐 操作 抛投";
}

void IUAVNavigationArmShotAloneFireProtDialog::SendXygsDYFireProtKZMsg(const QString strQWeaponId, const std::string& currentSensorInfoID)
{
    if (m_currentComponentID.isEmpty()) {
        return ;
    }

    if (currentSensorInfoID.empty()) {
        return ;
    }

}

void IUAVNavigationArmShotAloneFireProtDialog::SendXygsDYFireProtKZMsg(const unsigned char* const message, const unsigned int msglength, const std::string& currentSensorInfoID)
{
    if (m_currentComponentID.isEmpty()) {
        return ;
    }

    if (currentSensorInfoID.empty()) {
        return ;
    }

    logInfo() << __FILE__ << __FUNCTION__ <<" " << __LINE__ <<" " << QString::fromStdString(currentSensorInfoID) << message[4] << message[5];

}

void IUAVNavigationArmShotAloneFireProtDialog::ShowXygsDYFireProtKZMsgStatusInfo(const QString strQSensorId, const QByteArray message)
{
    if(ui == nullptr){
        return ;
    }

    if(message.size() <= 0){
        return ;
    }

    if (m_currentComponentID.isEmpty()) {
        return ;
    }

    const std::string& strSensorId = strQSensorId.toStdString();

    if (m_CurrentSensorID != strSensorId) {
        return ;
    }

    m_TimeOutBomb = ::time(nullptr);
}

void IUAVNavigationArmShotAloneFireProtDialog::ShowDY_ZC_HUD_ControlInfo(const QString strQWeaponId, const QByteArray message)
{
    if(ui == nullptr){
        return ;
    }

    if(message.size() <= 0){
        return ;
    }

}

void UAVNavigationArmShotAloneFireProtBoxLayout::setGeometry(const QRect &rect)
{
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
    if(m_pIUAVNavigationArmShotAloneFireProtDialog && m_pIUAVNavigationArmShotAloneFireProtDialog->getPlayVideoManageWidget()){
        m_pIUAVNavigationArmShotAloneFireProtDialog->getPlayVideoManageWidget()->Lock();
        QVBoxLayout::setGeometry(rect);
        m_pIUAVNavigationArmShotAloneFireProtDialog->getPlayVideoManageWidget()->UnLock();
    }else{
        QVBoxLayout::setGeometry(rect);
    }
#else
    QVBoxLayout::setGeometry(rect);
#endif
}

