#include "IUAVNavigationArmShotAlone54Dialog.h"
#include "ui_IUAVNavigationArmShotAlone54Dialog.h"
#include "IUAVNavigationArmShotAlone54Activator.h"
#include "PlayVideoManage/IPlayVideoManage.h"
#include "ImageStatusWidget.h"
#include "IUAVNavigationArmShotAlone54CruiseControl.h"
#include "LY_UAVNavigationArmShotAlone54Interface.h"
#include "LY_UAVNavigationArmShotAlone54.h"

#include <QBuffer>
#include <QScreen>
#include <QSqlQuery>
#include <QNetworkDatagram>



#ifdef VIDEOHUDDIALOG_SHOW
    #include "VideoHUDDialog.h"
#endif

#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES

    #include "PlayVideoManage/PlayVideoManageWidget.h"

    #ifdef PLAY_YOLO_VIDEO_STREAM_MANAGE_DEFINES

        #include "PlayVideoManage/PlayYoloVideoStreamManageWidget.h"

    #endif

    #ifdef PLAY_VIDEO_STREAM_MANAGE_PLAY_VIDEO_MANAGE_54_DEFINES
        #include "PlayVideoManage/PlayVideoManageWidget54.h"
    #endif

#endif

#include "lyUI/LyMessageBox.h"
#include "netheader.h"

#include <QCString.h>


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
#define TIMER_FIRE_CONTROL_REFRESH  (1000 * 500)

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

#include "LY_UAVNavigationArmShotAlone54Interface.h"
#include "DataManage/SystemInfo/SystemInternalInterface.h"

void SystemFireProtControl_SendMsgToAll(const unsigned long long int    vehicle_id,       // 无人机ID
    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType,      // 消防 灭火罐 灭火弹的类型
    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType   // 消防 灭火罐 灭火弹的控制指令类型
)
{
    if (vehicle_id == 0)
    {
        return;
    }

    if (fireProtType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Nona)
    {
        return;
    }

    if (fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Nona)
    {
        return;
    }

    qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl systemFireProtControl;
    //memset(&systemFireProtControl, 0, sizeof(systemFireProtControl));
    systemFireProtControl.head.id          = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SYSTEMINTERNALINTERFACEHEAD_ID_SYSTEMFIREPROTCONTROL;
    systemFireProtControl.head.time        = QDateTime::currentMSecsSinceEpoch();
    systemFireProtControl.vehicle_id       = vehicle_id;
    systemFireProtControl.fireProtType     = fireProtType;
    systemFireProtControl.fireProtOperType = fireProtOperType;

    void *lParam = (void *)(&systemFireProtControl);
    if (lParam == nullptr)
    {
        return;
    }

    LY_UAVNavigationArmShotAlone54Interface::Get().SendMsgToAll(IInterface::MSG_Command, QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_CONTROL_ID, lParam);
}

void SystemFireProtControl_Data_SendMsgToAll(const unsigned long long int    vehicle_id,       // 无人机ID
    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType,      // 消防 灭火罐 灭火弹的类型
    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType,   // 消防 灭火罐 灭火弹的控制指令类型
    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data
)
{
    if (vehicle_id == 0)
    {
        return;
    }

    if (fireProtType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Nona)
    {
        return;
    }

    if (fireProtOperType == qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Nona)
    {
        return;
    }

    qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl systemFireProtControl;
    //memset(&systemFireProtControl, 0, sizeof(systemFireProtControl));
    systemFireProtControl.head.id          = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SYSTEMINTERNALINTERFACEHEAD_ID_SYSTEMFIREPROTCONTROL;
    systemFireProtControl.head.time        = QDateTime::currentMSecsSinceEpoch();
    systemFireProtControl.vehicle_id       = vehicle_id;
    systemFireProtControl.fireProtType     = fireProtType;
    systemFireProtControl.fireProtOperType = fireProtOperType;
    systemFireProtControl.data             = data;

    void *lParam = (void *)(&systemFireProtControl);
    if (lParam == nullptr)
    {
        return;
    }

    LY_UAVNavigationArmShotAlone54Interface::Get().SendMsgToAll(IInterface::MSG_Command, QNZKNA_DATAMANAGE_SYSTEMINFO_SYSTEMINTERNALINTERFACE_CONTROL_ID, lParam);
}

#else

void SendMsgToAll(const char *const message, const unsigned int length)
{
    if (message == nullptr)
    {
        return;
    }

    if (length != sizeof(qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl))
    {
        return;
    }

    void *lParam = (void *)(message);
    if (lParam == nullptr)
    {
        return;
    }
}

void SystemFireProtControl_Data_SendMsgToAll(const unsigned long long int    vehicle_id,       // 无人机ID
    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType,      // 消防 灭火罐 灭火弹的类型
    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType,   // 消防 灭火罐 灭火弹的控制指令类型
    const qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data
)
{
    if (message == nullptr)
    {
        return;
    }

    if (length != sizeof(qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl))
    {
        return;
    }

    void *lParam = (void *)(message);
    if (lParam == nullptr)
    {
        return;
    }
}
#endif

const char *IUAVNavigationArmShotAlone54Dialog::QPushButton_Property_ComponentName = "QPushButton_Property_ComponentNameDY54";
const char *IUAVNavigationArmShotAlone54Dialog::QPushButton_Property_ComponentID   = "QPushButton_Property_ComponentIDDY54";


IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog(const QString &currentComponentID, const QString &currentComponentName, QWidget *parent)
	: QWidget(parent)
    , ui(new Ui::IUAVNavigationArmShotAlone54Dialog)
    , m_StrTitle("")
    , m_VerticalBoxLayout(nullptr)
    , m_IPlayVideoManage(nullptr)
    , m_CurrentTime(0)
    , m_Timer()
    , m_Pill_CheckBox_All_Shot_Checked(false)
    , m_singleSelectedBomb(true)
    , m_CurrentSensorID("")
    , m_TimeOutBomb(0)
    , m_TimeOutBomb_38MM(0)
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
    , m_VideoType_54("")
    , m_videoControlSensorInfoID("")
    , m_SensorType("")
    , m_DY54_BombType(DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
    , m_IUAVNavigationArmShotAlone54CruiseControl(nullptr)
    , m_XygsDY95_1_SBUS_Command{0}
    , m_UseUdpSocket(false)
    , m_UdpHostAddressRemote("")
    , m_UdpHostAddressLocalPort(0)
    , m_UdpHostAddressRemotePort(0)
    , m_udpSocket(nullptr)
    , m_UseUdpSocket_95_1_StatusSync(false)
#ifdef VIDEOHUDDIALOG_SHOW
    , m_VideoHUDDialog(nullptr)
#endif
{
    m_DY54_ImageStatusList_Pill_1.clear();
    m_DY54_ImageStatusList_Pill_2.clear();
    m_pButtonGroupComponent = new QButtonGroup(this);
    m_pButtonGroupComponent->setObjectName(QString::fromUtf8("Button_DY54_GroupComponent"));

	ui->setupUi(this);

    ui->checkBox_DY54_LEFT_All_Shot->setVisible(false);
    ui->checkBox_DY54_LEFT_All_Shot->hide();
    ui->pushButton_DY54_LEFT_All_Shot->setVisible(false);
    ui->pushButton_DY54_LEFT_All_Shot->hide();

    ui->widget_DY54_Bottom_Oper->setVisible(false);
    ui->widget_DY54_Bottom_Oper->hide();

    ui->widget_DY54_BombType_Rocket->setVisible(false);
    ui->widget_DY54_BombType_Rocket->hide();

    ui->groupBox_38mmlauncher_Control->setVisible(false);
    ui->groupBox_38mmlauncher_Control->hide();

    ui->groupBox_95_1launcher_Control->setVisible(false);
    ui->groupBox_95_1launcher_Control->hide();

    ui->pushButton_38mmlauncher_Control_Select_ALL->setVisible(false);
    ui->pushButton_38mmlauncher_Control_Select_ALL->hide();

    ui->pushButton_38mmlauncher_Control_Select_None->setVisible(false);
    ui->pushButton_38mmlauncher_Control_Select_None->hide();


    ui->pushButton_38mmlauncher_Control_Percussion->setEnabled(false);

    ui->pushButton_95_1launcher_Control_Percussion->setEnabled(false);

    ui->verticalSlider_95_1launcher_Control_fuyang->setVisible(false);
    ui->verticalSlider_95_1launcher_Control_fuyang->hide();


    ui->groupBox_DY54_BombType_A->setAlignment(Qt::AlignCenter);
    ui->groupBox_DY54_BombType_B->setAlignment(Qt::AlignCenter);

    ui->groupBox_DY54_BombType_A->setStyleSheet(QString("QGroupBox::title { subcontrol-origin: margin; margin-left: 5; margin-right: 5; top: -3; color: #ffb848; }"));

    connect(ui->pushButton_DY54_LEFT_Video_Light,                           &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_LEFT_Video_Light);
    connect(ui->pushButton_DY54_LEFT_Video_Infrared,                        &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_LEFT_Video_Infrared);

    connect(ui->checkBox_DY54_LEFT_All_Shot,                                &QCheckBox::stateChanged, this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_Pill_CheckBox_All_Shot);
    connect(ui->pushButton_DY54_LEFT_All_Shot,                              &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_Pill_Command_All_Shot);

    connect(ui->pushButton_DY54_Pill_1_Command_Insure,                      &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Insure);
    connect(ui->pushButton_DY54_Pill_1_Command_Percussion,                  &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Percussion);
    connect(ui->pushButton_DY54_Pill_1_Command_Reset,                       &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Reset);

    connect(ui->pushButton_DY54_Pill_2_Command_Insure,                      &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Insure);
    connect(ui->pushButton_DY54_Pill_2_Command_Percussion,                  &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Percussion);
    connect(ui->pushButton_DY54_Pill_2_Command_Reset,                       &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Reset);

    connect(ui->pushButton_DY54_Right_CCIP_2,                               &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_StartAutoAiming);
    connect(ui->pushButton_DY54_Right_CCIP,                                 &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_StopAutoAiming);

    connect(ui->pushButton_DY54_BombType_Cruise_Command_UnLock,             &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock);
    connect(ui->pushButton_DY54_BombType_Cruise_Command_Lock,               &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock);
    connect(ui->pushButton_DY54_BombType_Cruise_Pill_Command_Reset,         &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset);
    connect(ui->pushButton_DY54_BombType_Cruise_Pill_1_Command_Shot,        &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot);
    connect(ui->pushButton_DY54_BombType_Cruise_Pill_2_Command_Shot,        &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot);
    connect(ui->pushButton_DY54_BombType_Cruise_Pill_3_Command_Shot,        &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot);
    connect(ui->pushButton_DY54_BombType_Cruise_Pill_4_Command_Shot,        &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot);

    {
        ui->pushButton_DY54_BombType_Cruise_Pill_Command_Reset->setEnabled(false);
        ui->pushButton_DY54_BombType_Cruise_Pill_1_Command_Shot->setEnabled(false);
        ui->pushButton_DY54_BombType_Cruise_Pill_2_Command_Shot->setEnabled(false);
        ui->pushButton_DY54_BombType_Cruise_Pill_3_Command_Shot->setEnabled(false);
        ui->pushButton_DY54_BombType_Cruise_Pill_4_Command_Shot->setEnabled(false);
    }

    {
        m_VerticalBoxLayout = new UAVNavigationArmShotAlone54BoxLayout(this, this);
        m_VerticalBoxLayout->setSpacing(2);
        m_VerticalBoxLayout->setObjectName(QString::fromUtf8("UAVNavigationArmShotAlone54BoxLayout"));
        m_VerticalBoxLayout->setContentsMargins(2, 2, 2, 2);
		//m_VerticalBoxLayout->setContentsMargins(7, 1, 1, 1);

        m_VerticalBoxLayout->addWidget(ui->widget_DY54_Top_Video);

        //ui->groupBox_DY54_Middle->setFixedHeight(230);
		ui->groupBox_DY54_Middle->setFixedHeight(60);
        m_VerticalBoxLayout->addWidget(ui->groupBox_DY54_Middle);

    }

    m_DY54_ImageStatusList_Pill_1.push_back(ui->ImageStatusWidget_DY54_Pill_1_Command_Insure_Status);
    m_DY54_ImageStatusList_Pill_1.push_back(ui->ImageStatusWidget_DY54_Pill_1_Command_Percussion_Status);
    m_DY54_ImageStatusList_Pill_1.push_back(ui->ImageStatusWidget_DY54_Pill_1_Command_Reset_Status);
    m_DY54_ImageStatusList_Pill_2.push_back(ui->ImageStatusWidget_DY54_Pill_2_Command_Insure_Status);
    m_DY54_ImageStatusList_Pill_2.push_back(ui->ImageStatusWidget_DY54_Pill_2_Command_Percussion_Status);
    m_DY54_ImageStatusList_Pill_2.push_back(ui->ImageStatusWidget_DY54_Pill_2_Command_Reset_Status);

    for (auto ImageStatus : m_DY54_ImageStatusList_Pill_1)
    {
        ImageStatus->setIUAVNavigationArmShotAlone54Dialog(this);
        ImageStatus->setImageStatusPixmap(QPixmap(":/resources/gallery/24x24@1.3/02.png"),
                                          QPixmap(":/resources/gallery/24x24@1.3/01.png"),
                                          QPixmap(":/resources/gallery/24x24@1.3/03.png"));
        ImageStatus->setBombStatusMiddlePixmap(QPixmap(":/resources/gallery/24x24/02.png"),
                                               QPixmap(":/resources/gallery/24x24/01.png"),
                                               QPixmap(":/resources/gallery/24x24/03.png"));
    }

    for (auto ImageStatus : m_DY54_ImageStatusList_Pill_2)
    {
        ImageStatus->setIUAVNavigationArmShotAlone54Dialog(this);
        ImageStatus->setImageStatusPixmap(QPixmap(":/resources/gallery/24x24@1.3/02.png"),
                                          QPixmap(":/resources/gallery/24x24@1.3/01.png"),
                                          QPixmap(":/resources/gallery/24x24@1.3/03.png"));
        ImageStatus->setBombStatusMiddlePixmap(QPixmap(":/resources/gallery/24x24/02.png"),
                                               QPixmap(":/resources/gallery/24x24/01.png"),
                                               QPixmap(":/resources/gallery/24x24/03.png"));
    }

    {
        {
            ui->widget_DY54_BombType_A_1->initBombData(BombStatusWidget::BombData(QT_TRANSLATE_NOOP("IUAVNavigationArmShotAlone54Dialog", "The rockA"),
                                                                                  BombStatusWidget::BombHangType::BombHangType_A,
                                                                                  QT_TRANSLATE_NOOP("IUAVNavigationArmShotAlone54Dialog", "The RocketsA2"),
                                                                                  BombStatusWidget::BombType::BombType_A2,
                                                                                  0x0001),
                                                       this,
                                                       bomBCallBackFunction);
            ui->widget_DY54_BombType_A_1->setBombStatusPixmap(QPixmap(":/resources/gallery/24x24@2/e1.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/e2.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/e3.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/e4.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/e5.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/e6.png"));
            ui->widget_DY54_BombType_A_1->setBombStatusMiddlePixmap(QPixmap(":/resources/gallery/24x24@1.3/e1.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/e2.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/e3.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/e4.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/e5.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/e6.png"));
        }

    }

    {
        {
            ui->widget_DY54_BombType_B_1->initBombData(BombStatusWidget::BombData(QT_TRANSLATE_NOOP("IUAVNavigationArmShotAlone54Dialog", "The rockA"),
                                                                                  BombStatusWidget::BombHangType::BombHangType_B,
                                                                                  QT_TRANSLATE_NOOP("IUAVNavigationArmShotAlone54Dialog", "The rockA"),
                                                                                  BombStatusWidget::BombType::BombType_B2,
                                                                                  0x0001),
                                                       this,
                                                       bomBCallBackFunction);
            ui->widget_DY54_BombType_B_1->setBombStatusPixmap(QPixmap(":/resources/gallery/24x24@2/f1.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/f2.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/f3.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/f4.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/f5.png"),
                                                              QPixmap(":/resources/gallery/24x24@2/f6.png"));
            ui->widget_DY54_BombType_B_1->setBombStatusMiddlePixmap(QPixmap(":/resources/gallery/24x24@1.3/f1.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/f2.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/f3.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/f4.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/f5.png"),
                                                                    QPixmap(":/resources/gallery/24x24@1.3/f6.png"));
        }

    }

    InitBombState(true);

    {
        QPushButton *weaponComponentButton = new QPushButton(m_currentComponentName, this);
        if (weaponComponentButton != nullptr)
        {
            weaponComponentButton->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed));
            weaponComponentButton->setMinimumSize(QSize(2, 2));
            weaponComponentButton->setCheckable(true);
            weaponComponentButton->setChecked(true);

            weaponComponentButton->setProperty(QPushButton_Property_ComponentName, m_currentComponentName);
            weaponComponentButton->setProperty(QPushButton_Property_ComponentID,   m_currentComponentID);
            this->ui->horizontalLayout_Vehicle->insertWidget(this->ui->horizontalLayout_Vehicle->count() - 1, weaponComponentButton);

            if (m_pButtonGroupComponent != nullptr)
            {
                m_pButtonGroupComponent->addButton(weaponComponentButton);
            }
        }
    }

    connect(&m_Timer, &QTimer::timeout, this, [this]()
    {
        this->InitBombState();
    });


    m_Timer.setInterval(TIMER_FIRE_CONTROL_REFRESH / 1000);
    m_Timer.start();

    connect(this, &IUAVNavigationArmShotAlone54Dialog::ShowXygsDY54KZMsgStatusInfoSig, this, &IUAVNavigationArmShotAlone54Dialog::ShowXygsDY54KZMsgStatusInfo, Qt::QueuedConnection);
    connect(this, &IUAVNavigationArmShotAlone54Dialog::ShowDY_ZC_HUD_ControlInfoSig,   this, &IUAVNavigationArmShotAlone54Dialog::ShowDY_ZC_HUD_ControlInfo,   Qt::QueuedConnection);

    ui->lineEdit_Target_New_ID->setAttribute(Qt::WA_InputMethodEnabled, false);
    QRegExp rx("[a-zA-Z0-9]{25}");
    QRegExpValidator *latitude = new QRegExpValidator(rx, this);
    ui->lineEdit_Target_New_ID->setValidator(latitude);

    InitTargetShotControl();
}

IUAVNavigationArmShotAlone54Dialog::~IUAVNavigationArmShotAlone54Dialog()
{
    if (ui != nullptr && ui->widget_DY54_Top_Video != nullptr && m_IPlayVideoManage != nullptr)
    {
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
        m_IPlayVideoManage->StopVideoManageThread();
#endif
    }

#ifdef VIDEOHUDDIALOG_SHOW
    if (m_VideoHUDDialog)
    {
        delete m_VideoHUDDialog;
    }
#endif

    delete ui;
}

bool IUAVNavigationArmShotAlone54Dialog::initSensorAndBomb(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoFireControlMap)
{
    bool result = false;

    m_SensorType = "";

    std::string StartType("");

    std::string strCruise_IO1_Enable_Cruise_IO1_Disable("");
    std::string strCruise_IO2_Enable_Cruise_IO2_Disable("");
    std::string strCruise_Command_UnLock_Cruise_Command_Lock("");
    std::string strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot("");
    if (tmpSensorInfoFireControlMap.size() == 1)
    {
        std::map<std::string, qnzkna::SensorManage::CSensorInfo>::iterator itrMapFirst = tmpSensorInfoFireControlMap.begin();
        std::string DetectType("");
        itrMapFirst->second.GetSensorParam(std::string("Detect"), DetectType);
        m_SensorType = itrMapFirst->second.GetSensorType();
        if ( ("FireControl54" != itrMapFirst->second.GetSensorType() && "FireControl38mm" != itrMapFirst->second.GetSensorType() && "FireControl95-1" != itrMapFirst->second.GetSensorType() && "FireControl95-1-Wolf" != itrMapFirst->second.GetSensorType() && 
			"FireControl38mmPWM" != itrMapFirst->second.GetSensorType() && "FireControlFireProt" != itrMapFirst->second.GetSensorType()) || DetectType == "1")
        {
            return false;
        }
        const std::string strSensorIDFirst(itrMapFirst->second.GetSensorID());
        const std::string strSensorNameFirst(itrMapFirst->second.GetSensorName());
        const std::string strSensorSimpleNameFirst(itrMapFirst->second.GetSensorSimpleName());
        const std::string strSensorTypeFirst(itrMapFirst->second.GetSensorType());

        itrMapFirst->second.GetSensorParam(std::string("Start"), StartType);
        std::string strTitle("");
        itrMapFirst->second.GetSensorParam(std::string("Title"), strTitle);
        std::string bombTypeDY54First("");
        itrMapFirst->second.GetSensorParam(std::string("弹药类型54"), bombTypeDY54First);
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
        std::string videoType54First("");
        itrMapFirst->second.GetSensorParam(std::string("videoType54"), videoType54First);

        std::string videoControlSensorInfoIDFirst("");
        itrMapFirst->second.GetSensorParam(std::string("videoControlSensorInfoID"), videoControlSensorInfoIDFirst);

        std::string useHostAddressFirst("");
        itrMapFirst->second.GetSensorParam(std::string("UseHostAddress"), useHostAddressFirst);
        std::string udpHostAddressFirst("");
        itrMapFirst->second.GetSensorParam(std::string("HostAddress"), udpHostAddressFirst);
        std::string udpHostAddressLocalPortFirst("");
        itrMapFirst->second.GetSensorParam(std::string("HostAddressLocalPort"), udpHostAddressLocalPortFirst);
        std::string udpHostAddressRemotePortFirst("");
        itrMapFirst->second.GetSensorParam(std::string("HostAddressRemotePort"), udpHostAddressRemotePortFirst);


        if (!strTitle.empty()) {
            m_StrTitle = QString(tr2(strTitle.c_str()));
        }
        else {
            m_StrTitle = QString::fromStdString(m_SensorType);
        }
        std::transform(bombTypeDY54First.begin(),    bombTypeDY54First.end(),    bombTypeDY54First.begin(), ::tolower);
        std::transform(videoDeviceTypeFirst.begin(), videoDeviceTypeFirst.end(), videoDeviceTypeFirst.begin(), ::tolower);
        m_videoDeviceType        = videoDeviceTypeFirst;
        m_videoDeviceURL         = videoDeviceURLFirst;
        m_videoDeviceURLInfrared = videoDeviceURLFirstInfrared;
        m_videoDevicePort        = videoDevicePortFirst;
        m_videoDeviceChannel     = videoDeviceChannelFirst;
        m_videoDeviceUserName    = videoDeviceUserNameFirst;
        m_videoDevicePassWord    = videoDevicePassWordFirst;
        m_VideoType_54           = videoType54First;

        m_videoControlSensorInfoID = QString::fromStdString(videoControlSensorInfoIDFirst);

        m_CurrentSensorID = strSensorIDFirst;

        if(useHostAddressFirst == "1"){
            m_UseUdpSocket = true;


            if(QHostAddress(QString::fromStdString(udpHostAddressFirst)).isNull()){
                m_UseUdpSocket = false;
            }

            m_UdpHostAddressRemote = QHostAddress(QString::fromStdString(udpHostAddressFirst));

            bool ok = false;
            const int udpHostAddressLocalPort = QString::fromStdString(udpHostAddressLocalPortFirst).toInt(&ok);
            if (!ok)
            {
                m_UseUdpSocket = false;
            }
            m_UdpHostAddressLocalPort = udpHostAddressLocalPort;

            const int udpHostAddressRemotePort = QString::fromStdString(udpHostAddressRemotePortFirst).toInt(&ok);
            if (!ok)
            {
                m_UseUdpSocket = false;
            }
            m_UdpHostAddressRemotePort = udpHostAddressRemotePort;
        }

        result = true;

        if ((bombTypeDY54First == std::string("video"))
                || (bombTypeDY54First == std::string("detect"))
                || (bombTypeDY54First == std::string("视频"))
                || (bombTypeDY54First == std::string("侦察"))
                || (bombTypeDY54First == std::string("侦察视频"))
                || (bombTypeDY54First == std::string("视频显示"))
           )
        {
            m_DY54_BombType = DY54_BombType_Enum::DY54_BombType_Enum_Rocket;
        }
        else if ((bombTypeDY54First == std::string("rocket"))
                 || (bombTypeDY54First == std::string("火箭弹"))
                 || (bombTypeDY54First == std::string("rocket 火箭弹"))
                 || (bombTypeDY54First == std::string("侦察"))
                 || (bombTypeDY54First == std::string("侦察视频"))
                 || (bombTypeDY54First == std::string("视频显示"))
                )
        {
            m_DY54_BombType = DY54_BombType_Enum::DY54_BombType_Enum_Rocket;
        }
        else if ((bombTypeDY54First == std::string("cruise"))
                 || (bombTypeDY54First == std::string("航弹"))
                 || (bombTypeDY54First == std::string("cruise 航弹"))
                 || (bombTypeDY54First == std::string("航空炸弹"))
                 || (bombTypeDY54First == std::string("cruise 航空炸弹"))
                )
        {
            m_DY54_BombType = DY54_BombType_Enum::DY54_BombType_Enum_Cruise;

            itrMapFirst->second.GetSensorParam(std::string("Cruise_IO1_Enable;Cruise_IO1_Disable"), strCruise_IO1_Enable_Cruise_IO1_Disable);
            itrMapFirst->second.GetSensorParam(std::string("Cruise_IO2_Enable;Cruise_IO2_Disable"), strCruise_IO2_Enable_Cruise_IO2_Disable);
            itrMapFirst->second.GetSensorParam(std::string("Cruise_Command_UnLock;Cruise_Command_Lock"), strCruise_Command_UnLock_Cruise_Command_Lock);
            itrMapFirst->second.GetSensorParam(std::string("Cruise_Pill_Command_Reset;Cruise_Pill_1_Command_Shot;Cruise_Pill_2_Command_Shot;Cruise_Pill_3_Command_Shot;Cruise_Pill_4_Command_Shot"),
                                               strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot);
        }
    }

    if ("FireControl54" == m_SensorType)
    {
        {
            ui->widget_DY54_Bottom_Oper->setVisible(true);
            ui->widget_DY54_Bottom_Oper->show();

            ui->groupBox_38mmlauncher_Control->setVisible(false);
            ui->groupBox_38mmlauncher_Control->hide();

            ui->groupBox_95_1launcher_Control->setVisible(false);
            ui->groupBox_95_1launcher_Control->hide();
        }

        if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_None)
        {
            if (ui != nullptr && ui->widget_DY54_BombType_Rocket != nullptr && ui->widget_DY54_BombType_Cruise != nullptr)
            {
                //ui->widget_DY54_BombType_Rocket->setVisible(true);
                //ui->widget_DY54_BombType_Rocket->show();

                ui->widget_DY54_BombType_Cruise->setVisible(false);
                ui->widget_DY54_BombType_Cruise->hide();
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()));
            }
        }
        else if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Detect)
        {
            if (ui != nullptr && ui->widget_DY54_BombType_Rocket != nullptr && ui->widget_DY54_BombType_Cruise != nullptr)
            {
                //ui->widget_DY54_BombType_Rocket->setVisible(true);
                //ui->widget_DY54_BombType_Rocket->show();

                ui->widget_DY54_BombType_Cruise->setVisible(false);
                ui->widget_DY54_BombType_Cruise->hide();
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()));
            }
        }
        else if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
        {
            if (ui != nullptr && ui->widget_DY54_BombType_Rocket != nullptr && ui->widget_DY54_BombType_Cruise != nullptr)
            {
                //ui->widget_DY54_BombType_Rocket->setVisible(true);
                //ui->widget_DY54_BombType_Rocket->show();

                ui->widget_DY54_BombType_Cruise->setVisible(false);
                ui->widget_DY54_BombType_Cruise->hide();
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()));
            }
        }
        else if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
        {
            if (ui != nullptr && ui->widget_DY54_BombType_Rocket != nullptr && ui->widget_DY54_BombType_Cruise != nullptr)
            {

                ui->widget_DY54_BombType_Rocket->setVisible(false);
                ui->widget_DY54_BombType_Rocket->hide();
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Insure()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Percussion()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Reset()));

                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Insure()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Percussion()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Reset()));

                ui->widget_DY54_BombType_Cruise->setVisible(true);
                ui->widget_DY54_BombType_Cruise->show();

                m_IUAVNavigationArmShotAlone54CruiseControl = new IUAVNavigationArmShotAlone54CruiseControl(this);
                if (m_IUAVNavigationArmShotAlone54CruiseControl)
                {
                    m_IUAVNavigationArmShotAlone54CruiseControl->InitWeaponComponent();

                    const QString &strCruise_IO1_Enable_Cruise_IO1_Disable_Temp(QString::fromStdString(strCruise_IO1_Enable_Cruise_IO1_Disable));
                    const QString &strCruise_IO2_Enable_Cruise_IO2_Disable_Temp(QString::fromStdString(strCruise_IO2_Enable_Cruise_IO2_Disable));
                    const QString &strCruise_Command_UnLock_Cruise_Command_Lock_Temp(QString::fromStdString(strCruise_Command_UnLock_Cruise_Command_Lock));
                    const QString &strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp(QString::fromStdString(strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot));

                    const QStringList &strCruise_IO1_Enable_Cruise_IO1_Disable_Temp_List = strCruise_IO1_Enable_Cruise_IO1_Disable_Temp.split(";");
                    const QStringList &strCruise_IO2_Enable_Cruise_IO2_Disable_Temp_List = strCruise_IO2_Enable_Cruise_IO2_Disable_Temp.split(";");
                    const QStringList &strCruise_Command_UnLock_Cruise_Command_Lock_Temp_List = strCruise_Command_UnLock_Cruise_Command_Lock_Temp.split(";");
                    const QStringList &strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List = strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp.split(";");

                    if (strCruise_IO1_Enable_Cruise_IO1_Disable_Temp_List.size() == 2)
                    {
                        if (!strCruise_IO1_Enable_Cruise_IO1_Disable_Temp_List.at(0).isEmpty() && !strCruise_IO1_Enable_Cruise_IO1_Disable_Temp_List.at(1).isEmpty())
                        {

                            bool ok = false;
                            const long long int Cruise_IO1_Enable_Value = strCruise_IO1_Enable_Cruise_IO1_Disable_Temp_List.at(0).toLongLong(&ok);

                            bool ok1 = false;
                            const long long int Cruise_IO1_Disable_Value = strCruise_IO1_Enable_Cruise_IO1_Disable_Temp_List.at(1).toLongLong(&ok1);

                            if (ok && ok1)
                            {
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO1_Enable_Value(Cruise_IO1_Enable_Value);
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO1_Disable_Value(Cruise_IO1_Disable_Value);
                            }
                        }
                    }

                    if (strCruise_IO2_Enable_Cruise_IO2_Disable_Temp_List.size() == 2)
                    {
                        if (!strCruise_IO2_Enable_Cruise_IO2_Disable_Temp_List.at(0).isEmpty() && !strCruise_IO2_Enable_Cruise_IO2_Disable_Temp_List.at(1).isEmpty())
                        {

                            bool ok = false;
                            const long long int Cruise_IO2_Enable_Value = strCruise_IO2_Enable_Cruise_IO2_Disable_Temp_List.at(0).toLongLong(&ok);

                            bool ok1 = false;
                            const long long int Cruise_IO2_Disable_Value = strCruise_IO2_Enable_Cruise_IO2_Disable_Temp_List.at(1).toLongLong(&ok1);

                            if (ok && ok1)
                            {
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO2_Enable_Value(Cruise_IO2_Enable_Value);
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_IO2_Disable_Value(Cruise_IO2_Disable_Value);
                            }
                        }
                    }

                    if (strCruise_Command_UnLock_Cruise_Command_Lock_Temp_List.size() == 2)
                    {
                        if (!strCruise_Command_UnLock_Cruise_Command_Lock_Temp_List.at(0).isEmpty() && !strCruise_Command_UnLock_Cruise_Command_Lock_Temp_List.at(1).isEmpty())
                        {

                            bool ok = false;
                            const long long int Cruise_Command_UnLock_Value = strCruise_Command_UnLock_Cruise_Command_Lock_Temp_List.at(0).toLongLong(&ok);

                            bool ok1 = false;
                            const long long int Cruise_Command_Lock_Value = strCruise_Command_UnLock_Cruise_Command_Lock_Temp_List.at(1).toLongLong(&ok1);

                            if (ok && ok1)
                            {
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Command_UnLock_Value(Cruise_Command_UnLock_Value);
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Command_Lock_Value(Cruise_Command_Lock_Value);
                            }
                        }
                    }

                    if (strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.size() == 5)
                    {
                        if (!strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(0).isEmpty()
                            && !strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(1).isEmpty()
                            && !strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(2).isEmpty()
                            && !strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(3).isEmpty()
                            && !strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(4).isEmpty()
                            )
                        {

                            bool ok = false;
                            const long long int Cruise_Pill_Command_Reset_Value = strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(0).toLongLong(&ok);

                            bool ok1 = false;
                            const long long int Cruise_Pill_1_Command_Shot_Value = strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(1).toLongLong(&ok1);

                            bool ok2 = false;
                            const long long int Cruise_Pill_2_Command_Shot_Value = strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(2).toLongLong(&ok2);

                            bool ok3 = false;
                            const long long int Cruise_Pill_3_Command_Shot_Value = strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(3).toLongLong(&ok3);

                            bool ok4 = false;
                            const long long int Cruise_Pill_4_Command_Shot_Value = strCruise_Pill_Command_Reset_Cruise_Pill_1_Command_Shot_Cruise_Pill_2_Command_Shot_Cruise_Pill_3_Command_Shot_Cruise_Pill_4_Command_Shot_Temp_List.at(4).toLongLong(&ok4);

                            if (ok && ok1 && ok2 && ok3 && ok4)
                            {
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_Command_Reset_Value(Cruise_Pill_Command_Reset_Value);
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_1_Command_Shot_Value(Cruise_Pill_1_Command_Shot_Value);
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_2_Command_Shot_Value(Cruise_Pill_2_Command_Shot_Value);
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_3_Command_Shot_Value(Cruise_Pill_3_Command_Shot_Value);
                                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54Dialog_Set_DY54_BombType_Cruise_Pill_4_Command_Shot_Value(Cruise_Pill_4_Command_Shot_Value);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if (ui != nullptr && ui->widget_DY54_BombType_Rocket != nullptr && ui->widget_DY54_BombType_Cruise != nullptr)
            {
                //ui->widget_DY54_BombType_Rocket->setVisible(true);
                //ui->widget_DY54_BombType_Rocket->show();

                ui->widget_DY54_BombType_Cruise->setVisible(false);
                ui->widget_DY54_BombType_Cruise->hide();
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()));
                disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()));
            }
        }
    }
    else if ("FireControl38mm" == m_SensorType)
    {
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()));

        if(m_UseUdpSocket){
            m_udpSocket = new QUdpSocket(this);
            m_udpSocket->bind(QHostAddress::Any, m_UdpHostAddressLocalPort);

            connect(m_udpSocket, &QUdpSocket::readyRead, this, [this]() {
                while (m_udpSocket->hasPendingDatagrams()) {
                    const QNetworkDatagram& datagram = m_udpSocket->receiveDatagram();
                    Deal_netheader_uctype_dy38mm_control(datagram.data().constData(), datagram.data().size());
                }
            });
        }

        m_DY54_BombType = DY54_BombType_Enum::DY54_BombType_Enum_38MM;

        ui->widget_DY54_Bottom_Oper->setVisible(false);
        ui->widget_DY54_Bottom_Oper->hide();

        ui->groupBox_38mmlauncher_Control->setVisible(true);
        ui->groupBox_38mmlauncher_Control->show();

        ui->groupBox_95_1launcher_Control->setVisible(false);
        ui->groupBox_95_1launcher_Control->hide();

        ui->pushButton_38mmlauncher_Control_puyang_stop->setVisible(false);
        ui->pushButton_38mmlauncher_Control_puyang_stop->hide();

        {
            static std::vector<QCheckBox*> AmmoEquip =
            {
                ui->checkBox_38mmlauncher_Control_equip_1,
                ui->checkBox_38mmlauncher_Control_equip_2,
                ui->checkBox_38mmlauncher_Control_equip_3,
                ui->checkBox_38mmlauncher_Control_equip_4,
                ui->checkBox_38mmlauncher_Control_equip_5,
                ui->checkBox_38mmlauncher_Control_equip_6
            };

            for (int i = 0; i < AmmoEquip.size(); ++i)
            {
                AmmoEquip[i]->setChecked(false);
                AmmoEquip[i]->setEnabled(false);
            }
        }

        connect(ui->pushButton_38mmlauncher_Control_Video_Light, &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Video_Light);
        connect(ui->pushButton_38mmlauncher_Control_Video_Infrared, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Video_Infrared);
        connect(ui->pushButton_38mmlauncher_Control_Video_Close, &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Video_Close);
        connect(ui->pushButton_38mmlauncher_Control_fuyang_up, &QPushButton::clicked,      this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_fuyang_up);
        connect(ui->pushButton_38mmlauncher_Control_puyang_down, &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_puyang_down);
        connect(ui->pushButton_38mmlauncher_Control_lock, &QPushButton::clicked,           this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_lock);
        connect(ui->pushButton_38mmlauncher_Control_unlock, &QPushButton::clicked,         this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_unlock);
        connect(ui->pushButton_38mmlauncher_Control_Percussion, &QPushButton::clicked,     this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Percussion);
    }
    else if ("FireControl95-1" == m_SensorType)
    {
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()));


        memset(&m_XygsDY95_1_SBUS_Command, 0, sizeof(m_XygsDY95_1_SBUS_Command));
        m_XygsDY95_1_SBUS_Command.head            = XygsDY95_1_SBUS_KZMSG_HEAD;
        m_XygsDY95_1_SBUS_Command.channel_11      = 1000; // 1500
        m_XygsDY95_1_SBUS_Command.channel_13      = 1000;
        m_XygsDY95_1_SBUS_Command.channel_15      = 500;// 1000;

        if( m_UseUdpSocket && ( !m_UdpHostAddressRemote.isNull() ) ){
            m_udpSocket = new QUdpSocket(this);
        }

        m_UseUdpSocket_95_1_StatusSync = LY_UAVNavigationArmShotAlone54::Get().getUseUdpSocket_95_1_StatusSync();

        m_DY54_BombType = DY54_BombType_Enum::DY54_BombType_Enum_95_1;

        ui->widget_DY54_Bottom_Oper->setVisible(false);
        ui->widget_DY54_Bottom_Oper->hide();

        ui->groupBox_38mmlauncher_Control->setVisible(false);
        ui->groupBox_38mmlauncher_Control->hide();

        ui->groupBox_95_1launcher_Control->setVisible(true);
        ui->groupBox_95_1launcher_Control->show();


        ui->pushButton_95_1launcher_Control_fuyang_up->setVisible(false);
        ui->pushButton_95_1launcher_Control_fuyang_up->hide();

        ui->pushButton_95_1launcher_Control_puyang_down->setVisible(false);
        ui->pushButton_95_1launcher_Control_puyang_down->hide();

        ui->pushButton_95_1launcher_Control_center->setVisible(false);
        ui->pushButton_95_1launcher_Control_center->hide();

        ui->pushButton_95_1launcher_Control_shuiping_left->setVisible(false);
        ui->pushButton_95_1launcher_Control_shuiping_left->hide();

        ui->pushButton_95_1launcher_Control_shuiping_right->setVisible(false);
        ui->pushButton_95_1launcher_Control_shuiping_right->hide();

        ui->verticalSlider_95_1launcher_Control_fuyang->setVisible(true);
        ui->verticalSlider_95_1launcher_Control_fuyang->show();

        //ui->pushButton_95_1launcher_Control_lock->setVisible(false);
        //ui->pushButton_95_1launcher_Control_lock->hide();

        //ui->pushButton_95_1launcher_Control_unlock->setVisible(false);
        //ui->pushButton_95_1launcher_Control_unlock->hide();


        connect(ui->pushButton_95_1launcher_Control_Video_Light, &QPushButton::clicked,          this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Light);
        connect(ui->pushButton_95_1launcher_Control_Video_Light_Infrared, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Light_Infrared);
        connect(ui->pushButton_95_1launcher_Control_Video_Infrared, &QPushButton::clicked,       this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Infrared);
        connect(ui->pushButton_95_1launcher_Control_Video_Close, &QPushButton::clicked,          this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Close);
        connect(ui->pushButton_95_1launcher_Control_jiguang_open, &QPushButton::clicked,         this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_jiguang_open);
        connect(ui->pushButton_95_1launcher_Control_jiguang_close, &QPushButton::clicked,        this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_jiguang_close);
        connect(ui->pushButton_95_1launcher_Control_focus_far, &QPushButton::clicked,            this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_focus_far);
        connect(ui->pushButton_95_1launcher_Control_focus_near, &QPushButton::clicked,           this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_focus_near);
        connect(ui->pushButton_95_1launcher_Control_focus_stop, &QPushButton::clicked,           this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_focus_stop);
        connect(ui->pushButton_95_1launcher_Control_fuyang_up, &QPushButton::clicked,            this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_fuyang_up);
        connect(ui->pushButton_95_1launcher_Control_puyang_down, &QPushButton::clicked,          this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_puyang_down);
        connect(ui->verticalSlider_95_1launcher_Control_fuyang, &QSlider::valueChanged,          this, &IUAVNavigationArmShotAlone54Dialog::on_verticalSlider_95_1launcher_Control_fuyang);
        connect(ui->pushButton_95_1launcher_Control_lock, &QPushButton::clicked,                 this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_lock);
        connect(ui->pushButton_95_1launcher_Control_unlock, &QPushButton::clicked,               this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_unlock);
        connect(ui->pushButton_95_1launcher_Control_Percussion, &QPushButton::clicked,           this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Percussion);
    }
    else if ("FireControl95-1-Wolf" == m_SensorType)
    {
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()));

        // m_udpSocket = new QUdpSocket(this);

        m_DY54_BombType = DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf;

        ui->widget_DY54_Bottom_Oper->setVisible(false);
        ui->widget_DY54_Bottom_Oper->hide();

        ui->groupBox_38mmlauncher_Control->setVisible(false);
        ui->groupBox_38mmlauncher_Control->hide();

        ui->groupBox_95_1launcher_Control->setVisible(true);
        ui->groupBox_95_1launcher_Control->show();


        ui->pushButton_95_1launcher_Control_fuyang_up->setVisible(false);
        ui->pushButton_95_1launcher_Control_fuyang_up->hide();

        ui->pushButton_95_1launcher_Control_puyang_down->setVisible(false);
        ui->pushButton_95_1launcher_Control_puyang_down->hide();

        ui->pushButton_95_1launcher_Control_center->setVisible(false);
        ui->pushButton_95_1launcher_Control_center->hide();

        ui->pushButton_95_1launcher_Control_shuiping_left->setVisible(false);
        ui->pushButton_95_1launcher_Control_shuiping_left->hide();

        ui->pushButton_95_1launcher_Control_shuiping_right->setVisible(false);
        ui->pushButton_95_1launcher_Control_shuiping_right->hide();

        ui->verticalSlider_95_1launcher_Control_fuyang->setVisible(false);
        ui->verticalSlider_95_1launcher_Control_fuyang->hide();

        //ui->pushButton_95_1launcher_Control_lock->setVisible(false);
        //ui->pushButton_95_1launcher_Control_lock->hide();

        //ui->pushButton_95_1launcher_Control_unlock->setVisible(false);
        //ui->pushButton_95_1launcher_Control_unlock->hide();


        connect(ui->pushButton_95_1launcher_Control_Video_Light, &QPushButton::clicked,          this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Light);
        connect(ui->pushButton_95_1launcher_Control_Video_Infrared, &QPushButton::clicked,       this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Infrared);
        connect(ui->pushButton_95_1launcher_Control_Video_Light_Infrared, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Light_Infrared);
        connect(ui->pushButton_95_1launcher_Control_Video_Close, &QPushButton::clicked,          this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Close);
        connect(ui->pushButton_95_1launcher_Control_jiguang_open, &QPushButton::clicked,         this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_jiguang_open);
        connect(ui->pushButton_95_1launcher_Control_jiguang_close, &QPushButton::clicked,        this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_jiguang_close);
        connect(ui->pushButton_95_1launcher_Control_focus_far, &QPushButton::clicked,            this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_focus_far);
        connect(ui->pushButton_95_1launcher_Control_focus_near, &QPushButton::clicked,           this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_focus_near);
        connect(ui->pushButton_95_1launcher_Control_focus_stop, &QPushButton::clicked,           this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_focus_stop);
        connect(ui->pushButton_95_1launcher_Control_fuyang_up, &QPushButton::clicked,            this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_fuyang_up);
        connect(ui->pushButton_95_1launcher_Control_puyang_down, &QPushButton::clicked,          this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_puyang_down);
        connect(ui->pushButton_95_1launcher_Control_lock, &QPushButton::clicked,                 this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_lock);
        connect(ui->pushButton_95_1launcher_Control_unlock, &QPushButton::clicked,               this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_unlock);
        connect(ui->pushButton_95_1launcher_Control_Percussion, &QPushButton::clicked,           this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Percussion);
    }
    else if ("FireControl38mmPWM" == m_SensorType)
    {
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()));
        disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()));



        m_DY54_BombType = DY54_BombType_Enum::DY54_BombType_Enum_38MM_PWM;

        ui->widget_DY54_Bottom_Oper->setVisible(false);
        ui->widget_DY54_Bottom_Oper->hide();

        ui->groupBox_38mmlauncher_Control->setVisible(true);
        ui->groupBox_38mmlauncher_Control->show();

        ui->groupBox_38mmlauncher_Control_3->setVisible(false);
        ui->groupBox_38mmlauncher_Control_3->hide();

        ui->groupBox_95_1launcher_Control->setVisible(false);
        ui->groupBox_95_1launcher_Control->hide();

        {
            static std::vector<QCheckBox*> AmmoEquip =
            {
                ui->checkBox_38mmlauncher_Control_equip_1,
                ui->checkBox_38mmlauncher_Control_equip_2,
                ui->checkBox_38mmlauncher_Control_equip_3,
                ui->checkBox_38mmlauncher_Control_equip_4,
                ui->checkBox_38mmlauncher_Control_equip_5,
                ui->checkBox_38mmlauncher_Control_equip_6
            };

            for (int i = 0; i < AmmoEquip.size(); ++i)
            {
                AmmoEquip[i]->setChecked(false);
                AmmoEquip[i]->setEnabled(false);
            }
        }

        connect(ui->pushButton_38mmlauncher_Control_Video_Light, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Video_Light);
        connect(ui->pushButton_38mmlauncher_Control_Video_Infrared, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Video_Infrared);
        connect(ui->pushButton_38mmlauncher_Control_Video_Close, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Video_Close);
        connect(ui->pushButton_38mmlauncher_Control_fuyang_up, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_fuyang_up);
        connect(ui->pushButton_38mmlauncher_Control_puyang_down, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_puyang_down);
        connect(ui->pushButton_38mmlauncher_Control_puyang_stop, &QPushButton::clicked,    this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_puyang_stop);
        connect(ui->pushButton_38mmlauncher_Control_lock, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_lock);
        connect(ui->pushButton_38mmlauncher_Control_unlock, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_unlock);
        connect(ui->pushButton_38mmlauncher_Control_Percussion, &QPushButton::clicked, this, &IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Percussion);
    }
	else if ("FireControlFireProt" == m_SensorType)
	{
		disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()));
		disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()));
		disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()));
		disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()));
		disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()));
		disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()));
		disconnect(nullptr, nullptr, SLOT(IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()));



		m_DY54_BombType = DY54_BombType_Enum::DY54_BombType_Enum_None;

		ui->widget_DY54_Bottom_Oper->setVisible(false);
		ui->widget_DY54_Bottom_Oper->hide(); 

		ui->groupBox_38mmlauncher_Control->setVisible(false);
		ui->groupBox_38mmlauncher_Control->hide();

		ui->groupBox_38mmlauncher_Control_3->setVisible(false);
		ui->groupBox_38mmlauncher_Control_3->hide();

		ui->groupBox_95_1launcher_Control->setVisible(false);
		ui->groupBox_95_1launcher_Control->hide();

		//{
		//	static std::vector<QCheckBox*> AmmoEquip =
		//	{
		//		ui->checkBox_38mmlauncher_Control_equip_1,
		//		ui->checkBox_38mmlauncher_Control_equip_2,
		//		ui->checkBox_38mmlauncher_Control_equip_3,
		//		ui->checkBox_38mmlauncher_Control_equip_4,
		//		ui->checkBox_38mmlauncher_Control_equip_5,
		//		ui->checkBox_38mmlauncher_Control_equip_6
		//	};
		//	for (int i = 0; i < AmmoEquip.size(); ++i)
		//	{
		//		AmmoEquip[i]->setChecked(false);
		//		AmmoEquip[i]->setEnabled(false);
		//	}
		//}

	}


#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
    if (result && ui != nullptr && ui->widget_DY54_Top_Video != nullptr && m_IPlayVideoManage == nullptr)
    {
        ui->widget_DY54_Top_Video->clearFocus();

        if (QString::fromStdString(m_videoDeviceType).compare("rtsp", Qt::CaseInsensitive) == 0
                || QString::fromStdString(m_videoDeviceType).compare("hk", Qt::CaseInsensitive) == 0
           )
        {
            m_IPlayVideoManage = new qnzkna::PlayVideoManage::PlayVideoManageWidget(ui->widget_DY54_Top_Video);
            m_IPlayVideoManage->setWindowTitle(QString::fromStdString(m_videoDeviceType));
            ui->widget_DY54_Top_Video->setWindowTitle(QString::fromStdString(m_videoDeviceType));
        }
        else if (QString::fromStdString(m_videoDeviceType).compare("opencv", Qt::CaseInsensitive) == 0
                 || QString::fromStdString(m_videoDeviceType).compare("opencv54", Qt::CaseInsensitive) == 0)
        {
            m_IPlayVideoManage = nullptr;
#ifdef PLAY_YOLO_VIDEO_STREAM_MANAGE_DEFINES
            m_IPlayVideoManage = new qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget(ui->widget_DY54_Top_Video);
            ui->widget_DY54_Top_Video->setWindowTitle(QString::fromStdString(m_videoDeviceType));
            m_IPlayVideoManage->setWindowTitle(QString::fromStdString(m_videoDeviceType));
#endif
        }
        else if (QString::fromStdString(m_videoDeviceType).compare("rtsp54", Qt::CaseInsensitive) == 0)
        {
            m_IPlayVideoManage = nullptr;
#ifdef PLAY_VIDEO_STREAM_MANAGE_PLAY_VIDEO_MANAGE_54_DEFINES
            m_IPlayVideoManage = new qnzkna::PlayVideoManage::PlayVideoManageWidget54(ui->widget_DY54_Top_Video);
#endif
        }

        if (QString::fromStdString(m_videoDeviceType).compare("rtsp", Qt::CaseInsensitive) == 0
                || QString::fromStdString(m_videoDeviceType).compare("rtsp54", Qt::CaseInsensitive) == 0)
        {
            QString str = QString::fromLocal8Bit(QByteArray::fromStdString(m_videoDeviceURL));

            if (str.isEmpty())
                str = QString::fromLocal8Bit(QByteArray::fromStdString(m_videoDeviceURLInfrared));

            if (!str.isEmpty())
            {
                auto* pISensor = IUAVNavigationArmShotAlone54Activator::Get().getPISensorVedioService();
                if (nullptr != pISensor)
                {
                    pISensor->AddUrl(m_currentComponentID.toLongLong(), str);
                }
            }
        }

        QVBoxLayout *verticalLayout = new QVBoxLayout(ui->widget_DY54_Top_Video);
        if (verticalLayout && m_IPlayVideoManage != nullptr)
        {
            verticalLayout->addWidget(m_IPlayVideoManage);
            ui->widget_DY54_Top_Video->setLayout(verticalLayout);
            const qnzkna::PlayVideoManage::CVideoDevice videoDevice(m_currentComponentID.toStdString(), QString::fromStdString(m_videoDeviceType).toLower().toStdString(), m_videoDeviceURL, m_videoDeviceURLInfrared, m_videoDevicePort, m_videoDeviceChannel, m_videoDeviceUserName, m_videoDevicePassWord, m_VideoType_54);
            m_IPlayVideoManage->StartVideoManageObjectHost(videoDevice);

            if (StartType == "Start")
            {
                m_IPlayVideoManage->hide();
                on_Button_DY54_GroupComponent_buttonClicked(nullptr);
            }
            else if (QString::fromStdString(m_videoDeviceType).compare("rtsp54", Qt::CaseInsensitive) == 0)
            {
                m_IPlayVideoManage->hide();
                on_Button_DY54_GroupComponent_buttonClicked(nullptr);
            }
        }
    }
#endif

#ifdef VIDEOHUDDIALOG_SHOW
    if (m_VideoHUDDialog == nullptr)
    {
        m_VideoHUDDialog = new VideoHUDDialog(m_currentComponentID, ui->widget_DY54_Top_Video);
        m_VideoHUDDialog->hide();
    }
#endif

    return result;
}

bool IUAVNavigationArmShotAlone54Dialog::initSensorAndOptoelectronic(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoOptoelectronicMap)
{
    bool result = false;
    if (tmpSensorInfoOptoelectronicMap.size() == 1)
    {
        std::map<std::string, qnzkna::SensorManage::CSensorInfo>::iterator itrMapFirst = tmpSensorInfoOptoelectronicMap.begin();
        std::string typeOptoelectronic("");
        itrMapFirst->second.GetSensorParam(std::string("Type"), typeOptoelectronic);
        if ("Optoelectronic" == itrMapFirst->second.GetSensorType() && typeOptoelectronic == "gaode54")
        {

            const QString &strSensorIDFirst(QString::fromStdString(itrMapFirst->second.GetSensorID()));
            const QString &strSensorNameFirst(QString::fromStdString(itrMapFirst->second.GetSensorName()));
            const QString &strComponentIDFirst(QString::fromStdString(itrMapFirst->second.GetComponentID()));
            const QString &strcurrentComponentNameFirst(m_currentComponentName);

            if (strComponentIDFirst == m_currentComponentID)
            {

                result = true;
            }

        }
        else
        {

            result = false;
        }
    }

    return result;
}

void IUAVNavigationArmShotAlone54Dialog::InitWeaponInfoList()
{

}

void IUAVNavigationArmShotAlone54Dialog::on_Button_DY54_GroupComponent_buttonClicked(QAbstractButton *button)
{
    const qint64 currect = QDateTime::currentSecsSinceEpoch();
    if ((QString::fromStdString(m_videoDeviceType).compare("opencv", Qt::CaseInsensitive) == 0)
            || (QString::fromStdString(m_videoDeviceType).compare("opencv54", Qt::CaseInsensitive) == 0)
            || (QString::fromStdString(m_videoDeviceType).compare("rtsp54", Qt::CaseInsensitive) == 0)
            || (QString::fromStdString(m_videoDeviceType).compare("rtsp", Qt::CaseInsensitive) == 0 && m_CurrentTime == 0)
            || (QString::fromStdString(m_videoDeviceType).compare("hk", Qt::CaseInsensitive) == 0 && currect - m_CurrentTime > 30)
       )
    {
        m_CurrentTime = currect;
        show();
        if (ui != nullptr && ui->widget_DY54_Top_Video != nullptr && m_IPlayVideoManage != nullptr)
        {
            ui->widget_DY54_Top_Video->show();
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
            m_IPlayVideoManage->show();
            const qnzkna::PlayVideoManage::CVideoDevice videoDevice(m_currentComponentID.toStdString(), QString::fromStdString(m_videoDeviceType).toLower().toStdString(), m_videoDeviceURL, m_videoDeviceURLInfrared, m_videoDevicePort, m_videoDeviceChannel, m_videoDeviceUserName, m_videoDevicePassWord, m_VideoType_54);

            m_IPlayVideoManage->StartVideoManageThread(videoDevice);

            logInfo() << __FILE__ << currect << __LINE__ << " " << m_currentComponentID.toStdString().c_str() << " " << m_videoDeviceURL.c_str() << " " << m_videoDeviceURLInfrared.c_str() ;
#endif
        }
    }

#ifdef VIDEOHUDDIALOG_SHOW
    if (m_VideoHUDDialog != nullptr)
    {
        moveVideoHUDDialog();
        m_VideoHUDDialog->show();
    }
#endif

    if (button == nullptr)
    {
        return ;
    }
    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << m_videoDeviceURL.c_str() << " " << m_videoDeviceURLInfrared.c_str() << " test " << button->property(QPushButton_Property_ComponentName) << button->property(QPushButton_Property_ComponentID);
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_LEFT_Video_Light()
{
    if (ui != nullptr && ui->widget_DY54_Top_Video != nullptr && m_IPlayVideoManage != nullptr)
    {
        ui->widget_DY54_Top_Video->show();
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
        m_IPlayVideoManage->show();
        m_IPlayVideoManage->setVideoType(2);
#endif
    }

    on_pushButton_95_1launcher_Control_Video_Light();
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_LEFT_Video_Infrared()
{
 if (ui != nullptr && ui->widget_DY54_Top_Video != nullptr && m_IPlayVideoManage != nullptr)
 {
    ui->widget_DY54_Top_Video->show();
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
    m_IPlayVideoManage->show();
    m_IPlayVideoManage->setVideoType(1);
#endif
 }

    on_pushButton_95_1launcher_Control_Video_Close();
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Insure()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket && ui->widget_DY54_BombType_A_1->getSelected())
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.1 Projectile operation: Insurance! Are you sure Insurance?"), QMessageBox::Yes | QMessageBox::No))
        {

            ResetCommandStatus(XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_1);

            const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x01, 0x01, 0X7D, 0X7D};
            SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);

        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Percussion()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket && ui->widget_DY54_BombType_A_1->getSelected())
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.1 Projectile operation: Cocking! Are you sure cocking?"), QMessageBox::Yes | QMessageBox::No))
        {

            ui->ImageStatusWidget_DY54_Pill_1_Command_Percussion_Status->setStatusDark();
            ui->ImageStatusWidget_DY54_Pill_1_Command_Reset_Status->setStatusDark();

            const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x01, 0x02, 0X7D, 0X7D};
            SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);

        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Reset()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket && ui->widget_DY54_BombType_A_1->getSelected())
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.1 Projectile operation: Reset! Are you sure reset?"), QMessageBox::Yes | QMessageBox::No))
        {

            ResetCommandStatus(XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_1);

            const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x01, 0x03, 0X7D, 0X7D};
            SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);

        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Insure()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket && ui->widget_DY54_BombType_B_1->getSelected())
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.2 Projectile operation: Insurance! Are you sure Insurance?"), QMessageBox::Yes | QMessageBox::No))
        {

            ResetCommandStatus(XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_2);

            const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x02, 0x01, 0X7D, 0X7D};
            SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);

        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Percussion()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket && ui->widget_DY54_BombType_B_1->getSelected())
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.2 Projectile operation: Cocking! Are you sure Cocking?"), QMessageBox::Yes | QMessageBox::No))
        {

            ui->ImageStatusWidget_DY54_Pill_2_Command_Percussion_Status->setStatusDark();
            ui->ImageStatusWidget_DY54_Pill_2_Command_Reset_Status->setStatusDark();

            const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x02, 0x02, 0X7D, 0X7D};
            SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);

        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Reset()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket && ui->widget_DY54_BombType_B_1->getSelected())
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.2 Projectile operation: reset! Are you sure reset?"), QMessageBox::Yes | QMessageBox::No))
        {

            ResetCommandStatus(XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_2);

            const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x02, 0x03, 0X7D, 0X7D};
            SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);

        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise && ui->pushButton_DY54_BombType_Cruise_Command_UnLock)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("Cruise Projectile operation: Unlock! Are you sure Unlock?"), QMessageBox::Yes | QMessageBox::No))
        {
            if (m_IUAVNavigationArmShotAlone54CruiseControl)
            {
                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Command_UnLock(m_currentComponentID);
            }

            {
                ui->pushButton_DY54_BombType_Cruise_Pill_Command_Reset->setEnabled(true);
                ui->pushButton_DY54_BombType_Cruise_Pill_1_Command_Shot->setEnabled(true);
                ui->pushButton_DY54_BombType_Cruise_Pill_2_Command_Shot->setEnabled(true);
                ui->pushButton_DY54_BombType_Cruise_Pill_3_Command_Shot->setEnabled(true);
                ui->pushButton_DY54_BombType_Cruise_Pill_4_Command_Shot->setEnabled(true);
            }

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 航空炸弹：解锁！确定解锁吗？";
        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise && ui->pushButton_DY54_BombType_Cruise_Command_Lock)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("Cruise Projectile operation: Lock! Are you sure Lock?"), QMessageBox::Yes | QMessageBox::No))
        {

            {
                ui->pushButton_DY54_BombType_Cruise_Pill_Command_Reset->setEnabled(false);
                ui->pushButton_DY54_BombType_Cruise_Pill_1_Command_Shot->setEnabled(false);
                ui->pushButton_DY54_BombType_Cruise_Pill_2_Command_Shot->setEnabled(false);
                ui->pushButton_DY54_BombType_Cruise_Pill_3_Command_Shot->setEnabled(false);
                ui->pushButton_DY54_BombType_Cruise_Pill_4_Command_Shot->setEnabled(false);
            }

            if (m_IUAVNavigationArmShotAlone54CruiseControl)
            {
                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Command_Lock(m_currentComponentID);
            }

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 航空炸弹：加锁！确定加锁吗？";
        }
    }
}
void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise && ui->pushButton_DY54_BombType_Cruise_Pill_Command_Reset)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("Cruise Projectile operation: Reset! Are you sure Reset?"), QMessageBox::Yes | QMessageBox::No))
        {
            if (m_IUAVNavigationArmShotAlone54CruiseControl)
            {
                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_Command_Reset(m_currentComponentID);
            }

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 航空炸弹：复位！确定复位吗？";
        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise && ui->pushButton_DY54_BombType_Cruise_Pill_1_Command_Shot)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.1 Cruise Projectile operation: Shot! Are you sure Shot?"), QMessageBox::Yes | QMessageBox::No))
        {
            if (m_IUAVNavigationArmShotAlone54CruiseControl)
            {
                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_1_Command_Shot(m_currentComponentID);
            }

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 1号航空炸弹：发射！确定发射吗？";
        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise && ui->pushButton_DY54_BombType_Cruise_Pill_2_Command_Shot)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.2 Cruise Projectile operation: Shot! Are you sure Shot?"), QMessageBox::Yes | QMessageBox::No))
        {
            if (m_IUAVNavigationArmShotAlone54CruiseControl)
            {
                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_2_Command_Shot(m_currentComponentID);
            }

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 2号航空炸弹：发射！确定发射吗？";
        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise && ui->pushButton_DY54_BombType_Cruise_Pill_3_Command_Shot)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.3 Cruise Projectile operation: Shot! Are you sure Shot?"), QMessageBox::Yes | QMessageBox::No))
        {
            if (m_IUAVNavigationArmShotAlone54CruiseControl)
            {
                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_3_Command_Shot(m_currentComponentID);
            }

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 3号航空炸弹：发射！确定发射吗？";
        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot()
{
    if (m_DY54_BombType != DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
    {
        LyMessageBox::warning(nullptr, tr("Warning"), tr("Bomb type dose not match the operation command, the command was not send!"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise && ui->pushButton_DY54_BombType_Cruise_Pill_4_Command_Shot)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.4 Cruise Projectile operation: Shot! Are you sure Shot?"), QMessageBox::Yes | QMessageBox::No))
        {
            if (m_IUAVNavigationArmShotAlone54CruiseControl)
            {
                m_IUAVNavigationArmShotAlone54CruiseControl->IUAVNavigationArmShotAlone54CruiseControl_DY54_BombType_Cruise_Pill_4_Command_Shot(m_currentComponentID);
            }

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << u8" 4号航空炸弹：发射！确定发射吗？";
        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_StartAutoAiming()
{
    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("Start auto aim : Are you sure to start auto aim?"), QMessageBox::Yes | QMessageBox::No))
    {

        qnzkna::framework::IRegistry *pIRegistry = IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl();
        if (pIRegistry == nullptr)
        {
            return ;
        }

        bool ok = false;
        long long int componentID = m_currentComponentID.toLongLong(&ok);
        if (!ok)
        {
            return ;
        }

        qnzkna::DY_ZC_Control::DY_ZC_ControlInfo autoAimingInfo;
        autoAimingInfo.componentID     = componentID;
        autoAimingInfo.bombType        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_None;
        autoAimingInfo.oper            = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_AutoAiming;
        autoAimingInfo.operData        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_AutoAiming_Start;

        if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
        {
            autoAimingInfo.bombType        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Rocket;
        }
        else if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
        {
            autoAimingInfo.bombType        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Cruise;
        }

        qnzkna::framework::MessageReferenceProps props;
        props.put("TargetID",   m_currentComponentID.toStdString());

        pIRegistry->messageNotify(new qnzkna::framework::MessageReference("MsgDYZCControlAutoAiming", (const char *)(&autoAimingInfo), sizeof(autoAimingInfo), props));
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_StopAutoAiming()
{
    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("Stop aiming : Confirm stop aiming, note: the drone will switch to increased stability"), QMessageBox::Yes | QMessageBox::No))
    {

        qnzkna::framework::IRegistry *pIRegistry = IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl();
        if (pIRegistry == nullptr)
        {
            return ;
        }

        bool ok = false;
        long long int componentID = m_currentComponentID.toLongLong(&ok);
        if (!ok)
        {
            return ;
        }
        qnzkna::DY_ZC_Control::DY_ZC_ControlInfo autoAimingInfo;
        autoAimingInfo.componentID     = componentID;
        autoAimingInfo.bombType        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_None;
        autoAimingInfo.oper            = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_AutoAiming;
        autoAimingInfo.operData        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_AutoAiming_Stop;

        if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
        {
            autoAimingInfo.bombType        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Rocket;
        }
        else if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
        {
            autoAimingInfo.bombType        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Cruise;
        }

        qnzkna::framework::MessageReferenceProps props;
        props.put("TargetID",   m_currentComponentID.toStdString());

        pIRegistry->messageNotify(new qnzkna::framework::MessageReference("MsgDYZCControlAutoAiming", (const char *)(&autoAimingInfo), sizeof(autoAimingInfo), props));
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_Pill_CheckBox_All_Shot(int state)
{
    m_Pill_CheckBox_All_Shot_Checked = false;
    if (ui->checkBox_DY54_LEFT_All_Shot->checkState() == Qt::CheckState::Checked)
    {
        m_Pill_CheckBox_All_Shot_Checked = true;
    }
}

void IUAVNavigationArmShotAlone54Dialog::IUAVNavigationArmShotAlone_DY54_Pill_Command_All_Shot()
{
    m_Pill_CheckBox_All_Shot_Checked = (ui->checkBox_DY54_LEFT_All_Shot->checkState() == Qt::CheckState::Checked);
    if (m_Pill_CheckBox_All_Shot_Checked)
    {
        if (ui->widget_DY54_BombType_A_1->getSelected() && ui->widget_DY54_BombType_B_1->getSelected())
        {
            if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("No.1 No.2 Projectile operation: Insurance,Cocking! Are you sure Insurance,Cocking?"), QMessageBox::Yes | QMessageBox::No))
            {

                {
                    ResetCommandStatus(XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_1);

                    const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x01, 0x01, 0X7D, 0X7D};
                    SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);
                }

                {
                    ui->ImageStatusWidget_DY54_Pill_1_Command_Percussion_Status->setStatusDark();
                    ui->ImageStatusWidget_DY54_Pill_1_Command_Reset_Status->setStatusDark();

                    const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x01, 0x02, 0X7D, 0X7D};
                    SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);
                }

                {
                    ResetCommandStatus(XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_2);

                    const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x02, 0x01, 0X7D, 0X7D};
                    SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);
                }

                {
                    ui->ImageStatusWidget_DY54_Pill_2_Command_Percussion_Status->setStatusDark();
                    ui->ImageStatusWidget_DY54_Pill_2_Command_Reset_Status->setStatusDark();

                    const unsigned char message[] = {0x7B, 0x7B, 0xA0, 0xB0, 0x02, 0x02, 0X7D, 0X7D};
                    SendXygsDY54KZMsg(message, sizeof(message), m_CurrentSensorID);
                }
            }
        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

bool IUAVNavigationArmShotAlone54Dialog::event(QEvent *event)
{
    if (event->type() == QEvent::Type::ScreenChangeInternal)
    {
        moveVideoHUDDialog();
    }
    return QWidget::event(event);
}

void IUAVNavigationArmShotAlone54Dialog::moveEvent(QMoveEvent *event)
{
    moveVideoHUDDialog();

    QWidget::moveEvent(event);
}

void IUAVNavigationArmShotAlone54Dialog::resizeEvent(QResizeEvent *event)
{
    resizeVideoHUDDialog();

    QWidget::resizeEvent(event);
}

void IUAVNavigationArmShotAlone54Dialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    moveVideoHUDDialog();

#ifdef VIDEOHUDDIALOG_SHOW
    if (m_VideoHUDDialog != nullptr)
    {
        m_VideoHUDDialog->show();
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

#ifdef VIDEOHUDDIALOG_SHOW
    if (m_VideoHUDDialog != nullptr)
    {
        m_VideoHUDDialog->hide();
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    switch (event->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void IUAVNavigationArmShotAlone54Dialog::moveVideoHUDDialog()
{
#ifdef VIDEOHUDDIALOG_SHOW
    if (m_VideoHUDDialog != nullptr)
    {
        if (ui && ui->widget_DY54_Top_Video && m_IPlayVideoManage)
        {
            m_VideoHUDDialog->move(ui->widget_DY54_Top_Video->mapToGlobal(m_IPlayVideoManage->VideoWidgetPos()));
            m_VideoHUDDialog->resize(m_IPlayVideoManage->VideoWidgetSize());
        }
        else if (ui && ui->widget_DY54_Top_Video)
        {
            m_VideoHUDDialog->move(this->mapToGlobal(ui->widget_DY54_Top_Video->pos()));
            m_VideoHUDDialog->resize(ui->widget_DY54_Top_Video->size());
        }
        else
        {
            m_VideoHUDDialog->move(this->mapToGlobal(this->pos()));
            m_VideoHUDDialog->resize(size());
        }
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::resizeVideoHUDDialog()
{
#ifdef VIDEOHUDDIALOG_SHOW
    if (m_VideoHUDDialog != nullptr)
    {
        if (ui && ui->widget_DY54_Top_Video && m_IPlayVideoManage)
        {
            m_VideoHUDDialog->resize(m_IPlayVideoManage->VideoWidgetSize());
        }
        else if (ui && ui->widget_DY54_Top_Video)
        {
            m_VideoHUDDialog->resize(ui->widget_DY54_Top_Video->size());
        }
        else
        {
            m_VideoHUDDialog->resize(size());
        }
    }
#endif
}

qnzkna::PlayVideoManage::IPlayVideoManage *IUAVNavigationArmShotAlone54Dialog::getPlayVideoManageWidget() const
{
    if (ui && ui->widget_DY54_Top_Video && m_IPlayVideoManage)
    {
        return m_IPlayVideoManage;
    }
    return nullptr;
}

void IUAVNavigationArmShotAlone54Dialog::Recv_netheader_uctype_dy38mm_control(char * lpData,int nLength)
{

    if ((nLength < sizeof(NetHeader) + sizeof(qnzkna::ProtocolXygsDY::XygsDY38MM::XygsDY38MMKZStatusMsg)))
    {
        return;
    }

    NetHeader* pNetHeader = (NetHeader*)(lpData);

    if ((pNetHeader->unLength < sizeof(NetHeader) + sizeof(qnzkna::ProtocolXygsDY::XygsDY38MM::XygsDY38MMKZStatusMsg)))
    {
        return;
    }

    if (pNetHeader->ucType != NETHEADER_UCTYPE_DY38MM_CONTROL) {
        return;
    }

    Deal_netheader_uctype_dy38mm_control( (lpData + sizeof(NetHeader)), (pNetHeader->unLength - sizeof(NetHeader)) );
}

void IUAVNavigationArmShotAlone54Dialog::Deal_netheader_uctype_dy38mm_control(const char * const lpData, int nLength)
{
    if ((nLength != sizeof(qnzkna::ProtocolXygsDY::XygsDY38MM::XygsDY38MMKZStatusMsg)))
    {
        return;
    }

    const qnzkna::ProtocolXygsDY::XygsDY38MM::XygsDY38MMKZStatusMsg* const pXygsDY38MMKZStatusMsg = (const qnzkna::ProtocolXygsDY::XygsDY38MM::XygsDY38MMKZStatusMsg* const)(lpData);

    if ((pXygsDY38MMKZStatusMsg->heading != 0x25) ||
        (pXygsDY38MMKZStatusMsg->stacking != 0x25) ||
        (pXygsDY38MMKZStatusMsg->length != 0x04) ||
        (pXygsDY38MMKZStatusMsg->ending != 0x27)
        )
    {
        return;
    }

    const uint8_t verify = VerifyAccumulation(pXygsDY38MMKZStatusMsg);
    if ((pXygsDY38MMKZStatusMsg->verify != verify))
    {
        return;
    }

    m_TimeOutBomb_38MM = ::time(nullptr);


    const uint8_t commandPercussion = pXygsDY38MMKZStatusMsg->controlcommandPercussion;
    const uint8_t commandLock       = pXygsDY38MMKZStatusMsg->controlcommandLock;

   static std::vector<QCheckBox*> AmmoEquip =
    {
        ui->checkBox_38mmlauncher_Control_equip_1,
        ui->checkBox_38mmlauncher_Control_equip_2,
        ui->checkBox_38mmlauncher_Control_equip_3,
        ui->checkBox_38mmlauncher_Control_equip_4,
        ui->checkBox_38mmlauncher_Control_equip_5,
        ui->checkBox_38mmlauncher_Control_equip_6
    };
    //是否挂载
    for (int i = 0; i < AmmoEquip.size(); ++i)
    {
        bool isErase = (commandPercussion >> (5 - i)) & 1;
        if (isErase)
        {
            //AmmoEquip[(5 - i)]->setStyleSheet("background-color: green;");
            //AmmoEquip[i]->setChecked(false);
            //AmmoEquip[i]->setEnabled(true);

            if (commandLock == 0x01)
            {
                AmmoEquip[(5 - i)]->setStyleSheet("background-color: green;");
            }
            else
            {
                AmmoEquip[(5 - i)]->setStyleSheet("background-color: red;");
            }
        }
        else
        {
            AmmoEquip[(5 - i)]->setStyleSheet("background-color: gray;");
            //AmmoEquip[i]->setChecked(false);
            //AmmoEquip[i]->setEnabled(true);
        }

        ////加锁 解锁
        //isErase = (commandLock >> (5 - i)) & 1;
        //if (isErase)
        //    AmmoEquip[(5 - i)]->setStyleSheet("background-color: green;");
        //else
        //    AmmoEquip[(5 - i)]->setStyleSheet("background-color: red;");
    }


}

void IUAVNavigationArmShotAlone54Dialog::Update_XygsDY95_1_SBUS_CommandStatusSyn(const qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_CommandStatusSync* const xygsDY95_1_SBUS_CommandStatusSync)
{
    if (ui == nullptr) {
        return;
    }

    if(xygsDY95_1_SBUS_CommandStatusSync == nullptr){
        return ;
    }

    if(xygsDY95_1_SBUS_CommandStatusSync->seatUIFlag == ((uint64_t)ui)){
        return ;
    }

    const qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command*           const xygsDY95_1_SBUS_Command           = &(xygsDY95_1_SBUS_CommandStatusSync->command);

    if(xygsDY95_1_SBUS_Command->head != XygsDY95_1_SBUS_KZMSG_HEAD){
        return ;
    }

    const auto& position = xygsDY95_1_SBUS_Command->channel_13;

    if (position > 200 && position < 1800)
    {
        if(position == ui->verticalSlider_95_1launcher_Control_fuyang->value()){
            return ;
        }


        disconnect(ui->verticalSlider_95_1launcher_Control_fuyang, &QSlider::valueChanged, this, &IUAVNavigationArmShotAlone54Dialog::on_verticalSlider_95_1launcher_Control_fuyang);

        ui->verticalSlider_95_1launcher_Control_fuyang->setValue(position);
        m_XygsDY95_1_SBUS_Command.channel_13 = position;

        connect(ui->verticalSlider_95_1launcher_Control_fuyang, &QSlider::valueChanged, this, &IUAVNavigationArmShotAlone54Dialog::on_verticalSlider_95_1launcher_Control_fuyang);
    }
}

QString IUAVNavigationArmShotAlone54Dialog::getTitleInfo() const
{
    return m_StrTitle;
}

std::string IUAVNavigationArmShotAlone54Dialog::getCurrentSensorInfo() const
{
    return m_CurrentSensorID;
}

std::string IUAVNavigationArmShotAlone54Dialog::getCurrentComponentInfo() const
{
    return m_currentComponentID.toStdString();
}

const QString &IUAVNavigationArmShotAlone54Dialog::getCurrentComponentID() const
{
    return m_currentComponentID;
}

const QString& IUAVNavigationArmShotAlone54Dialog::getCurrentComponentName() const
{
	return m_currentComponentName;
}

std::string  IUAVNavigationArmShotAlone54Dialog::getSensorType() const
{
    return m_SensorType;
}

IUAVNavigationArmShotAlone54Dialog::DY54_BombType_Enum IUAVNavigationArmShotAlone54Dialog::getBombType() const
{
    return m_DY54_BombType;
}

void IUAVNavigationArmShotAlone54Dialog::SendXygsDY54KZMsg(XygsDY54KZ::XygsDY54KZMsg &xygsDY54KZMsg, const std::string &currentSensorInfoID)
{
    if (m_currentComponentID.isEmpty())
    {
        return ;
    }

    if (currentSensorInfoID.empty())
    {
        return ;
    }

    xygsDY54KZMsg.head1   = XygsDY54KZ::XygsDY54KZMsgHead1;
    xygsDY54KZMsg.head2   = XygsDY54KZ::XygsDY54KZMsgHead2;
    xygsDY54KZMsg.head3   = XygsDY54KZ::XygsDY54KZMsgHead3;
    xygsDY54KZMsg.head4   = XygsDY54KZ::XygsDY54KZMsgHead4;
    xygsDY54KZMsg.end1    = XygsDY54KZ::XygsDY54KZMsgEnd1;
    xygsDY54KZMsg.end2    = XygsDY54KZ::XygsDY54KZMsgEnd2;

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << QString::fromStdString(currentSensorInfoID) << (uint8_t)(xygsDY54KZMsg.pill) << (uint8_t)(xygsDY54KZMsg.cmd);

    qnzkna::SystemMaintenance::ISystemMaintenanceService *pSystemMaintenanceService = IUAVNavigationArmShotAlone54Activator::Get().getPISystemMaintenanceService();
    if (pSystemMaintenanceService == nullptr)
    {
        return ;
    }

    qnzkna::framework::IRegistry *pIRegistry = IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl();
    if (pIRegistry == nullptr)
    {
        return ;
    }

    qnzkna::NodeManage::NODE_TYPE_UINT   nLocalType;
    std::string                          strLocalID;
    const qnzkna::NodeManage::NODE_TYPE_UINT nTargetType = NODE_TYPE_CARTRIDGE;
    pSystemMaintenanceService->GetNodeInfo(&nLocalType, &strLocalID);

    qnzkna::framework::MessageReferenceProps props;
    props.put("SourceType", std::to_string(nLocalType));
    props.put("SourceID",   strLocalID);
    props.put("TargetType", std::to_string(nTargetType));
    props.put("TargetID",   currentSensorInfoID);

    pIRegistry->messageNotify(new qnzkna::framework::MessageReference("MsgNavigationArmShotSend54", (const char *const)(&xygsDY54KZMsg), sizeof(xygsDY54KZMsg), props));
}

void IUAVNavigationArmShotAlone54Dialog::SendXygsDY54KZMsg(const unsigned char *const message, const unsigned int msglength, const std::string &currentSensorInfoID)
{
    if (m_currentComponentID.isEmpty())
    {
        return ;
    }

    if (currentSensorInfoID.empty())
    {
        return ;
    }

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << QString::fromStdString(currentSensorInfoID) << message[4] << message[5];

    qnzkna::SystemMaintenance::ISystemMaintenanceService *pSystemMaintenanceService = IUAVNavigationArmShotAlone54Activator::Get().getPISystemMaintenanceService();
    if (pSystemMaintenanceService == nullptr)
    {
        return ;
    }

    qnzkna::framework::IRegistry *pIRegistry = IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl();
    if (pIRegistry == nullptr)
    {
        return ;
    }

    qnzkna::NodeManage::NODE_TYPE_UINT nLocalType;
    std::string                          strLocalID;
    const qnzkna::NodeManage::NODE_TYPE_UINT nTargetType = NODE_TYPE_CARTRIDGE;
    pSystemMaintenanceService->GetNodeInfo(&nLocalType, &strLocalID);

    qnzkna::framework::MessageReferenceProps props;
    props.put("SourceType", std::to_string(nLocalType));
    props.put("SourceID",   strLocalID);
    props.put("TargetType", std::to_string(nTargetType));
    props.put("TargetID",   currentSensorInfoID);

    pIRegistry->messageNotify(new qnzkna::framework::MessageReference("MsgNavigationArmShotSend54", (const char *const)(message), msglength, props));
}

void IUAVNavigationArmShotAlone54Dialog::on_Message_Recv(const std::string &strSensorId, const QByteArray &message)
{
    if (message.size() <= 0)
    {
        return ;
    }

    if (m_currentComponentID.isEmpty())
    {
        return ;
    }

    if (strSensorId.empty())
    {
        return ;
    }

    if (m_CurrentSensorID != strSensorId)
    {
        return ;
    }

    emit ShowXygsDY54KZMsgStatusInfoSig(QString::fromStdString(strSensorId), message);
}

void IUAVNavigationArmShotAlone54Dialog::on_DY_ZC_HUD_ControlInfo(const std::string &strWeaponId, const QByteArray &message)
{
    if (message.size() <= 0)
    {
        return ;
    }

    if (m_CurrentSensorID.empty())
    {
        return ;
    }

    if (strWeaponId.empty())
    {
        return ;
    }

    if (m_currentComponentID != QString::fromStdString(strWeaponId))
    {
        return ;
    }

    emit ShowDY_ZC_HUD_ControlInfoSig(QString::fromStdString(strWeaponId), message);
}

void IUAVNavigationArmShotAlone54Dialog::bomBCallBackFunction(BombStatusWidget *bombStatusWidget, IUAVNavigationArmShotAlone54Dialog *dialog)
{
    if (bombStatusWidget == nullptr || dialog == nullptr)
    {
        return;
    }

    IUAVNavigationArmShotAlone54Dialog *navigationArmShotDialog = dialog;
    if (navigationArmShotDialog == nullptr || navigationArmShotDialog->ui == nullptr)
    {
        return;
    }

    BombStatusWidget *bombStatus = bombStatusWidget;
    if (bombStatus == nullptr)
    {
        return;
    }
    navigationArmShotDialog->m_singleSelectedBomb = true;
}

bool IUAVNavigationArmShotAlone54Dialog::InitBombState(bool resetBomb)
{
    if (ui == nullptr)
    {
        return false;
    }

    time_t timeNow = ::time(nullptr);
    if (((m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket) || (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise)) && (timeNow - m_TimeOutBomb >=( TIMER_FIRE_CONTROL_REFRESH / TIMER_FIRE_CONTROL_REFRESH * 2)) || resetBomb)
    {
        m_TimeOutBomb = ::time(nullptr);

        ui->widget_DY54_BombType_A_1->setUnReign();
        ui->widget_DY54_BombType_B_1->setUnReign();

        ResetCommandStatus(XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_1);

        ui->ImageStatusWidget_DY54_Pill_1_Command_Percussion_Status->setStatusDark();
        ui->ImageStatusWidget_DY54_Pill_1_Command_Reset_Status->setStatusDark();

        ResetCommandStatus(XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_2);

        ui->ImageStatusWidget_DY54_Pill_2_Command_Percussion_Status->setStatusDark();
        ui->ImageStatusWidget_DY54_Pill_2_Command_Reset_Status->setStatusDark();

    }

    if ((m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM) && (timeNow - m_TimeOutBomb_38MM >= ( 5 )) )
    {
        m_TimeOutBomb_38MM = ::time(nullptr);

        if (ui->checkBox_38mmlauncher_Control_equip_1 != nullptr) {
            ui->checkBox_38mmlauncher_Control_equip_1->setStyleSheet("background-color: #081829;");
            ui->checkBox_38mmlauncher_Control_equip_2->setStyleSheet("background-color: #081829;");
            ui->checkBox_38mmlauncher_Control_equip_3->setStyleSheet("background-color: #081829;");
            ui->checkBox_38mmlauncher_Control_equip_4->setStyleSheet("background-color: #081829;");
            ui->checkBox_38mmlauncher_Control_equip_5->setStyleSheet("background-color: #081829;");
            ui->checkBox_38mmlauncher_Control_equip_6->setStyleSheet("background-color: #081829;");
        }
    }


    if ((timeNow - m_TimeOut_DY_ZC_HUD_ControlInfo >= (10) ))
    {
        m_TimeOut_DY_ZC_HUD_ControlInfo = ::time(nullptr);
        ui->label_IUAVNavigationArmShotAlone54Dialog_DY_ZC_HUD_ControlInfo->setText("");
    }

    return true;
}

void IUAVNavigationArmShotAlone54Dialog::ResetCommandStatus(const XygsDY54KZ::XygsDY54KZMsg_Pill pill)
{
    if (XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_1 == pill)
    {
        for (auto ImageStatus : m_DY54_ImageStatusList_Pill_1)
        {
            ImageStatus->setStatusDark();
        }
    }
    else if (XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_2 == pill)
    {
        for (auto ImageStatus : m_DY54_ImageStatusList_Pill_2)
        {
            ImageStatus->setStatusDark();
        }
    }
}

void IUAVNavigationArmShotAlone54Dialog::StopXygsDY54KZChange(int millisecondsTime)
{
    Q_UNUSED(millisecondsTime)
}

void IUAVNavigationArmShotAlone54Dialog::InitTargetShotControl()
{
    ui->radioButton_Target_HaveFromTable->setChecked(true);
    ui->radioButton_Target_New->setChecked(false);

    m_TargetList.clear();
    QString str = QString("SELECT MBBH,MBMC FROM ZZMB_XTMBXX");
    QSqlQuery query;
    query.prepare(str);
    bool bGet = query.exec();
    if (bGet)
    {
        while (query.next())
        {
            QString strMBBH = tr2(query.value(0).toString().toLocal8Bit().data());
            QString strMBMC = tr2(query.value(1).toString().toLocal8Bit().data());
            m_TargetList.push_back(std::make_tuple(strMBBH.toStdString(), strMBMC.toStdString()));

            ui->comboBox_radioButton_Target_HaveFromTable->addItem(strMBMC);
            int nCount = ui->comboBox_radioButton_Target_HaveFromTable->count();
            ui->comboBox_radioButton_Target_HaveFromTable->setItemData(nCount - 1, strMBBH);

            ui->comboBox_radioButton_Target_HaveFromTable->setCurrentIndex(0);
        }
    }

}

uint8_t IUAVNavigationArmShotAlone54Dialog::GetCurrentHitEquip()
{
    std::vector<QCheckBox*> AmmoEquip =
    {
        ui->checkBox_38mmlauncher_Control_equip_1,
        ui->checkBox_38mmlauncher_Control_equip_2,
        ui->checkBox_38mmlauncher_Control_equip_3,
        ui->checkBox_38mmlauncher_Control_equip_4,
        ui->checkBox_38mmlauncher_Control_equip_5,
        ui->checkBox_38mmlauncher_Control_equip_6
    };

    uint8_t controlcommand = 0x00;
    for (int i = 0; i < AmmoEquip.size(); ++i)
    {
        bool iscurr = AmmoEquip[i]->isChecked();
        if (iscurr) {
            controlcommand = (0x01 << i) | controlcommand;
        }
    }
    return controlcommand;
}

void IUAVNavigationArmShotAlone54Dialog::ShowXygsDY54KZMsgStatusInfo(const QString strQSensorId, const QByteArray message)
{
    if (ui == nullptr)
    {
        return ;
    }

    if (message.size() <= 0)
    {
        return ;
    }

    if (m_currentComponentID.isEmpty())
    {
        return ;
    }

    const std::string &strSensorId = strQSensorId.toStdString();

    if (m_CurrentSensorID != strSensorId)
    {
        return ;
    }

    if (message.count() != sizeof(XygsDY54KZ::XygsDY54KZStatusMsg) && message.count() != sizeof(XygsDY54KZ::XygsDY54KZStatusFeedbackMsg))
    {
        return ;
    }

    m_TimeOutBomb = ::time(nullptr);

    if (message.count() == sizeof(XygsDY54KZ::XygsDY54KZStatusMsg))
    {
        const auto XygsDY54KZMsg_CommandStatus_Fun = [](const XygsDY54KZ::XygsDY54KZMsg_CommandStatus & status_on, ImageStatusWidget * const imageStatusWidget)
        {
            if (imageStatusWidget == nullptr)
            {
                return ;
            }
            switch (status_on)
            {
            case XygsDY54KZ::XygsDY54KZMsg_CommandStatus::XygsDY54KZMsg_CommandStatus_Fail:
            {
                imageStatusWidget->setStatusRed();
            }
            break;
            case XygsDY54KZ::XygsDY54KZMsg_CommandStatus::XygsDY54KZMsg_CommandStatus_Success_01:
            {
                imageStatusWidget->setStatusLight();
            }
            break;
            case XygsDY54KZ::XygsDY54KZMsg_CommandStatus::XygsDY54KZMsg_CommandStatus_Success_02:
            {
                imageStatusWidget->setStatusLight();
            }
            break;
            default:
                break;
            }
        };

        const XygsDY54KZ::XygsDY54KZStatusMsg *const xygsDY54KZStatusMsg = ((const XygsDY54KZ::XygsDY54KZStatusMsg * const)(message.constData()));

        const XygsDY54KZ::XygsDY54KZMsg_Pill &pill_on = xygsDY54KZStatusMsg->pill;
        switch (pill_on)
        {
        case XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_1:
        {
            const XygsDY54KZ::XygsDY54KZMsg_Command &switch_on = xygsDY54KZStatusMsg->cmd;
            switch (switch_on)
            {
            case XygsDY54KZ::XygsDY54KZMsg_Command::XygsDY54KZMsg_Command_Insure:
            {
                XygsDY54KZMsg_CommandStatus_Fun(xygsDY54KZStatusMsg->status, ui->ImageStatusWidget_DY54_Pill_1_Command_Insure_Status);
            }
            break;
            case XygsDY54KZ::XygsDY54KZMsg_Command::XygsDY54KZMsg_Command_Percussion:
            {
                XygsDY54KZMsg_CommandStatus_Fun(xygsDY54KZStatusMsg->status, ui->ImageStatusWidget_DY54_Pill_1_Command_Percussion_Status);
            }
            break;
            case XygsDY54KZ::XygsDY54KZMsg_Command::XygsDY54KZMsg_Command_Reset:
            {
                XygsDY54KZMsg_CommandStatus_Fun(xygsDY54KZStatusMsg->status, ui->ImageStatusWidget_DY54_Pill_1_Command_Reset_Status);
            }
            break;
            default:
                break;
            }
        }
        break;
        case XygsDY54KZ::XygsDY54KZMsg_Pill::XygsDY54KZMsg_Pill_2:
        {
            const XygsDY54KZ::XygsDY54KZMsg_Command &switch_on = xygsDY54KZStatusMsg->cmd;
            switch (switch_on)
            {
            case XygsDY54KZ::XygsDY54KZMsg_Command::XygsDY54KZMsg_Command_Insure:
            {
                XygsDY54KZMsg_CommandStatus_Fun(xygsDY54KZStatusMsg->status, ui->ImageStatusWidget_DY54_Pill_2_Command_Insure_Status);
            }
            break;
            case XygsDY54KZ::XygsDY54KZMsg_Command::XygsDY54KZMsg_Command_Percussion:
            {
                XygsDY54KZMsg_CommandStatus_Fun(xygsDY54KZStatusMsg->status, ui->ImageStatusWidget_DY54_Pill_2_Command_Percussion_Status);
            }
            break;
            case XygsDY54KZ::XygsDY54KZMsg_Command::XygsDY54KZMsg_Command_Reset:
            {
                XygsDY54KZMsg_CommandStatus_Fun(xygsDY54KZStatusMsg->status, ui->ImageStatusWidget_DY54_Pill_2_Command_Reset_Status);
            }
            break;
            default:
                break;
            }
        }
        break;
        default:
            break;
        }
    }
    else if (message.count() == sizeof(XygsDY54KZ::XygsDY54KZStatusFeedbackMsg))
    {
        const auto XygsDY54KZMsg_CommandStatusFeedback_Fun = [](const XygsDY54KZ::XygsDY54KZMsg_CommandStatusFeedback & statusFeedback, ImageStatusWidget * const imageStatusWidget)
        {
            if (imageStatusWidget == nullptr)
            {
                return ;
            }
            switch (statusFeedback)
            {
            case XygsDY54KZ::XygsDY54KZMsg_CommandStatusFeedback::XygsDY54KZMsg_CommandStatusFeedback_Close:
            {
                imageStatusWidget->setStatusDark();
            }
            break;
            case XygsDY54KZ::XygsDY54KZMsg_CommandStatusFeedback::XygsDY54KZMsg_CommandStatusFeedback_Open:
            {
                imageStatusWidget->setStatusLight();
            }
            break;
            case XygsDY54KZ::XygsDY54KZMsg_CommandStatusFeedback::XygsDY54KZMsg_CommandStatusFeedback_CloseOpen:
            {
                imageStatusWidget->setStatusRed();
            }
            break;
            default:
                break;
            }
        };

        const XygsDY54KZ::XygsDY54KZStatusFeedbackMsg *const xygsDY54KZStatusFeedbackMsg = ((const XygsDY54KZ::XygsDY54KZStatusFeedbackMsg * const)(message.constData()));

        if ((xygsDY54KZStatusFeedbackMsg->status != XygsDY54KZ::XygsDY54KZMsg_StatusFeedback::XygsDY54KZMsg_StatusFeedback_1))
        {
            return ;
        }

        XygsDY54KZMsg_CommandStatusFeedback_Fun(xygsDY54KZStatusFeedbackMsg->statusInsure_A,     ui->ImageStatusWidget_DY54_Pill_1_Command_Insure_Status);
        XygsDY54KZMsg_CommandStatusFeedback_Fun(xygsDY54KZStatusFeedbackMsg->statusPercussion_A, ui->ImageStatusWidget_DY54_Pill_1_Command_Percussion_Status);
        XygsDY54KZMsg_CommandStatusFeedback_Fun(xygsDY54KZStatusFeedbackMsg->statusInsure_B,     ui->ImageStatusWidget_DY54_Pill_2_Command_Insure_Status);
        XygsDY54KZMsg_CommandStatusFeedback_Fun(xygsDY54KZStatusFeedbackMsg->statusPercussion_B, ui->ImageStatusWidget_DY54_Pill_2_Command_Percussion_Status);

        if (ui && ui->widget_DY54_BombType_A_1 && ui->widget_DY54_BombType_B_1)
        {
            if (xygsDY54KZStatusFeedbackMsg->statusPercussion_A == XygsDY54KZ::XygsDY54KZMsg_CommandStatusFeedback::XygsDY54KZMsg_CommandStatusFeedback_Open)
            {
                ui->widget_DY54_BombType_A_1->setUnReign();
            }
            else
            {
                ui->widget_DY54_BombType_A_1->setReign();
            }

            if (xygsDY54KZStatusFeedbackMsg->statusPercussion_B == XygsDY54KZ::XygsDY54KZMsg_CommandStatusFeedback::XygsDY54KZMsg_CommandStatusFeedback_Open)
            {
                ui->widget_DY54_BombType_B_1->setUnReign();
            }
            else
            {
                ui->widget_DY54_BombType_B_1->setReign();
            }
        }
    }

}

void IUAVNavigationArmShotAlone54Dialog::ShowDY_ZC_HUD_ControlInfo(const QString strQWeaponId, const QByteArray message)
{
    if (ui == nullptr)
    {
        return ;
    }

    if (message.size() <= 0)
    {
        return ;
    }

    if (m_currentComponentID != strQWeaponId)
    {
        return ;
    }

    if (message.count() != sizeof(qnzkna::DY_ZC_HUD_Control::DY_ZC_HUD_ControlInfo))
    {
        return ;
    }

    const qnzkna::DY_ZC_HUD_Control::DY_ZC_HUD_ControlInfo *const controlInfo = ((const qnzkna::DY_ZC_HUD_Control::DY_ZC_HUD_ControlInfo * const)(message.constData()));

#ifdef VIDEOHUDDIALOG_SHOW
    if (m_VideoHUDDialog != nullptr)
    {
        m_VideoHUDDialog->updateDY_ZC_HUD_ControlInfo(*controlInfo);
    }
#else
    m_TimeOut_DY_ZC_HUD_ControlInfo = ::time(nullptr);
    if ((controlInfo->autoAimingState == 1) || (controlInfo->nVehicleTargetDistShow == 1 && controlInfo->nVehicleTargetCal == 1))
    {
        if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
        {
            QString strT = tr2("瞄准状态：");
            if (controlInfo->autoAimingState == 0)
            {
                strT += tr2("初始停止");
            }
            else if (controlInfo->autoAimingState == 1)
            {
                strT += tr2("执行中");
            }
            else if (controlInfo->autoAimingState == 2)
            {
                strT += tr2("停止");
            }
            else
            {
                strT += tr2("未知");
            }

            strT += tr2("， 近落点距目标： 距离：")   + QString::number(controlInfo->dTargetVehicleMinDist3D, 'f', 2);
            strT += tr2(" ,高度：") + QString::number(controlInfo->dTargetVehicleMinHeightDiff, 'f', 2);
            strT += tr2(" ,角度：") + QString::number(controlInfo->dTargetVehicleMinYawDiff, 'f', 2);
            ui->label_IUAVNavigationArmShotAlone54Dialog_DY_ZC_HUD_ControlInfo->setText(strT);
        }
        else if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
        {
            QString strT = tr2("瞄准状态：");
            if (controlInfo->autoAimingState == 0)
            {
                strT += tr2("初始停止");
            }
            else if (controlInfo->autoAimingState == 1)
            {
                strT += tr2("执行中");
            }
            else if (controlInfo->autoAimingState == 2)
            {
                strT += tr2("停止");
            }
            else
            {
                strT += tr2("未知");
            }

            strT += tr2("， 距目标： 距离：")   + QString::number(controlInfo->dTargetVehicleMinDist3D, 'f', 2);
            strT += tr2(" ,高度：") + QString::number(controlInfo->dTargetVehicleMinHeightDiff, 'f', 2);
            strT += tr2(" ,角度：") + QString::number(controlInfo->dTargetVehicleMinYawDiff, 'f', 2);
            ui->label_IUAVNavigationArmShotAlone54Dialog_DY_ZC_HUD_ControlInfo->setText(strT);
        }
        else
        {
            QString strT = tr2("错误警告：配置和弹药不匹配，需要配置和弹药匹配正确！");
            ui->label_IUAVNavigationArmShotAlone54Dialog_DY_ZC_HUD_ControlInfo->setText(strT);
        }
    }
    else
    {
        ui->label_IUAVNavigationArmShotAlone54Dialog_DY_ZC_HUD_ControlInfo->setText("");
    }
#endif

    m_TimeOut_DY_ZC_HUD_ControlInfo = ::time(nullptr);
    if ((controlInfo->autoAimingState == 1) || (controlInfo->nVehicleMarkDistShow == 1 && controlInfo->nVehicleMarkCal == 1))
    {
        if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Rocket)
        {
            QString strT = tr("Aiming State: ");
            if (controlInfo->autoAimingState == 0)
            {
                strT += tr("Initial Stop");
            }
            else if (controlInfo->autoAimingState == 1)
            {
                strT += tr("in Execution");
            }
            else if (controlInfo->autoAimingState == 2)
            {
                strT += tr("Stop");
            }
            else
            {
                strT += tr("Unknown");
            }

            strT += tr(", near point distance from target: Distance: ") + QString::number(controlInfo->dMarkVehicleMinDist3D, 'f', 2);
            strT += tr(", altitude: ") + QString::number(controlInfo->dMarkVehicleMinHeightDiff, 'f', 2);
            strT += tr(", angle: ") + QString::number(controlInfo->dMarkVehicleMinYawDiff, 'f', 2);
            ui->label_IUAVNavigationArmShotAlone54Dialog_DY_ZC_HUD_ControlInfo->setText(strT);
        }
        else if (m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_Cruise)
        {
            QString strT = tr("Aiming State: ");
            if (controlInfo->autoAimingState == 0)
            {
                strT += tr("Initial Stop");
            }
            else if (controlInfo->autoAimingState == 1)
            {
                strT += tr("in Execution");
            }
            else if (controlInfo->autoAimingState == 2)
            {
                strT += tr("Stop");
            }
            else
            {
                strT += tr("Unknown");
            }

            strT += tr(", near point distance from target: Distance: ") + QString::number(controlInfo->dMarkVehicleMinDist3D, 'f', 2);
            strT += tr(", altitude: ") + QString::number(controlInfo->dMarkVehicleMinHeightDiff, 'f', 2);
            strT += tr(", angle: ") + QString::number(controlInfo->dMarkVehicleMinYawDiff, 'f', 2);
            ui->label_IUAVNavigationArmShotAlone54Dialog_DY_ZC_HUD_ControlInfo->setText(strT);
        }
        else
        {
            QString strT = tr("Error warning: Configuration and ammo don't match, need configuration and ammo match correctly!");
            ui->label_IUAVNavigationArmShotAlone54Dialog_DY_ZC_HUD_ControlInfo->setText(strT);
        }
    }
    else
    {
        ui->label_IUAVNavigationArmShotAlone54Dialog_DY_ZC_HUD_ControlInfo->setText("");
    }
}

void UAVNavigationArmShotAlone54BoxLayout::setGeometry(const QRect &rect)
{
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
    if (m_pIUAVNavigationArmShotAlone54Dialog && m_pIUAVNavigationArmShotAlone54Dialog->getPlayVideoManageWidget())
    {
        m_pIUAVNavigationArmShotAlone54Dialog->getPlayVideoManageWidget()->Lock();
        QVBoxLayout::setGeometry(rect);
        m_pIUAVNavigationArmShotAlone54Dialog->getPlayVideoManageWidget()->UnLock();
    }
    else
    {
        QVBoxLayout::setGeometry(rect);
    }
#else
    QVBoxLayout::setGeometry(rect);
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_Target_ScreenShot_clicked()
{
    QMutexLocker locker(&m_sMutex);

    QScreen *pScreen = QGuiApplication::primaryScreen();
    if (!pScreen)
    {
        return;
    }
    QPixmap p = pScreen->grabWindow(ui->widget_DY54_Top_Video->winId());
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss-zzz") + ".png";
    if (!p.save(fileName, "PNG"))
    {
    }

    QByteArray inByteArray;
    QBuffer inBuffer(&inByteArray);
    inBuffer.open(QIODevice::WriteOnly);
    p.save(&inBuffer, "PNG");
    if (inByteArray.size() > 16 * 1024 * 1024)
    {
        qDebug() << "Error Image File Size is To Big.\n";
        return;
    }
    QString strID;
    bool bGetTarget = getCurrentTarget(strID);
    QString strFileName = QString(QDateTime::currentDateTime().toString("yyyy_MM_dd-hh:mm:ss.zzz"));
    QSqlQuery query;
    QString strSql = QString("INSERT INTO `ZKLY`.`XDMB_ZP` (ZPMC,ZPNR,MBBH) VALUES ('%1', :imageData,'%2')").arg(strFileName).arg(strID);
    query.prepare(strSql);
    query.bindValue(":imageData", inByteArray);
    if (!query.exec())
    {
        qDebug() << "Error inserting MB image into table.\n";
    }
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_Target_Record_clicked()
{

}

void IUAVNavigationArmShotAlone54Dialog::on_radioButton_Target_HaveFromTable_clicked()
{
    bool bChecked = ui->radioButton_Target_HaveFromTable->isChecked();
    if (bChecked)
    {
        return;
    }

    ui->radioButton_Target_HaveFromTable->setChecked(true);
    ui->radioButton_Target_New->setChecked(false);
}

void IUAVNavigationArmShotAlone54Dialog::on_radioButton_Target_New_clicked()
{
    ui->radioButton_Target_HaveFromTable->setChecked(false);
    ui->radioButton_Target_New->setChecked(true);
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Video_Light()
{
    bool ok = false;
    const unsigned long long unNodeID = m_videoControlSensorInfoID.toULongLong(&ok);
    if (!ok)
    {
        return ;
    }

    unsigned char pbuf[sizeof(unsigned long long) + 1]       = { 0 };
    (*(unsigned long long*)(pbuf)) = unNodeID;
    pbuf[sizeof(pbuf) - 1]         = 0x01;
    const int pbufSize             = sizeof(pbuf);


    char sendBuf[1024]       = { 0 };
    memset(sendBuf, 0, sizeof(sendBuf));
    NetHeader* netHeaderPtr  = (NetHeader*)(sendBuf);

    netHeaderPtr->ucType     = NETHEADER_UCTYPE_VIDEO_CMD;
    netHeaderPtr->ucSubType  = NETHEADER_UCSUBTYPE_Q20A_CMD_UP_TO_SERVER;
    netHeaderPtr->unNodeID   = unNodeID ;
    netHeaderPtr->unNodeType = 3;

    netHeaderPtr->unSeatType = NETHEADER_UNSEATTYPE_CLIENT;
    netHeaderPtr->unLength   = sizeof(NetHeader) + pbufSize;
    memcpy(sendBuf + sizeof(NetHeader), &pbuf, pbufSize);

    LY_UAVNavigationArmShotAlone54Interface::Get().SendData(sendBuf, netHeaderPtr->unLength);
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Video_Infrared()
{
    bool ok = false;
    const unsigned long long unNodeID = m_videoControlSensorInfoID.toULongLong(&ok);
    if (!ok)
    {
        return ;
    }

    unsigned char pbuf[sizeof(unsigned long long) + 1]       = { 0 };
    (*(unsigned long long*)(pbuf)) = unNodeID;
    pbuf[sizeof(pbuf) - 1]         = 0x02;
    const int pbufSize             = sizeof(pbuf);


    char sendBuf[1024]       = { 0 };
    memset(sendBuf, 0, sizeof(sendBuf));
    NetHeader* netHeaderPtr  = (NetHeader*)(sendBuf);

    netHeaderPtr->ucType     = NETHEADER_UCTYPE_VIDEO_CMD;
    netHeaderPtr->ucSubType  = NETHEADER_UCSUBTYPE_Q20A_CMD_UP_TO_SERVER;
    netHeaderPtr->unNodeID   = unNodeID ;
    netHeaderPtr->unNodeType = 3;

    netHeaderPtr->unSeatType = NETHEADER_UNSEATTYPE_CLIENT;
    netHeaderPtr->unLength   = sizeof(NetHeader) + pbufSize;
    memcpy(sendBuf + sizeof(NetHeader), &pbuf, pbufSize);

    LY_UAVNavigationArmShotAlone54Interface::Get().SendData(sendBuf, netHeaderPtr->unLength);
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Video_Close()
{
    bool ok = false;
    const unsigned long long unNodeID = m_videoControlSensorInfoID.toULongLong(&ok);
    if (!ok)
    {
        return ;
    }

    unsigned char pbuf[sizeof(unsigned long long) + 1]       = { 0 };
    (*(unsigned long long*)(pbuf)) = unNodeID;
    pbuf[sizeof(pbuf) - 1]         = 0x00;
    const int pbufSize             = sizeof(pbuf);


    char sendBuf[1024]       = { 0 };
    memset(sendBuf, 0, sizeof(sendBuf));
    NetHeader* netHeaderPtr  = (NetHeader*)(sendBuf);

    netHeaderPtr->ucType     = NETHEADER_UCTYPE_VIDEO_CMD;
    netHeaderPtr->ucSubType  = NETHEADER_UCSUBTYPE_Q20A_CMD_UP_TO_SERVER;
    netHeaderPtr->unNodeID   = unNodeID ;
    netHeaderPtr->unNodeType = 3;

    netHeaderPtr->unSeatType = NETHEADER_UNSEATTYPE_CLIENT;
    netHeaderPtr->unLength   = sizeof(NetHeader) + pbufSize;
    memcpy(sendBuf + sizeof(NetHeader), &pbuf, pbufSize);

    LY_UAVNavigationArmShotAlone54Interface::Get().SendData(sendBuf, netHeaderPtr->unLength);
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_fuyang_up()
{
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl38mm" && m_SensorType != "FireControl38mmPWM")
    {
        LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("配置弹药类型不是38mm弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl38mm" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM && ui->pushButton_38mmlauncher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_38MM_Pitch_ServoPWM;    // 38mm弹药挂架 俯仰
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_38MM_Pitch_ServoPWM;// 38mm弹药挂架 俯仰

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.pitchServoPWM_38MM.pitchServoPWMNumber_38MM = 13;
        data.pitchServoPWM_38MM.pitchServoPWMvalue_38MM  = 2100;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"38mm弹药挂架 俯仰 " << data.pitchServoPWM_38MM.pitchServoPWMvalue_38MM;
    }
    else if (m_SensorType == "FireControl38mmPWM" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM_PWM && ui->pushButton_38mmlauncher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_38MM_ServoPWM;    // 38mm弹药挂架 俯仰
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_38MM_ServoPWM_Pitch;// 38mm弹药挂架

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.servoPWM_38MM.servoPWMNumber_38MM = 12;
        data.servoPWM_38MM.servoPWMvalue_38MM  = 1200;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"38mm PWM 弹药挂架 俯仰 " << data.servoPWM_38MM.servoPWMvalue_38MM;
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_puyang_down()
{
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl38mm" && m_SensorType != "FireControl38mmPWM")
    {
        LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("配置弹药类型不是38mm弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl38mm" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM && ui->pushButton_38mmlauncher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_38MM_Pitch_ServoPWM;    // 38mm弹药挂架 俯仰
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_38MM_Pitch_ServoPWM;// 38mm弹药挂架 俯仰

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.pitchServoPWM_38MM.pitchServoPWMNumber_38MM = 13;
        data.pitchServoPWM_38MM.pitchServoPWMvalue_38MM  = 800;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"38mm弹药挂架 俯仰 " << data.pitchServoPWM_38MM.pitchServoPWMvalue_38MM;
    }
    else if (m_SensorType == "FireControl38mmPWM" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM_PWM && ui->pushButton_38mmlauncher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_38MM_ServoPWM;    // 38mm弹药挂架 俯仰
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_38MM_ServoPWM_Pitch;// 38mm弹药挂架

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.servoPWM_38MM.servoPWMNumber_38MM = 12;
        data.servoPWM_38MM.servoPWMvalue_38MM  = 1800;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"38mm PWM 弹药挂架 俯仰 " << data.servoPWM_38MM.servoPWMvalue_38MM;
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_puyang_stop()
{
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl38mm" && m_SensorType != "FireControl38mmPWM")
    {
        LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("配置弹药类型不是38mm弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl38mm" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM && ui->pushButton_38mmlauncher_Control_lock)
    {
        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"38mm弹药挂架 俯仰 ";
    }
    else if (m_SensorType == "FireControl38mmPWM" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM_PWM && ui->pushButton_38mmlauncher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_38MM_ServoPWM;    // 38mm弹药挂架 俯仰
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_38MM_ServoPWM_Pitch;// 38mm弹药挂架

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.servoPWM_38MM.servoPWMNumber_38MM = 12;
        data.servoPWM_38MM.servoPWMvalue_38MM  = 1500;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"38mm PWM 弹药挂架 俯仰 " << data.servoPWM_38MM.servoPWMvalue_38MM;
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_lock()
{
    // 加锁
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl38mm" && m_SensorType != "FireControl38mmPWM")
    {
        LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("配置弹药类型不是38mm弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

    if (m_SensorType == "FireControl38mm" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM && ui->pushButton_38mmlauncher_Control_lock)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("是否加锁38mm所有弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            uint8_t currentEquip = GetCurrentHitEquip();

            qnzkna::ProtocolXygsDY::XygsDY38MM::FireControl_XygsDY38MM_Command command;
            /*memset(&command, 0, sizeof(command));*/
            command.funcstacking   = qnzkna::ProtocolXygsDY::XygsDY38MM::XygsDY38MMKZMsg_Command_Stacking::XygsDY54KZMsg_Command_Insure;
            command.controlcommand =  0x01;
            command.randomstack[0] = 0x19;
            command.randomstack[1] = 0x1a;
            command.verify         = VerifyAccumulation(&command);
            sendMessage_38mm_Launcher_Control((const char * const)(&command), sizeof(command));

            static std::vector<QCheckBox*> AmmoEquip =
            {
                ui->checkBox_38mmlauncher_Control_equip_1,
                ui->checkBox_38mmlauncher_Control_equip_2,
                ui->checkBox_38mmlauncher_Control_equip_3,
                ui->checkBox_38mmlauncher_Control_equip_4,
                ui->checkBox_38mmlauncher_Control_equip_5,
                ui->checkBox_38mmlauncher_Control_equip_6
            };

            for (int i = 0; i < AmmoEquip.size(); ++i)
            {
                    AmmoEquip[i]->setChecked(false);
                    AmmoEquip[i]->setEnabled(false);
            }

            ui->pushButton_38mmlauncher_Control_Percussion->setEnabled(false);

        }
    }
#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    else if (m_SensorType == "FireControl38mmPWM" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM_PWM && ui->pushButton_38mmlauncher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        ui->pushButton_38mmlauncher_Control_Percussion->setEnabled(false);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"38mm PWM 弹药挂架 加锁 ";
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_unlock()
{
    // 解锁
    if (ui == nullptr){
        return;
    }

    if (m_SensorType != "FireControl38mm" && m_SensorType != "FireControl38mmPWM")
    {
        LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("配置弹药类型不是38mm弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

    if (m_SensorType == "FireControl38mm" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM && ui->pushButton_38mmlauncher_Control_unlock)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("是否解锁38mm弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            uint8_t currentEquip = GetCurrentHitEquip();
            //if (currentEquip == 0x00) {
            //    LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("请选择解锁38mm的弹药?"), QMessageBox::Yes | QMessageBox::No);
            //    return;
            //}
            qnzkna::ProtocolXygsDY::XygsDY38MM::FireControl_XygsDY38MM_Command command;
            /*memset(&command, 0, sizeof(command));*/
            command.funcstacking   = qnzkna::ProtocolXygsDY::XygsDY38MM::XygsDY38MMKZMsg_Command_Stacking::XygsDY54KZMsg_Command_Insure;
            //command.controlcommand = (~currentEquip) & 0x3F;
            command.controlcommand = 0x00;
            command.randomstack[0] = 0x19;
            command.randomstack[1] = 0x1a;
            command.verify         = VerifyAccumulation(&command);
            sendMessage_38mm_Launcher_Control((const char * const)(&command), sizeof(command));

            static std::vector<QCheckBox*> AmmoEquip =
            {
                ui->checkBox_38mmlauncher_Control_equip_1,
                ui->checkBox_38mmlauncher_Control_equip_2,
                ui->checkBox_38mmlauncher_Control_equip_3,
                ui->checkBox_38mmlauncher_Control_equip_4,
                ui->checkBox_38mmlauncher_Control_equip_5,
                ui->checkBox_38mmlauncher_Control_equip_6
            };

            for (int i = 0; i < AmmoEquip.size(); ++i)
            {
                AmmoEquip[i]->setChecked(false);
                AmmoEquip[i]->setEnabled(true);
            }

            ui->pushButton_38mmlauncher_Control_Percussion->setEnabled(true);

        }
    }
#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    else if (m_SensorType == "FireControl38mmPWM" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM_PWM && ui->pushButton_38mmlauncher_Control_lock)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("是否解锁38mm弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            bool ok = false;
            const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
            if (!ok)
            {
                LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
                return;
            }

            ui->pushButton_38mmlauncher_Control_Percussion->setEnabled(true);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"38mm PWM 弹药挂架 解锁";
        }
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_38mmlauncher_Control_Percussion()
{
    // 击发
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl38mm" && m_SensorType != "FireControl38mmPWM")
    {
        LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("配置弹药类型不是38mm弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

    if (m_SensorType == "FireControl38mm" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM && ui->pushButton_38mmlauncher_Control_unlock)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("是否击发38mm弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            uint8_t currentEquip = GetCurrentHitEquip();
            if (currentEquip == 0x00) {
                LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("请选择击发38mm的弹药?"), QMessageBox::Yes | QMessageBox::No);
                return;
            }
            qnzkna::ProtocolXygsDY::XygsDY38MM::FireControl_XygsDY38MM_Command command;
            /*memset(&command, 0, sizeof(command));*/
            command.funcstacking   = qnzkna::ProtocolXygsDY::XygsDY38MM::XygsDY38MMKZMsg_Command_Stacking::XygsDY54KZMsg_Command_Percussion;
            command.controlcommand = currentEquip;
            command.randomstack[0] = 0x18;
            command.randomstack[1] = 0x19;
            command.verify         = VerifyAccumulation(&command);
            sendMessage_38mm_Launcher_Control((const char * const)(&command), sizeof(command));
        }
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    else if (m_SensorType == "FireControl38mmPWM" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_38MM_PWM && ui->pushButton_38mmlauncher_Control_lock)
    {
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("38mm弹药 警告"), tr2("是否击发38mm弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            bool ok = false;
            const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
            if (!ok)
            {
                LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
                return;
            }

            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_38MM_ServoPWM;    // 38mm PWM 弹药挂架
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_38MM_ServoPWM_Shot;// 38mm PWM 弹药挂架 击发

            qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
            memset(&data, 0, sizeof(data));
            data.servoPWM_38MM.servoPWMNumber_38MM = 13;
            data.servoPWM_38MM.servoPWMvalue_38MM  = 1800;

            SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"38mm PWM 弹药挂架 击发 " << data.servoPWM_38MM.servoPWMvalue_38MM;
        }
    }
#endif

}

int IUAVNavigationArmShotAlone54Dialog::sendMessage_38mm_Launcher_Control(const char * const pbuf, const int pbufSize)
{
    if(m_UseUdpSocket && m_udpSocket)
    {
        //UDP发送
        if (nullptr == m_udpSocket || pbufSize <= 0 || pbuf == nullptr) {
            return 0;
        }
        qint64 byteSent = m_udpSocket->writeDatagram(pbuf, pbufSize, (m_UdpHostAddressRemote), m_UdpHostAddressRemotePort);
        return byteSent;
    }

    bool ok = false;
    const unsigned long long unNodeID = QString::fromStdString(m_CurrentSensorID).toULongLong(&ok);
    if (!ok)
    {
        return false;
    }
    char sendBuf[1024]       = { 0 };
    memset(sendBuf, 0, sizeof(sendBuf));
    NetHeader* netHeaderPtr  = (NetHeader*)(sendBuf);

    netHeaderPtr->ucType     = NETHEADER_UCTYPE_DY38MM_CONTROL;
    netHeaderPtr->ucSubType  = NETHEADER_UCSUBTYPE_Q20A_CMD_UP_TO_SERVER;
    netHeaderPtr->unNodeID   = unNodeID;
    netHeaderPtr->unNodeType = 3;

    netHeaderPtr->unSeatType = NETHEADER_UNSEATTYPE_CLIENT;
    netHeaderPtr->unLength    = sizeof(NetHeader) + pbufSize;
    memcpy(sendBuf + sizeof(NetHeader), pbuf, pbufSize);

    return LY_UAVNavigationArmShotAlone54Interface::Get().SendData(sendBuf, netHeaderPtr->unLength);
}


//95-1
void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Light()
{
 // 观瞄(可见光)
    bool ok = false;
    const unsigned long long unNodeID = m_videoControlSensorInfoID.toULongLong(&ok);
    if (!ok)
    {
        return ;
    }

    unsigned char pbuf[sizeof(unsigned long long) + 1]       = { 0 };
    (*(unsigned long long*)(pbuf)) = unNodeID;
    pbuf[sizeof(pbuf) - 1]         = 0x01;
    const int pbufSize             = sizeof(pbuf);


    char sendBuf[1024]       = { 0 };
    memset(sendBuf, 0, sizeof(sendBuf));
    NetHeader* netHeaderPtr  = (NetHeader*)(sendBuf);

    netHeaderPtr->ucType     = NETHEADER_UCTYPE_VIDEO_CMD;
    netHeaderPtr->ucSubType  = NETHEADER_UCSUBTYPE_Q20A_CMD_UP_TO_SERVER;
    netHeaderPtr->unNodeID   = unNodeID;
    netHeaderPtr->unNodeType = 3;

    netHeaderPtr->unSeatType = NETHEADER_UNSEATTYPE_CLIENT;
    netHeaderPtr->unLength   = sizeof(NetHeader) + pbufSize;
    memcpy(sendBuf + sizeof(NetHeader), &pbuf, pbufSize);

    LY_UAVNavigationArmShotAlone54Interface::Get().SendData(sendBuf, netHeaderPtr->unLength);
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Light_Infrared()
{
 // 观瞄(红外)
    bool ok = false;
    const unsigned long long unNodeID = m_videoControlSensorInfoID.toULongLong(&ok);
    if (!ok)
    {
        return ;
    }

    unsigned char pbuf[sizeof(unsigned long long) + 1]       = { 0 };
    (*(unsigned long long*)(pbuf)) = unNodeID;
    pbuf[sizeof(pbuf) - 1]         = 0x07;
    const int pbufSize             = sizeof(pbuf);


    char sendBuf[1024]       = { 0 };
    memset(sendBuf, 0, sizeof(sendBuf));
    NetHeader* netHeaderPtr   = (NetHeader*)(sendBuf);

    netHeaderPtr->ucType      = NETHEADER_UCTYPE_VIDEO_CMD;
    netHeaderPtr->ucSubType   = NETHEADER_UCSUBTYPE_Q20A_CMD_UP_TO_SERVER;
    netHeaderPtr->unNodeID    = unNodeID;
    netHeaderPtr->unNodeType  = 3;

    netHeaderPtr->unSeatType = NETHEADER_UNSEATTYPE_CLIENT;
    netHeaderPtr->unLength = sizeof(NetHeader) + pbufSize;
    memcpy(sendBuf + sizeof(NetHeader), &pbuf, pbufSize);

    LY_UAVNavigationArmShotAlone54Interface::Get().SendData(sendBuf, netHeaderPtr->unLength);
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Infrared()
{
 // 前置
    bool ok = false;
    const unsigned long long unNodeID = m_videoControlSensorInfoID.toULongLong(&ok);
    if (!ok)
    {
        return ;
    }

   unsigned char pbuf[sizeof(unsigned long long) + 1]       = { 0 };
    (*(unsigned long long*)(pbuf)) = unNodeID;
    pbuf[sizeof(pbuf) - 1]         = 0x02;
    const int pbufSize             = sizeof(pbuf);


    char sendBuf[1024]       = { 0 };
    memset(sendBuf, 0, sizeof(sendBuf));
    NetHeader* netHeaderPtr  = (NetHeader*)(sendBuf);

    netHeaderPtr->ucType     = NETHEADER_UCTYPE_VIDEO_CMD;
    netHeaderPtr->ucSubType  = NETHEADER_UCSUBTYPE_Q20A_CMD_UP_TO_SERVER;
    netHeaderPtr->unNodeID   = unNodeID;
    netHeaderPtr->unNodeType = 3;

    netHeaderPtr->unSeatType = NETHEADER_UNSEATTYPE_CLIENT;
    netHeaderPtr->unLength   = sizeof(NetHeader) + pbufSize;
    memcpy(sendBuf + sizeof(NetHeader), &pbuf, pbufSize);

    LY_UAVNavigationArmShotAlone54Interface::Get().SendData(sendBuf, netHeaderPtr->unLength);
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Video_Close()
{
    bool ok = false;
    const unsigned long long unNodeID = m_videoControlSensorInfoID.toULongLong(&ok);
    if (!ok)
    {
        return ;
    }

    unsigned char pbuf[sizeof(unsigned long long) + 1]       = { 0 };
    (*(unsigned long long*)(pbuf)) = unNodeID;
    pbuf[sizeof(pbuf) - 1]         = 0x00;
    const int pbufSize             = sizeof(pbuf);


    char sendBuf[1024]       = { 0 };
    memset(sendBuf, 0, sizeof(sendBuf));
    NetHeader* netHeaderPtr  = (NetHeader*)(sendBuf);

    netHeaderPtr->ucType     = NETHEADER_UCTYPE_VIDEO_CMD;
    netHeaderPtr->ucSubType  = NETHEADER_UCSUBTYPE_Q20A_CMD_UP_TO_SERVER;
    netHeaderPtr->unNodeID   = unNodeID;
    netHeaderPtr->unNodeType = 3;

    netHeaderPtr->unSeatType = NETHEADER_UNSEATTYPE_CLIENT;
    netHeaderPtr->unLength   = sizeof(NetHeader) + pbufSize;
    memcpy(sendBuf + sizeof(NetHeader), &pbuf, pbufSize);

    LY_UAVNavigationArmShotAlone54Interface::Get().SendData(sendBuf, netHeaderPtr->unLength);
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_jiguang_open()
{
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }


        m_XygsDY95_1_SBUS_Command.channel_15 = 1900;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
        // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
        // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

        // xygsDY95_1_SBUS_Command.channel_15 = 1900;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

        xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


        sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));


        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 打开激光 ";
    }
    else  if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_TibetanMastiffQ20A_95_1_Wolf_ServoPWM;    // 机器狼狗 95-1 弹药挂架
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_UAV_95_1_ServoPWM_Laser_Open;// 无人机  95-1 弹药挂架 激光

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.servoPWM_95_1.servoPWMNumber_95_1 = 11;
        data.servoPWM_95_1.servoPWMvalue_95_1  = 1900;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 打开激光 " << data.servoPWM_95_1.servoPWMvalue_95_1;
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_jiguang_close()
{
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }


        m_XygsDY95_1_SBUS_Command.channel_15 = 1000;
        m_XygsDY95_1_SBUS_Command.channel_15 = 500;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
        // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
        // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

        // xygsDY95_1_SBUS_Command.channel_15 = 1000;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

        xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


        sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));


        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 关闭激光 ";
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_TibetanMastiffQ20A_95_1_Wolf_ServoPWM;    // 机器狼狗 95-1 弹药挂架
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_UAV_95_1_ServoPWM_Laser_Close;// 无人机  95-1 弹药挂架 激光

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.servoPWM_95_1.servoPWMNumber_95_1 = 11;
        data.servoPWM_95_1.servoPWMvalue_95_1  = 1000;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 关闭激光 " << data.servoPWM_95_1.servoPWMvalue_95_1;
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_focus_far()
{
    bIsFocusFar = !bIsFocusFar;
    if (bIsFocusFar == false)
    {
        on_pushButton_95_1launcher_Control_focus_stop();
        return;
    }
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }


        m_XygsDY95_1_SBUS_Command.channel_11 = 1900;
        m_XygsDY95_1_SBUS_Command.channel_11 = 1700;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
        // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
        // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

        // xygsDY95_1_SBUS_Command.channel_11 = 1900;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

        xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


        sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));


        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 变焦远 ";
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_TibetanMastiffQ20A_95_1_Wolf_ServoPWM;    // 机器狼狗 95-1 弹药挂架
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_UAV_95_1_ServoPWM_Focus_Far;// 无人机  95-1 弹药挂架 变焦

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.servoPWM_95_1.servoPWMNumber_95_1 = 10;
        data.servoPWM_95_1.servoPWMvalue_95_1  = 1900;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);
        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 变焦远 " << data.servoPWM_95_1.servoPWMvalue_95_1;
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_focus_near()
{
    if (ui == nullptr) {
        return;
    }

    bIsFocusNear = !bIsFocusNear;
    if (bIsFocusNear == false)
    {
        on_pushButton_95_1launcher_Control_focus_stop();
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }


        m_XygsDY95_1_SBUS_Command.channel_11 = 1100;
        m_XygsDY95_1_SBUS_Command.channel_11 = 200;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
        // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
        // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

        // xygsDY95_1_SBUS_Command.channel_11 = 1100;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

        xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


        sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));
        sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));


        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 变焦近 ";
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_TibetanMastiffQ20A_95_1_Wolf_ServoPWM;    // 机器狼狗 95-1 弹药挂架
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_UAV_95_1_ServoPWM_Focus_Near;// 无人机  95-1 弹药挂架 变焦

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.servoPWM_95_1.servoPWMNumber_95_1 = 10;
        data.servoPWM_95_1.servoPWMvalue_95_1  = 1100;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);
        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 变焦近 " << data.servoPWM_95_1.servoPWMvalue_95_1;
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_focus_stop()
{
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }


        m_XygsDY95_1_SBUS_Command.channel_11 = 1500;
        m_XygsDY95_1_SBUS_Command.channel_11 = 1000;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
        // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
        // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

        // xygsDY95_1_SBUS_Command.channel_11 = 1500;

        qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

        xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


        sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));

        // 总是不停止变焦 多发2次
        sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));
        sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));


        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 停止变焦 ";
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_TibetanMastiffQ20A_95_1_Wolf_ServoPWM;    // 机器狼狗 95-1 弹药挂架
        constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_UAV_95_1_ServoPWM_Focus_Stop;// 无人机  95-1 弹药挂架 变焦

        qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
        memset(&data, 0, sizeof(data));
        data.servoPWM_95_1.servoPWMNumber_95_1 = 10;
        data.servoPWM_95_1.servoPWMvalue_95_1  = 1500;

        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        // 总是不停止变焦 多发2次
        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);
        SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);

        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 停止变焦 " << data.servoPWM_95_1.servoPWMvalue_95_1;
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_fuyang_up()
{
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_puyang_down()
{

}

void IUAVNavigationArmShotAlone54Dialog::on_verticalSlider_95_1launcher_Control_fuyang(int position)
{
    // 俯仰

      if (ui == nullptr) {
          return;
      }

      if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
      {
          LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
          return;
      }

  #ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
      if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
      {

          bool ok = false;
          const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
          if (!ok)
          {
              LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
              return;
          }


          //if (position > 700 && position < 2000)
          if (position > 200 && position < 1800)
          {
              m_XygsDY95_1_SBUS_Command.channel_13 = position;

              qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
              // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
              // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

              // xygsDY95_1_SBUS_Command.channel_13 = position;

              // 状态不用交换大小端字节
              if(m_UseUdpSocket_95_1_StatusSync){
                  xygsDY95_1_SBUS_Command.verify = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof(xygsDY95_1_SBUS_Command) - 2);

                  qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_CommandStatusSync xygsDY95_1_SBUS_CommandStatusSync;
                  xygsDY95_1_SBUS_CommandStatusSync.command       = xygsDY95_1_SBUS_Command;
                  xygsDY95_1_SBUS_CommandStatusSync.componentID   = componentID;
                  xygsDY95_1_SBUS_CommandStatusSync.seatUIFlag    = (uint64_t)ui;

                  sendMessage__95_1_StatusSync_control((const char * const)(&xygsDY95_1_SBUS_CommandStatusSync), sizeof(xygsDY95_1_SBUS_CommandStatusSync));
              }

              qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

              xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


              sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));

              logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << componentID << u8"95-1弹药挂架 俯仰" << position ;
          }
      }
      else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
      {

          bool ok = false;
          const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
          if (!ok)
          {
              LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
              return;
          }


      }
  #endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_lock()
{
    // 加锁
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否加锁95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            {
                m_XygsDY95_1_SBUS_Command.channel_16 = 0;
                m_XygsDY95_1_SBUS_Command.channel_14 = 2000;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;


                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

                xygsDY95_1_SBUS_Command.verify = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof(xygsDY95_1_SBUS_Command) - 2);


                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));
            }

            ui->pushButton_95_1launcher_Control_Percussion->setEnabled(false);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 加锁" ;
        }
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否加锁95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {

            ui->pushButton_95_1launcher_Control_Percussion->setEnabled(false);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 加锁" ;
        }
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_unlock()
{
    // 解锁
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否解锁95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {

            {
                m_XygsDY95_1_SBUS_Command.channel_16 = 1900;
                m_XygsDY95_1_SBUS_Command.channel_14 = 2000;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;


                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

                xygsDY95_1_SBUS_Command.verify = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof(xygsDY95_1_SBUS_Command) - 2);


                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));
            }


            ui->pushButton_95_1launcher_Control_Percussion->setEnabled(true);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 解锁";
        }
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否解锁95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {

            ui->pushButton_95_1launcher_Control_Percussion->setEnabled(true);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 解锁";
        }
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::on_pushButton_95_1launcher_Control_Percussion()
{
    // 击发
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否击发95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            {
                //m_XygsDY95_1_SBUS_Command.channel_14 = 2000;
                m_XygsDY95_1_SBUS_Command.channel_14 = 0;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
                // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
                // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

                // xygsDY95_1_SBUS_Command.channel_14 = 2000;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

                xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));

                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));


                for (int i = 0; i < 10; i++) {
                    QThread::msleep(20);
                    QCoreApplication::processEvents(QEventLoop::AllEvents);
                }
            }

            {
                //m_XygsDY95_1_SBUS_Command.channel_14 = 0; 2000
                m_XygsDY95_1_SBUS_Command.channel_14 = 2000;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
                // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
                // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

                // xygsDY95_1_SBUS_Command.channel_14 = 800;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

                xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));

                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));

                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));
            }

        }
        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 击发（单发，扳机舵机） ";
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否击发95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_TibetanMastiffQ20A_95_1_Wolf_ServoPWM;    // 机器狼狗 95-1 弹药挂架
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_UAV_95_1_ServoPWM_Shot;      // 人机  95-1 弹药挂架 击发

            qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
            memset(&data, 0, sizeof(data));
            data.servoPWM_95_1.servoPWMNumber_95_1 = 9;
            data.servoPWM_95_1.servoPWMvalue_95_1  = 2200;

            SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);


            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 击发（单发，扳机舵机） " << data.servoPWM_95_1.servoPWMvalue_95_1;
        }
    }
#endif
}

int IUAVNavigationArmShotAlone54Dialog::sendMessage__95_1_StatusSync_control(const char * const pbuf, const int pbufSize)
{
    if( m_UseUdpSocket_95_1_StatusSync) {
        //UDP发送
        if (pbufSize <= 0 || pbuf == nullptr) {
            return 0;
        }
        LY_UAVNavigationArmShotAlone54::Get().sendMessage_95_1_StatusSync_control(pbuf, pbufSize);
    }
    return 0;
}

int IUAVNavigationArmShotAlone54Dialog::sendMessage__95_1Launcher_Control(const char * const pbuf, const int pbufSize)
{
    if(m_UseUdpSocket && m_udpSocket)
    {
        //UDP发送
        if (nullptr == m_udpSocket || pbufSize <= 0 || pbuf == nullptr) {
            return 0;
        }
        qint64 byteSent = m_udpSocket->writeDatagram(pbuf, pbufSize, (m_UdpHostAddressRemote), m_UdpHostAddressRemotePort);
        return byteSent;
    }

    bool ok = false;
    const unsigned long long unNodeID = QString::fromStdString(m_CurrentSensorID).toULongLong(&ok);
    if (!ok)
    {
        return false;
    }
    char sendBuf[1024] = { 0 };
    memset(sendBuf, 0, sizeof(sendBuf));
    NetHeader* netHeaderPtr = (NetHeader*)(sendBuf);

    netHeaderPtr->ucType = NETHEADER_UCTYPE_DY95_1_CONTROL;
    netHeaderPtr->ucSubType = NETHEADER_UCSUBTYPE_Q20A_CMD_UP_TO_SERVER;
    netHeaderPtr->unNodeID = unNodeID;
    netHeaderPtr->unNodeType = 3;

    netHeaderPtr->unSeatType = NETHEADER_UNSEATTYPE_CLIENT;
    netHeaderPtr->unLength = sizeof(NetHeader) + pbufSize;
    memcpy(sendBuf + sizeof(NetHeader), pbuf, pbufSize);

    return LY_UAVNavigationArmShotAlone54Interface::Get().SendData(sendBuf, netHeaderPtr->unLength);
}

int IUAVNavigationArmShotAlone54Dialog::sendMessage__95_1_Wolf_Launcher_Control(const char * const pbuf, const int pbufSize)
{
    return 0;
}


//// 38mm  95-1 加锁 解锁 击发
void IUAVNavigationArmShotAlone54Dialog::Control__95_1__38MM__lock()
{
    // 加锁
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        // if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否加锁95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            {
                m_XygsDY95_1_SBUS_Command.channel_16 = 0;
                m_XygsDY95_1_SBUS_Command.channel_14 = 2000;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;


                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

                xygsDY95_1_SBUS_Command.verify = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof(xygsDY95_1_SBUS_Command) - 2);


                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));
            }

            ui->pushButton_95_1launcher_Control_Percussion->setEnabled(false);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 加锁" ;
        }
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        // if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否加锁95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {

            ui->pushButton_95_1launcher_Control_Percussion->setEnabled(false);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 加锁" ;
        }
    }
#endif

}

void IUAVNavigationArmShotAlone54Dialog::Control__95_1__38MM__unlock()
{
    // 解锁
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        // if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否解锁95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {

            {
                m_XygsDY95_1_SBUS_Command.channel_16 = 1900;
                m_XygsDY95_1_SBUS_Command.channel_14 = 2000;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;


                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

                xygsDY95_1_SBUS_Command.verify = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof(xygsDY95_1_SBUS_Command) - 2);


                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));
            }


            ui->pushButton_95_1launcher_Control_Percussion->setEnabled(true);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 解锁";
        }
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        // if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否解锁95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {

            ui->pushButton_95_1launcher_Control_Percussion->setEnabled(true);

            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 解锁";
        }
    }
#endif
}

void IUAVNavigationArmShotAlone54Dialog::Control__95_1__38MM__Percussion()
{
    // 击发
    if (ui == nullptr) {
        return;
    }

    if (m_SensorType != "FireControl95-1" && m_SensorType != "FireControl95-1-Wolf")
    {
        LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("配置弹药类型不是95-1弹药!"), QMessageBox::Yes | QMessageBox::No);
        return;
    }

#ifdef USE_LYSD_DATAMANAGESYSTEMINTERNALINTERFACE_SYSTEMINTERNALINTERFACE_SYSTEMFIREPROTCONTROL_ENABLE
    if (m_SensorType == "FireControl95-1" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1 && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        // if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否击发95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            {
                //m_XygsDY95_1_SBUS_Command.channel_14 = 2000;
                m_XygsDY95_1_SBUS_Command.channel_14 = 0;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
                // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
                // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

                // xygsDY95_1_SBUS_Command.channel_14 = 2000;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

                xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));

                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));


                for (int i = 0; i < 10; i++) {
                    QThread::msleep(20);
                    QCoreApplication::processEvents(QEventLoop::AllEvents);
                }
            }

            {
                //m_XygsDY95_1_SBUS_Command.channel_14 = 0; 2000
                m_XygsDY95_1_SBUS_Command.channel_14 = 2000;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command xygsDY95_1_SBUS_Command = m_XygsDY95_1_SBUS_Command;
                // memset(&xygsDY95_1_SBUS_Command, 0, sizeof(xygsDY95_1_SBUS_Command));
                // xygsDY95_1_SBUS_Command.head = XygsDY95_1_SBUS_KZMSG_HEAD;

                // xygsDY95_1_SBUS_Command.channel_14 = 800;

                qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Swap(xygsDY95_1_SBUS_Command);

                xygsDY95_1_SBUS_Command.verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(&xygsDY95_1_SBUS_Command)) + 1), sizeof (xygsDY95_1_SBUS_Command) - 2 );


                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));

                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));

                sendMessage__95_1Launcher_Control((const char * const)(&xygsDY95_1_SBUS_Command), sizeof(xygsDY95_1_SBUS_Command));
            }

        }
        logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1弹药挂架 击发（单发，扳机舵机） ";
    }
    else if (m_SensorType == "FireControl95-1-Wolf" && m_DY54_BombType == DY54_BombType_Enum::DY54_BombType_Enum_95_1_Wolf && ui->pushButton_95_1launcher_Control_lock)
    {

        bool ok = false;
        const unsigned long long int componentID = m_currentComponentID.toULongLong(&ok);
        if (!ok)
        {
            LyMessageBox::warning(nullptr, u8"警告", u8"无人机ID获取失败，指令未发送！", QMessageBox::Yes | QMessageBox::No);
            return;
        }

        // if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("95-1弹药 警告"), tr2("是否击发95-1弹药?"), QMessageBox::Yes | QMessageBox::No))
        {
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Enum        fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_TibetanMastiffQ20A_95_1_Wolf_ServoPWM;    // 机器狼狗 95-1 弹药挂架
            constexpr qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_Enum    fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_UAV_95_1_ServoPWM_Shot;      // 人机  95-1 弹药挂架 击发

            qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControlData data;
            memset(&data, 0, sizeof(data));
            data.servoPWM_95_1.servoPWMNumber_95_1 = 9;
            data.servoPWM_95_1.servoPWMvalue_95_1  = 2200;

            SystemFireProtControl_Data_SendMsgToAll(componentID, fireProtType, fireProtOperType, data);


            logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << u8"95-1-Wolf弹药挂架 击发（单发，扳机舵机） " << data.servoPWM_95_1.servoPWMvalue_95_1;
        }
    }
#endif
}



bool IUAVNavigationArmShotAlone54Dialog::getCurrentTarget(QString &strId)
{
    bool bTargetFromTable = ui->radioButton_Target_HaveFromTable->isChecked();
    if (bTargetFromTable)
    {
        QVariant vt = ui->comboBox_radioButton_Target_HaveFromTable->currentData();
        if (vt.isValid())
        {
            strId = vt.toString();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        QString strTxt = ui->lineEdit_Target_New_ID->text();
        if (0 == strTxt.length())
        {
            strId = createTargetID();
            QString strName = strId;
            ui->lineEdit_Target_New_ID->setText(strName);

            double dLon = 0, dLan = 0, dAlt = 0;
            qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *pStateService = IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService();
            if (pStateService)
            {
                qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t info_t;
                std::string strComponentId = m_currentComponentID.toStdString();
                bool bGet = pStateService->GetUAVRealTimeState(strComponentId, info_t);
                if (bGet)
                {
                    dLon = info_t.telemetryInfo.position.lng;
                    dLan = info_t.telemetryInfo.position.lat;
                    dAlt = info_t.telemetryInfo.position.heigth;
                }
            }

            QString strSql("INSERT INTO ZZMB_XTMBXX (`MBBH`, `MBMC`, `MBLX`, `MBSX`, `MBCZ`, `MBJD`, `MBWD`, `MBGD`, `MBSD`, `MBHX`, `MBMS`,`VXDJ`,`FXSJ`,`FXZB`, `CCCD`, `CCKD`, `CCGD`, `ZCHSYQ`, `SMDJSJ` , `YHBWDX`, `YHBWFX`, `MBMJ`, `SHD`, `FKZB`, `HSZB`) VALUES (");
            strSql += "'" + strId + "',";
            strSql += "'" + strId + "',";
            strSql += "'" + QString::number(0) + "',";
            strSql += "'" + QString::number(1) + "',";
            strSql += "'" + QString::number(0) + "',";
            strSql += "'" + QString::number(dLon, 'f', 7) + "',";
            strSql += "'" + QString::number(dLan, 'f', 7) + "',";
            strSql += "'" + QString::number(dAlt, 'f', 2) + "',";
            strSql += "'" + QString::number(0, 'f', 2) + "',";
            strSql += "'" + QString::number(0, 'f', 2) + "',";
            strSql += "'" + QString("") + "',";
            strSql += "'" + QString::number(1) + "',";
            strSql += "'" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "',";
            strSql += "'" + QString("") + "',";
            strSql += "'" + QString::number(0, 'f', 6) + "',";
            strSql += "'" + QString::number(0, 'f', 6) + "',";
            strSql += "'" + QString::number(0, 'f', 6) + "',";
            strSql += "'" + QString("") + "',";
            strSql += "'" + QString::number(0, 'f', 2) + "',";
            strSql += "'" + QString::number(0, 'f', 2) + "',";
            strSql += "'" + QString::number(0, 'f', 2) + "',";
            strSql += "'" + QString::number(0, 'f', 2) + "',";
            strSql += "'" + QString::number(0, 'f', 2) + "',";
            strSql += "'" + QString::number(0, 'f', 2) + "',";
            strSql += "'" + QString::number(0, 'f', 2) + "'";
            strSql += ")";

            QSqlQuery query;
            query.prepare(strSql);
            if (query.exec())
            {
                ui->comboBox_radioButton_Target_HaveFromTable->addItem(strId, strId);
                return true;
            }
        }
        else
        {
            return true;
        }
    }
    return false;
}

QString IUAVNavigationArmShotAlone54Dialog::createTargetID()
{
    return QDateTime::currentDateTime().toString("hh:mm:ss zzz");
}