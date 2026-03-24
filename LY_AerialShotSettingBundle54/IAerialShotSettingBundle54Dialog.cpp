#include "IAerialShotSettingBundle54Dialog.h"
#include "ui_IAerialShotSettingBundle54Dialog.h"

#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
    #include "LY_AerialShotSettingBundle54Service.h"
#endif

#include <Protocol/XygsZC/Bag_XygsZCDefines.h>
#include "LY_AerialShotSettingBundle54Service.h"
#include "VehicleDeflectionDistYawPitchDialog.h"
#include <QCString.h>
#include <LyWidget.h>
#include <LyMessageBox.h>
#include <QDateTime>
#include <math.h>
#include <time.h>

#define __DEBUG__LOGINFO__
#if defined(__DEBUG__LOGINFO__)
#include <QDebug>
#define logInfoFormat(format, ...)  qDebug(format, ##__VA_ARGS__)
#define logInfo                     qDebug

QDebug operator<<(QDebug debug, const AMapPod54Control &aMapPod54Control)
{
    const bool oldSetting = debug.autoInsertSpaces();
    debug.nospace() << "AMapPod54Control("
                    <<  QString("CommonCommand:%1").arg(static_cast<uint8_t>(aMapPod54Control.commonCommand), 0, 16)
                    << ')';
    debug.setAutoInsertSpaces(oldSetting);
    return debug.maybeSpace();
}

#else
#define logInfoFormat(format, ...)
#define logInfo() /##/
#endif

const char *IAerialShotSettingBundle54Dialog::QPushButton_Property_ComponentSysID  = "QPushButton_Property_ComponentSysID54";
const char *IAerialShotSettingBundle54Dialog::QPushButton_Property_ComponentName   = "QPushButton_Property_ComponentName54";
const char *IAerialShotSettingBundle54Dialog::QPushButton_Property_ComponentID     = "QPushButton_Property_ComponentID54";
const char *IAerialShotSettingBundle54Dialog::QPushButton_Property_SensorName      = "QPushButton_Property_SensorName54";
const char *IAerialShotSettingBundle54Dialog::QPushButton_Property_SensorID        = "QPushButton_Property_SensorID54";

unsigned char  xorStr(unsigned char *buf, int start, int end)
{
    int sum = 0;
    unsigned char re;

    int i = start - 1;
    for (; i < end ; i++)
    {
        sum = sum ^ buf[i];
    }
    re = (unsigned char)(sum & 0x000000FF);
    return re;
}

static const char *m_AerialShotSettingBundle54_Joystick_Property_ID                                      = "Gimbal54 Joy";
static const char *m_AerialShotSettingBundle54_Joystick_Property_ID_X                                    = "X";
static const char *m_AerialShotSettingBundle54_Joystick_Property_ID_Y                                    = "Y";
static bool                                               m_InitAerialShotSettingBundle54Function_DIJOFS = false;
static qnzkna::JoystickManage::JoystickStateTypeFunction  m_AerialShotSettingBundle54Function_DIJOFS_X   = nullptr;
static qnzkna::JoystickManage::JoystickStateTypeFunction  m_AerialShotSettingBundle54Function_DIJOFS_Y   = nullptr;

IAerialShotSettingBundle54Dialog::IAerialShotSettingBundle54Dialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::IAerialShotSettingBundle54Dialog)
    , m_WidgetShow(false)
    , m_DeflectionAngleofNose(0)
    , m_DeflectionAngleofPitch(0)
    , m_LightElectronicZoom(0)
    , m_LaserDistance(0)
    , m_AMapPod54StatusInfo{}
    , m_CurrentSensorID("")
    , m_CurrentComponentID("")
    , m_CurrentComponentName("")
    , m_pButtonCurrentComponent(nullptr)
    , m_pButtonGroupComponent(nullptr)
    , m_Widget_54_All_Function(false)
    , m_Joy_FineTuning(false)
    , m_JoyGimbalLock(true)
    , m_JoystickEnableGimbal(true)
    , m_Telemeter_Timer_Start(false)
    , m_Telemeter_Timer(this)
    , m_AerialShotSettingTelemetrySend54(true)
    , m_AerialShotSetting54AllFunction(false)
    , m_FlyControl54TelemetrySend(true)
    , m_FlyControl54TelemetrySendTime(0)
    , m_FlyControl54TelemetryTimer(this)
    , m_AerialShotSettingBundle54Timer(this)
    , m_AerialShotSettingBundle54TimerFunction(nullptr)
    , m_AerialShotSettingBundle54TimerJoystickControlSimple(this)
    , m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction(nullptr)
{
    static_assert(sizeof(AMapPod54Control) == 44, "AMapPod54Control 44-bit code generation is not supported.");
    static_assert(sizeof(AMapPod54Control) == AMAPPOD54CONTROL_LENGTH, "AMapPod54Control 44-bit code generation is not supported.");
    static_assert(sizeof(AMapPod54ControlStatus) == 64, "AMapPod54ControlStatus 64-bit code generation is not supported.");

    static_assert(std::is_pod<AMapPod54Control>::value,    "AMapPod54Control pod is not supported.");
    static_assert(std::is_pod<AMapPod54ControlStatus>::value,    "AMapPod54ControlStatus pod is not supported.");

    QSettings settings("./Data/conf/AerialShotSettingBundleSettings.ini", QSettings::IniFormat);
    settings.beginGroup("AerialShotSettings");
    if (settings.contains("AerialShotSettingTelemetrySend54ServerClient"))
    {
        m_AerialShotSettingTelemetrySend54 = settings.value("AerialShotSettingTelemetrySend54ServerClient", true).toBool();
    }

    if (settings.contains("AerialShotSetting54AllFunction"))
    {
        m_AerialShotSetting54AllFunction = settings.value("AerialShotSetting54AllFunction", false).toBool();
    }
    settings.endGroup();

    memset(&m_AMapPod54StatusInfo, 0, sizeof(m_AMapPod54StatusInfo));

    m_pButtonGroupComponent = new QButtonGroup(this);
    m_pButtonGroupComponent->setObjectName(QString::fromUtf8("Button_54_GroupComponent"));

    ui->setupUi(this);

    m_VehicleDeflectionDistYawPitch54Map.clear();
    m_AMapPod54ControlStatusInfoMap.clear();
    m_ZoomDistanceMap.clear();

    m_Widget_54_All_Function = false;

    ui->widget_54_Common_All_Function_Switch_Simple->show();
    ui->widget_54_Common_All_Function_Switch_Simple->setVisible(true);
    ui->widget_54_Common_All_Function_Switch_Common->hide();
    ui->widget_54_Common_All_Function_Switch_Common->setVisible(false);
    ui->widget_54_Common_All_Function_Switch_All->hide();
    ui->widget_54_Common_All_Function_Switch_All->setVisible(false);

    if (!m_AerialShotSetting54AllFunction)
    {
        ui->pushButton_54_Common_All_Function_Switch_All->hide();
        ui->pushButton_54_Common_All_Function_Switch_All->setVisible(false);
    }

    on_radioButton_JoystickDisEnable_clicked();
    on_radioButton_JoystickDisEnable_Common_clicked();
    ui->radioButton_JoystickDisEnable_Common->setChecked(false);
    ui->radioButton_JoystickDisEnable->setChecked(false);
    ui->radioButton_JoystickDisEnable_Common->setChecked(true);
    ui->radioButton_JoystickDisEnable->setChecked(true);

    m_JoystickEnableGimbal = ui->radioButton_JoystickEnableGimbal->isChecked();

    m_AerialShotSettingBundle54Timer.setInterval(300);
    m_AerialShotSettingBundle54TimerFunction = nullptr;
    connect(&m_AerialShotSettingBundle54Timer, &QTimer::timeout, [this]()
    {
        if (m_AerialShotSettingBundle54TimerFunction)
        {
            m_AerialShotSettingBundle54TimerFunction();
        }
    });

    m_AerialShotSettingBundle54TimerJoystickControlSimple.setInterval(300);
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;
    connect(&m_AerialShotSettingBundle54TimerJoystickControlSimple, &QTimer::timeout, [this]()
    {
        if (m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction)
        {
            m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction();
        }
    });

    connect(this, &IAerialShotSettingBundle54Dialog::ShowAMapPodStatusInfoSig, this, &IAerialShotSettingBundle54Dialog::ShowAMapPodStatusInfo, Qt::ConnectionType::QueuedConnection);

    connect(ui->label_JoystickWidget_Common, &JoystickWidget::joystickAxis,         this, &IAerialShotSettingBundle54Dialog::joystickAxisAerialShotSettingBundle54);
    connect(ui->label_JoystickWidget_Common, &JoystickWidget::mouseReleaseEventSig, this, &IAerialShotSettingBundle54Dialog::mouseReleaseEventSigAerialShotSettingBundle54);

    connect(ui->label_JoystickWidget, &JoystickWidget::joystickAxis,         this, &IAerialShotSettingBundle54Dialog::joystickAxisAerialShotSettingBundle54);
    connect(ui->label_JoystickWidget, &JoystickWidget::mouseReleaseEventSig, this, &IAerialShotSettingBundle54Dialog::mouseReleaseEventSigAerialShotSettingBundle54);

    connect(this, &IAerialShotSettingBundle54Dialog::joystickAxisAerialShotSettingBundle54, [this](long int lX, long int lY, long int lYoumen, long int lFangwei)
    {
        Q_UNUSED(lYoumen)
        Q_UNUSED(lFangwei)
        logInfo() << lX << lY << lYoumen << lFangwei;

        int16_t gimbalYaw   = 0;
        int16_t gimbalPitch = 0;

        if (lX > 0 && lX <= 100)
        {
            gimbalYaw = lX;
        }
        else if (lX < 0 && lX >= -100)
        {
            gimbalYaw = lX;
        }
        else
        {
            gimbalYaw = 0;
        }

        if (lY > 0 && lY <= 100)
        {
            gimbalPitch = lY;
        }
        else if (lY < 0 && lY >= -100)
        {
            gimbalPitch = lY;
        }
        else
        {
            gimbalPitch = 0;
        }
        if (gimbalYaw != 0 || gimbalPitch != 0)
        {
            if (m_JoystickEnableGimbal)
            {
                if (m_Joy_FineTuning)
                {
                    if (gimbalPitch < 30)
                    {
                        gimbalPitch = gimbalPitch / 10;
                    }
                    if (gimbalYaw < 30)
                    {
                        gimbalYaw = gimbalYaw / 10;
                    }
                }

                AMapPod54Control aMapPod54Control;
                memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
                aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = gimbalYaw / 10.0;
                aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = gimbalPitch   / 10.0;

                SendAMapPodControlMsg(aMapPod54Control);

                logInfo() << __FILE__ << __LINE__ << tr2("启用摇杆") << aMapPod54Control;
            }
            else
            {

                const int16_t PTZTrackControlYaw   = static_cast<int16_t>(gimbalYaw / 100.0 * 960);
                const int16_t PTZTrackControlPitch = static_cast<int16_t>(gimbalPitch / 100.0 * 540);

                AMapPod54Control aMapPod54Control;
                memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;
                aMapPod54Control.PTZJoyStickControlArg.PTZTrackControl.PTZTrackControlYaw   =  PTZTrackControlYaw;
                aMapPod54Control.PTZJoyStickControlArg.PTZTrackControl.PTZTrackControlPitch =  PTZTrackControlPitch;

                SendAMapPodControlMsg(aMapPod54Control);

                SendAMapPodControlZeroMsg();

                logInfo() << tr2("踪目标坐 选取跟踪目标坐标值") << aMapPod54Control << PTZTrackControlYaw << PTZTrackControlPitch;
            }
        }
    });

    connect(this, &IAerialShotSettingBundle54Dialog::mouseReleaseEventSigAerialShotSettingBundle54, [this](long int lX, long int lY, long int lYoumen, long int lFangwei)
    {
        Q_UNUSED(lYoumen)
        Q_UNUSED(lFangwei)
        logInfo() << lX << lY << lYoumen << lFangwei;

        int16_t gimbalYaw   = 0;
        int16_t gimbalPitch = 0;

        if (lX > 0 && lX <= 100)
        {
            gimbalYaw = lX;
        }
        else if (lX < 0 && lX >= -100)
        {
            gimbalYaw = lX;
        }
        else
        {
            gimbalYaw = 0;
        }

        if (lY > 0 && lY <= 100)
        {
            gimbalPitch = lY;
        }
        else if (lY < 0 && lY >= -100)
        {
            gimbalPitch = lY;
        }
        else
        {
            gimbalPitch = 0;
        }
        if (gimbalYaw != 0 || gimbalPitch != 0)
        {

            if (m_JoystickEnableGimbal)
            {
                if (m_Joy_FineTuning)
                {
                    if (gimbalPitch < 30)
                    {
                        gimbalPitch = gimbalPitch / 10;
                    }
                    if (gimbalYaw < 30)
                    {
                        gimbalYaw = gimbalYaw / 10;
                    }
                }

                AMapPod54Control aMapPod54Control;
                memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
                aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = gimbalYaw / 10.0;
                aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = gimbalPitch   / 10.0;

                SendAMapPodControlMsg(aMapPod54Control);

                logInfo() << __FILE__ << __LINE__ << tr2("启用摇杆") << aMapPod54Control;
            }
            else
            {

                const int16_t PTZTrackControlYaw   = static_cast<int16_t>(gimbalYaw / 100.0 * 960);
                const int16_t PTZTrackControlPitch = static_cast<int16_t>(gimbalPitch / 100.0 * 540);

                AMapPod54Control aMapPod54Control;
                memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                aMapPod54Control.commonCommand      = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StartSelectPointTracking;
                aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;
                aMapPod54Control.PTZJoyStickControlArg.PTZTrackControl.PTZTrackControlYaw   =  PTZTrackControlYaw;
                aMapPod54Control.PTZJoyStickControlArg.PTZTrackControl.PTZTrackControlPitch =  PTZTrackControlPitch;

                SendAMapPodControlMsg(aMapPod54Control);

                logInfo() << tr2("选取跟踪目标坐标值") << aMapPod54Control;
            }

            m_JoyGimbalLock = true;
        }
        else if (m_JoyGimbalLock)
        {
            m_JoyGimbalLock = false;
            AMapPod54Control aMapPod54Control;
            memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
            aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

            SendAMapPodControlMsg(aMapPod54Control);

            SendAMapPodControlZeroMsg();
            SendAMapPodControlZeroMsg();

            logInfo() << tr2("云台 禁用摇杆 方位锁定") << aMapPod54Control;
        }
    });

    if (m_AerialShotSettingTelemetrySend54)
    {
        m_FlyControl54TelemetryTimer.setInterval(300);
        connect(&m_FlyControl54TelemetryTimer, &QTimer::timeout, [this]()
        {
            if ( m_AerialShotSettingBundle54TimerFunction == nullptr)
            {

                for (const auto *buttonComponent : m_pButtonGroupComponent->buttons())
                {
                    const QVariant &variantComponentID = buttonComponent->property(QPushButton_Property_ComponentID);
                    const QVariant &variantSensorID    = buttonComponent->property(QPushButton_Property_SensorID);
                    if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
                    {
                        m_CurrentSensorID      = "";
                        m_CurrentComponentID   = "";
                        m_CurrentComponentName = "";
                        continue ;
                    }

                    const QString &componentID = variantComponentID.toString();
                    const QString &sensorID    = variantSensorID.toString();

                    if (componentID.isEmpty() || sensorID.isEmpty())
                    {
                        continue ;
                    }

                    const std::string strComponentID = componentID.toStdString();
                    const std::string strSensorID    = sensorID.toStdString();

                    bool sendFlyControl54Telemetry = true;
                    if (m_CurrentComponentID == componentID && (::time(nullptr) - m_FlyControl54TelemetrySendTime < 2))
                    {
                        sendFlyControl54Telemetry = false;
                    }

                    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *pIUAVRealTimeStateManageService = IAerialShotSettingBundle54Activator::Get().getPIUAVRealTimeStateManageService();
                    if (sendFlyControl54Telemetry && pIUAVRealTimeStateManageService != nullptr)
                    {

                        float deflectionAngleofNose = 0;
                        float deflectionAngleofPitch = 0;
                        getVehicleDeflectionDistYawPitch54ZoomDistanceValue(strComponentID, sensorID, deflectionAngleofNose, deflectionAngleofPitch);

                        qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stelemetryInformation;
                        bool result = pIUAVRealTimeStateManageService->GetUAVRealTimeState(strComponentID, stelemetryInformation);
                        if (result)
                        {
                            FlyControl54Telemetry flyControl54Telemetry;
                            flyControl54Telemetry.pitch            = lround(stelemetryInformation.telemetryInfo.angleofPitch * 100) - deflectionAngleofPitch;
                            flyControl54Telemetry.roll             = lround(stelemetryInformation.telemetryInfo.angleofRoll * 100);
                            flyControl54Telemetry.yaw              = lround(stelemetryInformation.telemetryInfo.angleofNose * 100) - deflectionAngleofNose;
                            flyControl54Telemetry.year             = stelemetryInformation.telemetryInfo._year;
                            flyControl54Telemetry.month            = stelemetryInformation.telemetryInfo._month;
                            flyControl54Telemetry.day              = stelemetryInformation.telemetryInfo._day;
                            flyControl54Telemetry.hour             = stelemetryInformation.telemetryInfo._hour;
                            flyControl54Telemetry.minute           = stelemetryInformation.telemetryInfo._min;
                            flyControl54Telemetry.second           = stelemetryInformation.telemetryInfo._sec;
                            flyControl54Telemetry.millisecond      = 0;
                            flyControl54Telemetry.longitude        = static_cast<float32_t>(stelemetryInformation.telemetryInfo.position.lng);
                            flyControl54Telemetry.latitude         = static_cast<float32_t>(stelemetryInformation.telemetryInfo.position.lat);
                            flyControl54Telemetry.satellites       = stelemetryInformation.telemetryInfo.gpsNumber;
                            flyControl54Telemetry.altitude         = lround(stelemetryInformation.telemetryInfo.gpsHeigth * 10.0);
                            flyControl54Telemetry.groundSpeed      = lround(stelemetryInformation.telemetryInfo.groundVelocity * (10.0 / 36));
                            flyControl54Telemetry.airspeed         = lround(stelemetryInformation.telemetryInfo.groundVelocity * 5);
                            flyControl54Telemetry.altitudeRelative = lround(stelemetryInformation.telemetryInfo.relativeOriginHeigth * 10);

                            AMapPod54Control aMapPod54Control;
                            memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));

                            aMapPod54Control.telemetryData = flyControl54Telemetry;

                            const int start = (AMAPPOD54CONTROL_HEAD_LENGTH + 1);
                            const int end   = (AMAPPOD54CONTROL_LENGTH - AMAPPOD54CONTROL_TAIL_LENGTH - 1);
                            aMapPod54Control.verify  = xorStr((unsigned char *)(&aMapPod54Control), start, end);
                            aMapPod54Control.head[0] = AMAPPOD54CONTROL_FIRST_HEAD;
                            aMapPod54Control.head[1] = AMAPPOD54CONTROL_SECOND_HEAD;
                            aMapPod54Control.end     = AMAPPOD54CONTROL_SECOND_TAIL;

#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
                            if (LY_AerialShotSettingBundle54Service::Get().getNetServerClient())
                            {
                                LY_AerialShotSettingBundle54Service::Get().NetServerClientSend(aMapPod54Control, sensorID);
                            }
                            else
#endif
                            {
                                IAerialShotSettingBundle54Activator::Get().sendMessage((const char *)(&aMapPod54Control), sizeof(AMapPod54Control), strSensorID, strComponentID);
                            }
                        }
                    }
                }
            }
        });
        m_FlyControl54TelemetryTimer.start();
    }

    initAMapPodState();

    qnzkna::JoystickManage::IJoystickManageService     *pIJoystickManageService = IAerialShotSettingBundle54Activator::Get().getPIJoystickManageService();
    if (pIJoystickManageService != nullptr)
    {

        m_AerialShotSettingBundle54Function_DIJOFS_X = [this](const std::string & joystickStateType, const std::string & joystickStateMode, const double joystickStateData)
        {
            if (!m_InitAerialShotSettingBundle54Function_DIJOFS
                    && (joystickStateType != m_AerialShotSettingBundle54_Joystick_Property_ID)
                    && (joystickStateMode != m_AerialShotSettingBundle54_Joystick_Property_ID_X))
            {
                return ;
            }
            if (m_Widget_54_All_Function && ui->label_JoystickWidget->getAxisVisible())
            {
                ui->label_JoystickWidget->setAxisXValue(joystickStateData * 100.0 / QNZKNA_JOYSTICKMANAGE_JOYSTICKDIRECT_JOYSTICK_AXIS_MAX);
            }
            else if (ui->label_JoystickWidget_Common->getAxisVisible())
            {
                ui->label_JoystickWidget_Common->setAxisXValue(joystickStateData * 100.0 / QNZKNA_JOYSTICKMANAGE_JOYSTICKDIRECT_JOYSTICK_AXIS_MAX);
            }

            qDebug() << __FILE__ << __LINE__ << joystickStateMode.c_str() << joystickStateType.c_str() << joystickStateData << (joystickStateData * 100.0 / QNZKNA_JOYSTICKMANAGE_JOYSTICKDIRECT_JOYSTICK_AXIS_MAX);
        };

        m_AerialShotSettingBundle54Function_DIJOFS_Y = [this](const std::string & joystickStateType, const std::string & joystickStateMode, const double joystickStateData)
        {
            if (!m_InitAerialShotSettingBundle54Function_DIJOFS
                    && (joystickStateType != m_AerialShotSettingBundle54_Joystick_Property_ID)
                    && (joystickStateMode != m_AerialShotSettingBundle54_Joystick_Property_ID_Y))
            {
                return ;
            }
            if (m_Widget_54_All_Function && ui->label_JoystickWidget->getAxisVisible())
            {
                ui->label_JoystickWidget->setAxisYValue(joystickStateData * 100.0 / QNZKNA_JOYSTICKMANAGE_JOYSTICKDIRECT_JOYSTICK_AXIS_MAX);
            }
            else if (ui->label_JoystickWidget_Common->getAxisVisible())
            {
                ui->label_JoystickWidget_Common->setAxisYValue(joystickStateData * 100.0 / QNZKNA_JOYSTICKMANAGE_JOYSTICKDIRECT_JOYSTICK_AXIS_MAX);
            }
            qDebug() << __FILE__ << __LINE__ << joystickStateMode.c_str() << joystickStateType.c_str() << joystickStateData << (joystickStateData * 100.0 / QNZKNA_JOYSTICKMANAGE_JOYSTICKDIRECT_JOYSTICK_AXIS_MAX);
        };

        struct qnzkna::JoystickManage::JoystickStateDataListener joystickStateDataListener;
        joystickStateDataListener.joystickStateDataListener            = this;
        joystickStateDataListener.joystickStateDataListenerFunction    = [this](const std::string & joystickStateType, const std::string & joystickStateMode, const double joystickStateData)
        {
            if (!m_InitAerialShotSettingBundle54Function_DIJOFS && joystickStateType != m_AerialShotSettingBundle54_Joystick_Property_ID)
            {
                return ;
            }
            if (m_Widget_54_All_Function && ui->label_JoystickWidget->getAxisVisible())
            {
                ui->label_JoystickWidget->updateAxisValue();
            }
            else if (ui->label_JoystickWidget_Common->getAxisVisible())
            {
                ui->label_JoystickWidget_Common->updateAxisValue();
            }
        };
        joystickStateDataListener.joystickStateType                    = m_AerialShotSettingBundle54_Joystick_Property_ID;
        joystickStateDataListener.joystickStateModeFunctionMap         =
        {
            {m_AerialShotSettingBundle54_Joystick_Property_ID_X, m_AerialShotSettingBundle54Function_DIJOFS_X},
            {m_AerialShotSettingBundle54_Joystick_Property_ID_Y, m_AerialShotSettingBundle54Function_DIJOFS_Y}
        };

        m_InitAerialShotSettingBundle54Function_DIJOFS = true;
        pIJoystickManageService->AddJoystickListener(joystickStateDataListener);
    }

    connect(this, &IAerialShotSettingBundle54Dialog::AutoAmimingSig, this, [this](const QString strComponentID, const QByteArray data)
    {
        if (strComponentID.isEmpty())
        {
            return;
        }

        if (data.length() != sizeof(qnzkna::DY_ZC_Control::DY_ZC_ControlInfo))
        {
            return;
        }

        const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo *const DY_ZC_ControlInfoData = reinterpret_cast<const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo *const>(data.constData());

        if (DY_ZC_ControlInfoData == nullptr)
        {
            return;
        }

        if (strComponentID != QString::number(DY_ZC_ControlInfoData->componentID))
        {
            return;
        }

        if (DY_ZC_ControlInfoData->oper == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_AutoAiming)
        {
            if (DY_ZC_ControlInfoData->operData == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_AutoAiming_Start)
            {

                QString strComponentIDTemp;
                QString strSensorIDTemp;

                for (const QAbstractButton *const button : m_pButtonGroupComponent->buttons())
                {
                    const QVariant &variantComponentID = button->property(QPushButton_Property_ComponentID);
                    const QVariant &variantSensorID = button->property(QPushButton_Property_SensorID);
                    if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
                    {
                        continue;
                    }

                    if (strComponentID == variantComponentID.toString())
                    {
                        strComponentIDTemp = variantComponentID.toString();
                        strSensorIDTemp = variantSensorID.toString();
                        break;
                    }
                }

                if (strComponentIDTemp.isEmpty() || strSensorIDTemp.isEmpty())
                {
                    return;
                }

                if (DY_ZC_ControlInfoData->bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Rocket)
                {
                    AMapPod54Control aMapPod54Control;
                    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalCenter;

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    AMapPod54Control aMapPod54ControlZero;
                    memset(&aMapPod54ControlZero, 0x00, sizeof(aMapPod54ControlZero));
                    SendAMapPodControlMsg(aMapPod54ControlZero, strComponentIDTemp, strSensorIDTemp);

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    SendAMapPodControlMsg(aMapPod54ControlZero, strComponentIDTemp, strSensorIDTemp);
                    logInfo() << tr2("云台归中") << aMapPod54Control;
                }
                else if (DY_ZC_ControlInfoData->bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Cruise)
                {
                    AMapPod54Control aMapPod54Control;
                    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalLookDownVertical;

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    AMapPod54Control aMapPod54ControlZero;
                    memset(&aMapPod54ControlZero, 0x00, sizeof(aMapPod54ControlZero));
                    SendAMapPodControlMsg(aMapPod54ControlZero, strComponentIDTemp, strSensorIDTemp);

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    SendAMapPodControlMsg(aMapPod54ControlZero, strComponentIDTemp, strSensorIDTemp);
                    logInfo() << tr2("垂直下视") << aMapPod54Control;
                }
                else
                {
                    AMapPod54Control aMapPod54Control;
                    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalCenter;

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    AMapPod54Control aMapPod54ControlZero;
                    memset(&aMapPod54ControlZero, 0x00, sizeof(aMapPod54ControlZero));
                    SendAMapPodControlMsg(aMapPod54ControlZero, strComponentIDTemp, strSensorIDTemp);

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    SendAMapPodControlMsg(aMapPod54ControlZero, strComponentIDTemp, strSensorIDTemp);
                    logInfo() << tr2("云台归中") << aMapPod54Control;
                }
            }
            else if (DY_ZC_ControlInfoData->operData == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_AutoAiming_Stop)
            {
                QString strComponentIDTemp;
                QString strSensorIDTemp;

                for (const QAbstractButton *const button : m_pButtonGroupComponent->buttons())
                {
                    const QVariant &variantComponentID = button->property(QPushButton_Property_ComponentID);
                    const QVariant &variantSensorID = button->property(QPushButton_Property_SensorID);
                    if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
                    {
                        continue;
                    }

                    if (strComponentID == variantComponentID.toString())
                    {
                        strComponentIDTemp = variantComponentID.toString();
                        strSensorIDTemp = variantSensorID.toString();
                        break;
                    }
                }

                if (strComponentIDTemp.isEmpty() || strSensorIDTemp.isEmpty())
                {
                    return;
                }

                if (DY_ZC_ControlInfoData->bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Rocket)
                {

                }
                else if (DY_ZC_ControlInfoData->bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Cruise)
                {

                }
                else
                {
                }
            }
            else
            {
            }
        }
        else if (DY_ZC_ControlInfoData->oper == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_MoveCenter)
        {
            if (DY_ZC_ControlInfoData->operData == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None)
            {
                QString strComponentIDTemp;
                QString strSensorIDTemp;

                for (const QAbstractButton *const button : m_pButtonGroupComponent->buttons())
                {
                    const QVariant &variantComponentID = button->property(QPushButton_Property_ComponentID);
                    const QVariant &variantSensorID = button->property(QPushButton_Property_SensorID);
                    if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
                    {
                        continue;
                    }

                    if (strComponentID == variantComponentID.toString())
                    {
                        strComponentIDTemp = variantComponentID.toString();
                        strSensorIDTemp = variantSensorID.toString();
                        break;
                    }
                }

                if (strComponentIDTemp.isEmpty() || strSensorIDTemp.isEmpty())
                {
                    return;
                }

                const int x = DY_ZC_ControlInfoData->x;
                const int y = DY_ZC_ControlInfoData->y;
                const int width = DY_ZC_ControlInfoData->width;
                const int heigth = DY_ZC_ControlInfoData->heigth;
                if (width <= 0 || heigth <= 0 || x <= 0 || y <= 0 || y > width || y > heigth)
                {
                    return;
                }

                const int16_t PTZTrackControlYaw = static_cast<int16_t>(1.0 * x / width * (2 * 960.0)) - 960.0;
                const int16_t PTZTrackControlPitch = static_cast<int16_t>(1.0 * y / heigth * (2 * 540.0)) - 540.0;

                {
                    AMapPod54Control aMapPod54Control;
                    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;
                    aMapPod54Control.PTZJoyStickControlArg.PTZTrackControl.PTZTrackControlYaw = PTZTrackControlYaw;
                    aMapPod54Control.PTZJoyStickControlArg.PTZTrackControl.PTZTrackControlPitch = -PTZTrackControlPitch;

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);
                }

                {
                    AMapPod54Control aMapPod54Control;
                    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StartSelectPointTracking;
                    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;
                    aMapPod54Control.PTZJoyStickControlArg.PTZTrackControl.PTZTrackControlYaw = PTZTrackControlYaw;
                    aMapPod54Control.PTZJoyStickControlArg.PTZTrackControl.PTZTrackControlPitch = -PTZTrackControlPitch;

                    qDebug() << x << y << width << heigth << PTZTrackControlYaw << -PTZTrackControlPitch;
                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);
                }
            }
        }
        else if (DY_ZC_ControlInfoData->oper == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_SingleLaserTelemeter)
        {
            qDebug() << DY_ZC_ControlInfoData->x;
            if (DY_ZC_ControlInfoData->operData == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None)
            {
                QString strComponentIDTemp;
                QString strSensorIDTemp;

                for (const QAbstractButton *const button : m_pButtonGroupComponent->buttons())
                {
                    const QVariant &variantComponentID = button->property(QPushButton_Property_ComponentID);
                    const QVariant &variantSensorID = button->property(QPushButton_Property_SensorID);
                    if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
                    {
                        continue;
                    }

                    if (strComponentID == variantComponentID.toString())
                    {
                        strComponentIDTemp = variantComponentID.toString();
                        strSensorIDTemp = variantSensorID.toString();
                        break;
                    }
                }

                if (strComponentIDTemp.isEmpty() || strSensorIDTemp.isEmpty())
                {
                    return;
                }

                {
                    const auto constFindIter = m_AMapPod54ControlStatusInfoMap.constFind(strSensorIDTemp);
                    if (constFindIter != m_AMapPod54ControlStatusInfoMap.constEnd())
                    {
                        auto &aMapPod54ControlStatusInfoMap = m_AMapPod54ControlStatusInfoMap[strSensorIDTemp];
                        aMapPod54ControlStatusInfoMap.laserStarted = true;
                        aMapPod54ControlStatusInfoMap.laserDistanceContinueStarted = false;
                        aMapPod54ControlStatusInfoMap.laserDistanceStarted = true;
                        aMapPod54ControlStatusInfoMap.receivedCmdFeedback = 0;
                        aMapPod54ControlStatusInfoMap.receivedLaserDistanceCount = 0;
                        aMapPod54ControlStatusInfoMap.laserDistanceTimes = ::time(nullptr);
                        aMapPod54ControlStatusInfoMap.laserDistanceCount = 0;

                        if (aMapPod54ControlStatusInfoMap.laserTimer != nullptr)
                        {
                            aMapPod54ControlStatusInfoMap.laserTimer->stop();
                        }
                        AMapPod54Control aMapPod54Control;
                        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_SingleLaserTelemeter;

                        SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                        logInfo() << tr2("单次激光测距") << aMapPod54Control << strComponentIDTemp << QDateTime::currentDateTime().toString("yyyy_MM_dd-hh:mm:ss.zzz");
                    }

                    {
                        AMapPod54Control aMapPod54Control;
                        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StopTracking;

                        SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                        AMapPod54Control aMapPod54ControlZero;
                        memset(&aMapPod54ControlZero, 0x00, sizeof(aMapPod54ControlZero));
                        SendAMapPodControlMsg(aMapPod54ControlZero, strComponentIDTemp, strSensorIDTemp);
                    }
                }
            }

        }
        else if (DY_ZC_ControlInfoData->oper == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_StopTracking)
        {
            if (DY_ZC_ControlInfoData->operData == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None)
            {
                QString strComponentIDTemp;
                QString strSensorIDTemp;

                for (const QAbstractButton *const button : m_pButtonGroupComponent->buttons())
                {
                    const QVariant &variantComponentID = button->property(QPushButton_Property_ComponentID);
                    const QVariant &variantSensorID = button->property(QPushButton_Property_SensorID);
                    if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
                    {
                        continue;
                    }

                    if (strComponentID == variantComponentID.toString())
                    {
                        strComponentIDTemp = variantComponentID.toString();
                        strSensorIDTemp = variantSensorID.toString();
                        break;
                    }
                }

                if (strComponentIDTemp.isEmpty() || strSensorIDTemp.isEmpty())
                {
                    return;
                }
                {
                    AMapPod54Control aMapPod54Control;
                    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StopTracking;

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    AMapPod54Control aMapPod54ControlZero;
                    memset(&aMapPod54ControlZero, 0x00, sizeof(aMapPod54ControlZero));
                    SendAMapPodControlMsg(aMapPod54ControlZero, strComponentIDTemp, strSensorIDTemp);
                }
            }
        }
        else if (DY_ZC_ControlInfoData->oper == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_AttitudeGuideFollow)
        {
            if (DY_ZC_ControlInfoData->operData == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None)
            {
                QString strComponentIDTemp;
                QString strSensorIDTemp;

                for (const QAbstractButton *const button : m_pButtonGroupComponent->buttons())
                {
                    const QVariant &variantComponentID = button->property(QPushButton_Property_ComponentID);
                    const QVariant &variantSensorID = button->property(QPushButton_Property_SensorID);
                    if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
                    {
                        continue;
                    }

                    if (strComponentID == variantComponentID.toString())
                    {
                        strComponentIDTemp = variantComponentID.toString();
                        strSensorIDTemp = variantSensorID.toString();
                        break;
                    }
                }

                if (strComponentIDTemp.isEmpty() || strSensorIDTemp.isEmpty())
                {
                    return;
                }
                {
                    double attitudeGuideFollow_Yaw = DY_ZC_ControlInfoData->x / 100.0;
                    if ((attitudeGuideFollow_Yaw < -180.0) || (attitudeGuideFollow_Yaw > 180.0))
                    {
                        return;
                    }

                    double attitudeGuideFollow_Pitch = DY_ZC_ControlInfoData->y / 100.0;
                    if ((attitudeGuideFollow_Pitch < -120.0) || (attitudeGuideFollow_Pitch > 90.0))
                    {
                        return;
                    }

                    int16_t yaw = static_cast<int16_t>(attitudeGuideFollow_Yaw * 100);
                    int16_t pitch = static_cast<int16_t>(attitudeGuideFollow_Pitch * 100);

                    AMapPod54Control aMapPod54Control;
                    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_AttitudeGuideFollow;
                    aMapPod54Control.commonCommandArg.AttitudeLeadSet_7C.yaw = yaw;
                    aMapPod54Control.commonCommandArg.AttitudeLeadSet_7C.pitch = pitch;

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    logInfo() << tr2(" 云台功能 姿态指引") << aMapPod54Control << yaw << pitch;
                }
            }
        }
        else if (DY_ZC_ControlInfoData->oper == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_PTZJoyStickControl)
        {
            if (DY_ZC_ControlInfoData->operData == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None)
            {
                QString strComponentIDTemp;
                QString strSensorIDTemp;

                for (const QAbstractButton *const button : m_pButtonGroupComponent->buttons())
                {
                    const QVariant &variantComponentID = button->property(QPushButton_Property_ComponentID);
                    const QVariant &variantSensorID = button->property(QPushButton_Property_SensorID);
                    if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
                    {
                        continue;
                    }

                    if (strComponentID == variantComponentID.toString())
                    {
                        strComponentIDTemp = variantComponentID.toString();
                        strSensorIDTemp = variantSensorID.toString();
                        break;
                    }
                }

                if (strComponentIDTemp.isEmpty() || strSensorIDTemp.isEmpty())
                {
                    return;
                }
                {
                    double PTZJoyStickControl_Yaw = DY_ZC_ControlInfoData->x;
                    if ((PTZJoyStickControl_Yaw < -100) || (PTZJoyStickControl_Yaw > 100))
                    {
                        return;
                    }

                    double PTZJoyStickControl_Pitch = DY_ZC_ControlInfoData->y;
                    if ((PTZJoyStickControl_Pitch < -100) || (PTZJoyStickControl_Pitch > 100))
                    {
                        return;
                    }

                    int16_t yaw = static_cast<int16_t>(PTZJoyStickControl_Yaw);
                    int16_t pitch = static_cast<int16_t>(PTZJoyStickControl_Pitch);

                    AMapPod54Control aMapPod54Control;
                    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
                    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw = yaw;
                    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = pitch;

                    SendAMapPodControlMsg(aMapPod54Control, strComponentIDTemp, strSensorIDTemp);

                    logInfo() << tr2(" 云台功能 启用摇杆") << aMapPod54Control << yaw << pitch;
                }
            }
        }

    }, Qt::QueuedConnection);
}

IAerialShotSettingBundle54Dialog::~IAerialShotSettingBundle54Dialog()
{
    m_InitAerialShotSettingBundle54Function_DIJOFS = false;

    delete ui;
}

void IAerialShotSettingBundle54Dialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    m_WidgetShow = true;
}

void IAerialShotSettingBundle54Dialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    m_WidgetShow = false;
}

void IAerialShotSettingBundle54Dialog::changeEvent(QEvent *event)
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

void IAerialShotSettingBundle54Dialog::initAMapPodState()
{
    m_DeflectionAngleofNose  = 0;
    m_DeflectionAngleofPitch = 0;
    m_LightElectronicZoom    = 0;
    m_LaserDistance          = 0;

    memset(&m_AMapPod54StatusInfo, 0, sizeof(m_AMapPod54StatusInfo));

    {
        ui->label_54_Status_PalyloadType->setText(AMapPod54ControlStatus_PayloadType_ToStr(m_AMapPod54StatusInfo.payloadType));

        ui->label_54_Status_InfraredCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(m_AMapPod54StatusInfo.autoCheckResult.infraredMovement));

        ui->label_54_Status_LightCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(m_AMapPod54StatusInfo.autoCheckResult.lightMovement));

        ui->label_54_Status_GyroCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(m_AMapPod54StatusInfo.autoCheckResult.gyroscopeData));

        ui->label_54_Status_AngleSensorCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(m_AMapPod54StatusInfo.autoCheckResult.angleSensor));

        ui->label_54_Status_MotorCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(m_AMapPod54StatusInfo.autoCheckResult.driveBoard));

        ui->label_54_Status_CompressCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(m_AMapPod54StatusInfo.autoCheckResult.compressStorage));

        ui->label_54_Status_CompositeHandCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(m_AMapPod54StatusInfo.autoCheckResult.comprehensiveProcess));

        ui->label_54_Status_PrepareCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(m_AMapPod54StatusInfo.autoCheckResult.podReadyStatus));

        ui->label_54_Status_TakePicture->setText((m_AMapPod54StatusInfo.pod1Status.photographStatus ? tr("Taking pictures") : tr("Unphotographed")));

        ui->label_54_Status_CapPicture->setText((m_AMapPod54StatusInfo.pod1Status.videoCapture ? tr("Video capture") : tr("Unvideo capture diagram")));

        ui->label_54_Status_ContinueTakePicture->setText((m_AMapPod54StatusInfo.pod1Status.burstStatus ? tr("Continuous capture") : tr("Not continuous capture")));

        ui->label_54_Status_ContinueCapPicture->setText((m_AMapPod54StatusInfo.pod1Status.videoBurstCapture ? tr("Continuous video capture") : tr("Not continuous video capture")));

        ui->label_54_Status_RecodeVideo->setText((m_AMapPod54StatusInfo.pod1Status.recordingStatus ? tr("video-recordings") : tr("Not video-recordings")));

        ui->label_54_Status_FileRename->setText((m_AMapPod54StatusInfo.pod1Status.fileRenameImportComplete ? tr("The file named information is finished") : tr("Unsuccessful import")));

        ui->label_54_Status_LightTFStatus->setText((m_AMapPod54StatusInfo.pod1Status.TFCardInsert ? tr("Have Tf card") : tr("Not have Tf card")));

        ui->label_54_Status_LightNetLinkStatus->setText((m_AMapPod54StatusInfo.pod1Status.networkConnection ? tr("Network connection normal") : tr("Network connection abnormal")));

        ui->label_54_Status_LightTFOperatorStatus->setText((m_AMapPod54StatusInfo.pod1Status.TFCardOperation ? tr("Tf card oper normal") : tr("Tf card oper abnormal")));

        ui->label_54_Status_LightShowPictureRespond->setText(QString::number(m_AMapPod54StatusInfo.pod1Status.infraredImageEnhancement));

        ui->label_54_Status_VideoOutput->setText((m_AMapPod54StatusInfo.pod2Status.videoOutput ? tr("Ligth video output") : tr("Infrared video output")));

        ui->label_54_Status_TrackingSource->setText((m_AMapPod54StatusInfo.pod2Status.traceSource ? tr("Ligth") : tr("Infrared")));

        ui->label_54_Status_InfraredOut->setText(QString::number(m_AMapPod54StatusInfo.pod2Status.infraredElectronicZoom * 10));

        ui->label_54_Status_LightOut->setText(QString::number(std::pow(2, m_AMapPod54StatusInfo.pod2Status.lightElectronicZoom)));

        ui->label_54_Status_Ribbon->setText(AMapPod54Control_FalseColorBandModel_ToStr(m_AMapPod54StatusInfo.pod2Status.ribbon));

        ui->label_54_Status_MotorStatus->setText(AMapPod54ControlStatus_ServoStatus_ToStr(m_AMapPod54StatusInfo.servoStatus));

        ui->label_54_Status_GimbalYaw->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podFrameworkYaw / 100.0)));

        ui->label_54_Status_GimbalPitch->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podFrameworkPitch / 100.0)));

        ui->label_54_Status_GimbalRoll->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podFrameworkRoll / 100.0)));

        ui->label_54_Status_ABVersion->setText(QString::asprintf("A:%u,B:%u", m_AMapPod54StatusInfo.payloadVersion.payloadVersionA, m_AMapPod54StatusInfo.payloadVersion.payloadVersionB));

        ui->label_54_Status_InfraredViewAngle->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.infraredViewAngle / 10.0)));

        ui->label_54_Status_TFRemainPer->setText(QString::asprintf("%u%%", m_AMapPod54StatusInfo.tfPercentageOfCapacityUsed));

        ui->label_54_Status_LigthViewAngle->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.lightViewAngle / 10.0)));

        ui->label_54_Status_TFCapacity->setText(QString::asprintf("%.1fG", (m_AMapPod54StatusInfo.tfTotalCapacity / 10.0)));

        ui->label_54_Status_InfraredFocus->setText(QString::asprintf("%.1fmm", (m_AMapPod54StatusInfo.infraredFocalLength / 10.0)));

        ui->label_54_Status_LightFocus->setText(QString::asprintf("%.1fmm", (m_AMapPod54StatusInfo.lightFocalLength / 10.0)));

        ui->label_54_Status_TelemeterDistanceValue->setText(QString::asprintf("%.1fm", (m_AMapPod54StatusInfo.laserDistance / 10.0)));

        ui->label_54_Status_TargetLongitude->setText(QString::asprintf("%.6f", m_AMapPod54StatusInfo.targetLongitude));

        ui->label_54_Status_TargetLatitude->setText(QString::asprintf("%.6f", m_AMapPod54StatusInfo.targetLatitude));

        ui->label_54_Status_TargetAltitude->setText(QString::asprintf("%.1fm", (m_AMapPod54StatusInfo.targetAltitude / 10.0)));

        ui->label_54_Status_Temperature->setText(QString::asprintf(u8"%.1f°", (m_AMapPod54StatusInfo.temperature / 10.0)));

        ui->label_54_Status_GimbalAttitudeYaw->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podAttitudeYaw / 100.0)));

        ui->label_54_Status_GimbalAttitudePitch->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podAttitudePitch / 100.0)));

        ui->label_54_Status_GimbalAttitudeRoll->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podAttitudeRoll / 100.0)));

        ui->label_54_Status_LightShowPictureRespond->setText(AMapPod54ControlStatus_CurrentDisplayImageFeedback_ToStr(m_AMapPod54StatusInfo.currentDisplayImageFeedback));

        ui->label_54_Status_RecvCmdRespond->setText(QString::asprintf("%u", m_AMapPod54StatusInfo.receivedCmdFeedback));

        ui->label_54_Status_AttitudeIndicateComplageRespond->setText(AMapPod54ControlStatus_AttitudeIndicationFeedback_ToStr(m_AMapPod54StatusInfo.attitudeIndicationFeedback));

        ui->label_54_Status_TrackCenterWidthPoint->setText(QString::asprintf("%d", m_AMapPod54StatusInfo.trackCenterWidthCoordinate));

        ui->label_54_Status_TrackCenterHeightPoint->setText(QString::asprintf("%d", m_AMapPod54StatusInfo.trackCenterHeightCoordinate));

        ui->label_TrackWidthLength->setText(QString::asprintf("%u", m_AMapPod54StatusInfo.trackingWidthLength));

        ui->label_54_Status_TrackHeightLength->setText(QString::asprintf("%u", m_AMapPod54StatusInfo.trackingHeightLength));
    }

    {

        ui->label_54_Status_TakePicture_Common->setText((m_AMapPod54StatusInfo.pod1Status.photographStatus ? tr("Taking pictures") : tr("Not Taking pictures")));

        ui->label_54_Status_CapPicture_Common->setText((m_AMapPod54StatusInfo.pod1Status.videoCapture ? tr("Video capture") : tr("Unvideo capture diagram")));

        ui->label_54_Status_ContinueTakePicture_Common->setText((m_AMapPod54StatusInfo.pod1Status.burstStatus ? tr("Continuous capture") : tr("Not continuous capture")));

        ui->label_54_Status_RecodeVideo_Common->setText((m_AMapPod54StatusInfo.pod1Status.recordingStatus ? tr("Video-recordings") : tr("Not video-recordings")));

        ui->label_54_Status_VideoOutput_Common->setText((m_AMapPod54StatusInfo.pod2Status.videoOutput ? tr("Ligth video output") : tr("Infrared video output")));

        ui->label_54_Status_InfraredOut_Common->setText(QString::number(m_AMapPod54StatusInfo.pod2Status.infraredElectronicZoom * 10));

        ui->label_54_Status_LightOut_Common->setText(QString::number(std::pow(2, m_AMapPod54StatusInfo.pod2Status.lightElectronicZoom)));

        ui->label_54_Status_Ribbon_Common->setText(AMapPod54Control_FalseColorBandModel_ToStr(m_AMapPod54StatusInfo.pod2Status.ribbon - 1));

        ui->label_54_Status_GimbalYaw_Common->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podFrameworkYaw / 100.0)));

        ui->label_54_Status_GimbalPitch_Common->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podFrameworkPitch / 100.0)));

        ui->label_54_Status_GimbalRoll_Common->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podFrameworkRoll / 100.0)));

        ui->label_54_Status_TFCapacity_Common->setText(QString::asprintf("%u%%", m_AMapPod54StatusInfo.tfPercentageOfCapacityUsed));

        ui->label_54_Status_InfraredFocus_Common->setText(QString::asprintf("%.1fmm", (m_AMapPod54StatusInfo.infraredFocalLength / 10.0)));

        ui->label_54_Status_LightFocus_Common->setText(QString::asprintf("%.1fmm", (m_AMapPod54StatusInfo.lightFocalLength / 10.0)));

        ui->label_54_Status_TelemeterDistanceValue_Common->setText(QString::asprintf("%.1fm", (m_AMapPod54StatusInfo.laserDistance / 10.0)));

        ui->label_54_Status_TargetLongitude_Common->setText(QString::asprintf("%.6f", (m_AMapPod54StatusInfo.targetLongitude)));

        ui->label_54_Status_TargetLatitude_Common->setText(QString::asprintf("%.6f", (m_AMapPod54StatusInfo.targetLatitude)));

        ui->label_54_Status_TargetAltitude_Common->setText(QString::asprintf("%.2fm", (m_AMapPod54StatusInfo.targetAltitude / 10.0)));

        ui->label_54_Status_GimbalAttitudeYaw_Common->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podAttitudeYaw / 100.0)));

        ui->label_54_Status_GimbalAttitudePitch_Common->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podAttitudePitch / 100.0)));

        ui->label_54_Status_GimbalAttitudeRoll_Common->setText(QString::asprintf(u8"%.2f°", (m_AMapPod54StatusInfo.podAttitudeRoll / 100.0)));

        ui->label_54_Status_LightShowPictureRespond_Common->setText(AMapPod54ControlStatus_CurrentDisplayImageFeedback_ToStr(m_AMapPod54StatusInfo.currentDisplayImageFeedback));

    }
}

void IAerialShotSettingBundle54Dialog::SendAMapPodControlMsg(AMapPod54Control &aMapPod54Control)
{
    if (m_CurrentComponentID.isEmpty() || m_CurrentSensorID.isEmpty())
    {
        return ;
    }

    m_FlyControl54TelemetrySend = false;

    m_FlyControl54TelemetrySendTime = ::time(nullptr);

    const std::string currentComponentID = m_CurrentComponentID.toStdString();
    const std::string currentSensorID    = m_CurrentSensorID.toStdString();

    auto pIUAVRealTimeStateManageService = IAerialShotSettingBundle54Activator::Get().getPIUAVRealTimeStateManageService();

    if (pIUAVRealTimeStateManageService != nullptr)
    {
        qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stelemetryInformation;
        bool result = pIUAVRealTimeStateManageService->GetUAVRealTimeState(currentComponentID, stelemetryInformation);
        if (result)
        {
            getVehicleDeflectionDistYawPitch54Value(currentComponentID);

            FlyControl54Telemetry flyControl54Telemetry;
            flyControl54Telemetry.pitch            = lround(stelemetryInformation.telemetryInfo.angleofPitch * 100) - m_DeflectionAngleofPitch;
            flyControl54Telemetry.roll             = lround(stelemetryInformation.telemetryInfo.angleofRoll * 100);
            flyControl54Telemetry.yaw              = lround(stelemetryInformation.telemetryInfo.angleofNose * 100) - m_DeflectionAngleofNose;
            flyControl54Telemetry.year             = stelemetryInformation.telemetryInfo._year;
            flyControl54Telemetry.month            = stelemetryInformation.telemetryInfo._month;
            flyControl54Telemetry.day              = stelemetryInformation.telemetryInfo._day;
            flyControl54Telemetry.hour             = stelemetryInformation.telemetryInfo._hour;
            flyControl54Telemetry.minute           = stelemetryInformation.telemetryInfo._min;
            flyControl54Telemetry.second           = stelemetryInformation.telemetryInfo._sec;
            flyControl54Telemetry.millisecond      = 0;
            flyControl54Telemetry.longitude        = stelemetryInformation.telemetryInfo.position.lng;
            flyControl54Telemetry.latitude         = stelemetryInformation.telemetryInfo.position.lat;
            flyControl54Telemetry.satellites       = stelemetryInformation.telemetryInfo.gpsNumber;
            flyControl54Telemetry.altitude         = lround(stelemetryInformation.telemetryInfo.gpsHeigth * 10.0);
            flyControl54Telemetry.groundSpeed      = lround(stelemetryInformation.telemetryInfo.groundVelocity * (1.0 / 36));
            flyControl54Telemetry.airspeed         = lround(stelemetryInformation.telemetryInfo.groundVelocity * 2);
            flyControl54Telemetry.altitudeRelative = lround(stelemetryInformation.telemetryInfo.relativeOriginHeigth * 10);

            aMapPod54Control.telemetryData = flyControl54Telemetry;
        }
    }

    const int start = (AMAPPOD54CONTROL_HEAD_LENGTH + 1);
    const int end   = (AMAPPOD54CONTROL_LENGTH - AMAPPOD54CONTROL_TAIL_LENGTH - 1);
    aMapPod54Control.verify  = xorStr((unsigned char *)(&aMapPod54Control), start, end);
    aMapPod54Control.head[0] = AMAPPOD54CONTROL_FIRST_HEAD;
    aMapPod54Control.head[1] = AMAPPOD54CONTROL_SECOND_HEAD;
    aMapPod54Control.end     = AMAPPOD54CONTROL_SECOND_TAIL;

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << QString::fromStdString(currentSensorID)  << QString::fromStdString(currentComponentID) << aMapPod54Control;
#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
    if (LY_AerialShotSettingBundle54Service::Get().getNetServerClient())
    {
        LY_AerialShotSettingBundle54Service::Get().NetServerClientSend(aMapPod54Control, m_CurrentSensorID);
    }
    else
#endif
    {
        IAerialShotSettingBundle54Activator::Get().sendMessage((const char *)(&aMapPod54Control), sizeof(AMapPod54Control), currentSensorID, currentComponentID);
    }

    m_FlyControl54TelemetrySend = true;
}

void IAerialShotSettingBundle54Dialog::SendAMapPodControlMsg(AMapPod54Control &aMapPod54Control, const QString strComponentID, const QString strSensorID)
{

    if (strComponentID.isEmpty() || strSensorID.isEmpty())
    {
        return ;
    }

    m_FlyControl54TelemetrySend = false;

    m_FlyControl54TelemetrySendTime = ::time(nullptr);

    const std::string currentComponentID = strComponentID.toStdString();
    const std::string currentSensorID    = strSensorID.toStdString();

    auto pIUAVRealTimeStateManageService = IAerialShotSettingBundle54Activator::Get().getPIUAVRealTimeStateManageService();

    if (pIUAVRealTimeStateManageService != nullptr)
    {
        qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stelemetryInformation;
        bool result = pIUAVRealTimeStateManageService->GetUAVRealTimeState(currentComponentID, stelemetryInformation);
        if (result)
        {
            getVehicleDeflectionDistYawPitch54Value(currentComponentID);

            FlyControl54Telemetry flyControl54Telemetry;
            flyControl54Telemetry.pitch            = lround(stelemetryInformation.telemetryInfo.angleofPitch * 100) - m_DeflectionAngleofPitch;
            flyControl54Telemetry.roll             = lround(stelemetryInformation.telemetryInfo.angleofRoll * 100);
            flyControl54Telemetry.yaw              = lround(stelemetryInformation.telemetryInfo.angleofNose * 100) - m_DeflectionAngleofNose;
            flyControl54Telemetry.year             = stelemetryInformation.telemetryInfo._year;
            flyControl54Telemetry.month            = stelemetryInformation.telemetryInfo._month;
            flyControl54Telemetry.day              = stelemetryInformation.telemetryInfo._day;
            flyControl54Telemetry.hour             = stelemetryInformation.telemetryInfo._hour;
            flyControl54Telemetry.minute           = stelemetryInformation.telemetryInfo._min;
            flyControl54Telemetry.second           = stelemetryInformation.telemetryInfo._sec;
            flyControl54Telemetry.millisecond      = 0;
            flyControl54Telemetry.longitude        = stelemetryInformation.telemetryInfo.position.lng;
            flyControl54Telemetry.latitude         = stelemetryInformation.telemetryInfo.position.lat;
            flyControl54Telemetry.satellites       = stelemetryInformation.telemetryInfo.gpsNumber;
            flyControl54Telemetry.altitude         = lround(stelemetryInformation.telemetryInfo.gpsHeigth * 10.0);
            flyControl54Telemetry.groundSpeed      = lround(stelemetryInformation.telemetryInfo.groundVelocity * (10.0 / 36));
            flyControl54Telemetry.airspeed         = lround(stelemetryInformation.telemetryInfo.groundVelocity * 5);
            flyControl54Telemetry.altitudeRelative = lround(stelemetryInformation.telemetryInfo.relativeOriginHeigth * 10);

            aMapPod54Control.telemetryData = flyControl54Telemetry;
        }
    }

    const int start = (AMAPPOD54CONTROL_HEAD_LENGTH + 1);
    const int end   = (AMAPPOD54CONTROL_LENGTH - AMAPPOD54CONTROL_TAIL_LENGTH - 1);
    aMapPod54Control.verify  = xorStr((unsigned char *)(&aMapPod54Control), start, end);
    aMapPod54Control.head[0] = AMAPPOD54CONTROL_FIRST_HEAD;
    aMapPod54Control.head[1] = AMAPPOD54CONTROL_SECOND_HEAD;
    aMapPod54Control.end     = AMAPPOD54CONTROL_SECOND_TAIL;

    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << QString::fromStdString(currentSensorID)  << QString::fromStdString(currentComponentID) << aMapPod54Control;
#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
    if (LY_AerialShotSettingBundle54Service::Get().getNetServerClient())
    {
        LY_AerialShotSettingBundle54Service::Get().NetServerClientSend(aMapPod54Control, m_CurrentSensorID);
    }
    else
#endif
    {
        IAerialShotSettingBundle54Activator::Get().sendMessage((const char *)(&aMapPod54Control), sizeof(AMapPod54Control), currentSensorID, currentComponentID);
    }

    m_FlyControl54TelemetrySend = true;
}

void IAerialShotSettingBundle54Dialog::SendAMapPodControlZeroMsg()
{
    AMapPod54Control aMapPod54ControlZero;
    memset(&aMapPod54ControlZero, 0x00, sizeof(aMapPod54ControlZero));
    SendAMapPodControlMsg(aMapPod54ControlZero);
}

void IAerialShotSettingBundle54Dialog::StopAMapPodGimbalChange(int millisecondsTime)
{
    Q_UNUSED(millisecondsTime)
}

void IAerialShotSettingBundle54Dialog::InitWeaponInfoList()
{
    qnzkna::SystemMaintenance::ISystemMaintenanceService    *pSystemMaintenanceService = IAerialShotSettingBundle54Activator::Get().getSystemMaintenanceService();
    qnzkna::SensorManage::ISensorManageService              *pISensorManageService = IAerialShotSettingBundle54Activator::Get().getPISensorManageService();
    if (!pSystemMaintenanceService || !pISensorManageService)
    {
        return;
    }
    std::set<std::string>  strPlatformList;
    pSystemMaintenanceService->GetWeaponComponentPlatforms(&strPlatformList);
    for (const auto &itrPlatform : strPlatformList)
    {
        std::string strPlatformId = itrPlatform;
        qnzkna::WeaponManage::WeaponComponent weaponInfo;
        if (!pSystemMaintenanceService->GetWeaponComponent(strPlatformId, &weaponInfo))
        {
            continue;
        }
        std::string strSysId;
        unsigned int nIndex = 0;
        if (!qnzkna::WeaponManage::WeaponSystem::DecodeWeaponComponentID(strSysId, nIndex, strPlatformId))
        {
            continue;
        }
        qnzkna::SensorManage::SensorInfoMap tmpSensorInfoOptoelectronicMap;
        qnzkna::SensorManage::SensorInfoMap tmpSensorInfoMap;
        bool bGetSensor = pISensorManageService->GetSensorInfosInWeaponComponent(strPlatformId, &tmpSensorInfoMap);
        if (bGetSensor)
        {
            qnzkna::SensorManage::SensorInfoMap::iterator itrSensor = tmpSensorInfoMap.begin();
            for (; itrSensor != tmpSensorInfoMap.end(); itrSensor ++)
            {
                qnzkna::SensorManage::CSensorInfo &tmpSensorInfo = itrSensor->second;
                std::string typeOptoelectronic("");
                tmpSensorInfo.GetSensorParam("Type", typeOptoelectronic);
                std::transform(typeOptoelectronic.begin(), typeOptoelectronic.end(), typeOptoelectronic.begin(), ::tolower);
                if ("Optoelectronic" == tmpSensorInfo.GetSensorType() && (typeOptoelectronic == "gaode54"))
                {
                    tmpSensorInfoOptoelectronicMap.insert(std::make_pair(tmpSensorInfo.GetSensorID(), tmpSensorInfo));
                }
            }
        }

        for (const auto &tmpSensorInfoOptoelectronic : tmpSensorInfoOptoelectronicMap)
        {
            QString title(QString::fromStdString(weaponInfo.getComponentName()));
            if (tmpSensorInfoOptoelectronicMap.size() > 1)
            {
                title += ":" + QString::fromStdString(tmpSensorInfoOptoelectronic.second.GetSensorName());
            }
            QPushButton *weaponComponentButton = new QPushButton(title, this);
            if (weaponComponentButton == nullptr)
            {
                continue;
            }

            Component_Property_Component54 component_Property_Component54;
            component_Property_Component54.sysID = QString::fromStdString(strSysId).toULong();
            component_Property_Component54.component_ComponentSysID = strSysId;
            component_Property_Component54.component_ComponentName  = weaponInfo.getComponentName();
            component_Property_Component54.component_ComponentID    = weaponInfo.getComponentID();
            component_Property_Component54.component_SensorName     = tmpSensorInfoOptoelectronic.second.GetSensorName();
            component_Property_Component54.component_SensorID       = tmpSensorInfoOptoelectronic.second.GetSensorID();
            m_Component_Property_Component54Map[QString::fromStdString(tmpSensorInfoOptoelectronic.second.GetSensorID())] = component_Property_Component54;

            weaponComponentButton->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed));
            weaponComponentButton->setMinimumSize(QSize(2, 2));
            weaponComponentButton->setCheckable(true);
            weaponComponentButton->setProperty(QPushButton_Property_ComponentSysID, QString::fromStdString(strSysId));
            weaponComponentButton->setProperty(QPushButton_Property_ComponentName, QString::fromStdString(weaponInfo.getComponentName()));
            weaponComponentButton->setProperty(QPushButton_Property_ComponentID, QString::fromStdString(weaponInfo.getComponentID()));
            weaponComponentButton->setProperty(QPushButton_Property_SensorName, QString::fromStdString(tmpSensorInfoOptoelectronic.second.GetSensorName()));
            weaponComponentButton->setProperty(QPushButton_Property_SensorID, QString::fromStdString(tmpSensorInfoOptoelectronic.second.GetSensorID()));
            this->ui->horizontalLayout_Vehicle->insertWidget(this->ui->horizontalLayout_Vehicle->count() - 1, weaponComponentButton);

            if (m_pButtonGroupComponent != nullptr)
            {
                m_pButtonGroupComponent->addButton(weaponComponentButton);
            }

            m_AMapPod54ControlStatusInfoMap.insert(QString::fromStdString(tmpSensorInfoOptoelectronic.second.GetSensorID()), AMapPod54ControlStatusInfo());
        }
    }
    if (m_pButtonGroupComponent->buttons().size() > 0)
    {
        on_Button_54_GroupComponent_buttonClicked(m_pButtonGroupComponent->buttons().at(0));
    }

    VehicleDeflectionDistYawPitchDialog::readVehicleDeflectionDistYawPitchData();
}

void IAerialShotSettingBundle54Dialog::on_Button_54_GroupComponent_buttonClicked(QAbstractButton *button)
{
    if (button == nullptr)
    {
        return ;
    }
    logInfo() << __FILE__ << __FUNCTION__ << " " << __LINE__ << " " << button->property(QPushButton_Property_ComponentName) << button->property(QPushButton_Property_ComponentID);

    const QVariant &variantComponentID   = button->property(QPushButton_Property_ComponentID);
    const QVariant &variantComponentName = button->property(QPushButton_Property_ComponentName);
    const QVariant &variantSensorID      = button->property(QPushButton_Property_SensorID);
    if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
    {
        button->setChecked(false);
        m_CurrentSensorID      = "";
        m_CurrentComponentID   = "";
        m_CurrentComponentName = "";
        return ;
    }

    const QString &selectComponentID   = variantComponentID.toString();
    const QString &selectComponentName = variantComponentName.toString();
    const QString &selectSensorID      = variantSensorID.toString();

    if ((selectComponentID == m_CurrentComponentID) && (selectSensorID == m_CurrentSensorID))
    {
        return ;
    }

    if (m_pButtonCurrentComponent != nullptr)
    {
        m_pButtonCurrentComponent->setChecked(false);
    }

    m_pButtonCurrentComponent = button;

    if (m_pButtonCurrentComponent != nullptr)
    {
        m_pButtonCurrentComponent->setChecked(true);
    }
    m_CurrentComponentID   = selectComponentID;
    m_CurrentComponentName = selectComponentName;
    m_CurrentSensorID      = selectSensorID;

    const auto constFindIter = m_AMapPod54ControlStatusInfoMap.constFind(m_CurrentSensorID);
    if (constFindIter != m_AMapPod54ControlStatusInfoMap.constEnd())
    {

        for (auto &aMapPod54ControlStatusInfoMapValue : m_AMapPod54ControlStatusInfoMap)
        {
            aMapPod54ControlStatusInfoMapValue.selectedCurrentSensor = true;
        }

        m_AMapPod54ControlStatusInfoMap[m_CurrentSensorID].selectedCurrentSensor = true;
        if (m_AMapPod54ControlStatusInfoMap[m_CurrentSensorID].targetBatchStart)
        {
        }
        else
        {
        }

        if (m_AMapPod54ControlStatusInfoMap[m_CurrentSensorID].targetBatchStart)
        {
        }
        else
        {
        }
    }
}

void IAerialShotSettingBundle54Dialog::on_Message_Recv(const QString strSensorID, const QByteArray &message)
{
    if (message.size() <= 0)
    {
        return ;
    }

    if (message.count() != sizeof(AMapPod54ControlStatus))
    {
        return ;
    }

    const auto constFindIter = m_AMapPod54ControlStatusInfoMap.constFind(strSensorID);
    if (constFindIter != m_AMapPod54ControlStatusInfoMap.constEnd())
    {
        {
            const AMapPod54ControlStatus *const aMapPod54ControlStatus = (const AMapPod54ControlStatus * const)(message.constData());

            qDebug() << strSensorID << aMapPod54ControlStatus->receivedCmdFeedback << (aMapPod54ControlStatus->laserDistance * 1.0F / 10.0F);
        }

        auto &aMapPod54ControlStatusInfo   = m_AMapPod54ControlStatusInfoMap[strSensorID];
        if (aMapPod54ControlStatusInfo.laserStarted)
        {

            const AMapPod54ControlStatus *const aMapPod54ControlStatus = (const AMapPod54ControlStatus * const)(message.constData());

            if ((aMapPod54ControlStatus->receivedCmdFeedback == static_cast<uint8_t>(AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_SingleLaserTelemeter))
                    || (aMapPod54ControlStatus->receivedCmdFeedback == static_cast<uint8_t>(AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StartContinueLaserTelemeter))
               )
            {
                aMapPod54ControlStatusInfo.receivedCmdFeedback = aMapPod54ControlStatus->receivedCmdFeedback;
                aMapPod54ControlStatusInfo.laserDistanceTimes = ::time(nullptr);
            }

            if ((aMapPod54ControlStatusInfo.receivedCmdFeedback == static_cast<uint8_t>(AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_SingleLaserTelemeter))
                    || (aMapPod54ControlStatusInfo.receivedCmdFeedback == static_cast<uint8_t>(AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StartContinueLaserTelemeter))
               )
            {
                qDebug() << strSensorID << aMapPod54ControlStatusInfo.receivedCmdFeedback << (aMapPod54ControlStatus->laserDistance * 1.0F / 10.0F);

                aMapPod54ControlStatusInfo.receivedLaserDistanceCount++;
                if (aMapPod54ControlStatusInfo.receivedLaserDistanceCount > 60)
                {
                    aMapPod54ControlStatusInfo.receivedCmdFeedback = 0;
                    aMapPod54ControlStatusInfo.receivedLaserDistanceCount = 0;
                }

                bool &targetBatch                  = aMapPod54ControlStatusInfo.targetBatch;
                bool &laserDistanceContinueStarted = aMapPod54ControlStatusInfo.laserDistanceContinueStarted;
                bool &laserDistanceStarted         = aMapPod54ControlStatusInfo.laserDistanceStarted;

                aMapPod54ControlStatusInfo.aMapPod54ControlStatus = *aMapPod54ControlStatus;

                const float laserDistance = aMapPod54ControlStatus->laserDistance * 1.0F / 10.0F;
                if ((aMapPod54ControlStatusInfo.receivedLaserDistanceCount > 4) && (laserDistanceContinueStarted || (laserDistanceStarted)) && laserDistance > 0.0F && laserDistance <= 30000.0F && IAerialShotSettingBundle54Activator::Get().getIRegistryImpl())
                {
                    if (::time(nullptr) - aMapPod54ControlStatusInfo.laserDistanceTimes > 10)
                    {
                        aMapPod54ControlStatusInfo.laserStarted                 = false;
                        aMapPod54ControlStatusInfo.laserDistanceStarted         = false;
                        aMapPod54ControlStatusInfo.laserDistanceContinueStarted = false;
                        aMapPod54ControlStatusInfo.laserDistanceTimes           = 0;
                    }

                    aMapPod54ControlStatusInfo.targetLongitude = aMapPod54ControlStatus->targetLongitude;
                    aMapPod54ControlStatusInfo.targetLatitude  = aMapPod54ControlStatus->targetLatitude;
                    aMapPod54ControlStatusInfo.targetAltitude  = aMapPod54ControlStatus->targetAltitude;

                    QString strComponentID = GetComponentID(strSensorID);
                    if (!strComponentID.isEmpty())
                    {

                        xygsZC_01 sxygsZC_01;
                        sxygsZC_01.sAzimuth  = aMapPod54ControlStatus->podAttitudeYaw * 1.0F / 100.0F;
                        sxygsZC_01.sPitching = aMapPod54ControlStatus->podAttitudePitch * 1.0F / 100.0F;
                        sxygsZC_01.sDistance = 0;
                        sxygsZC_01.useful = false;

                        {
                            sxygsZC_01.sDistance = laserDistance;
                            sxygsZC_01.useful = true;

                            if (aMapPod54ControlStatusInfo.Z == 0)
                            {
                                aMapPod54ControlStatusInfo.Z = m_Component_Property_Component54Map[strSensorID].sysID;
                            }

                            if (targetBatch)
                            {
                                aMapPod54ControlStatusInfo.P++;
                                if (aMapPod54ControlStatusInfo.P >= 255)
                                {
                                    aMapPod54ControlStatusInfo.P = 0;
                                }
                                aMapPod54ControlStatusInfo.targetBatchTime = QDateTime::currentMSecsSinceEpoch();
                            }

                            XygsZC_Mark_01 sxygsZC_Target_01;
                            sxygsZC_Target_01.Z                = aMapPod54ControlStatusInfo.Z;
                            sxygsZC_Target_01.P                = aMapPod54ControlStatusInfo.P;
                            sxygsZC_Target_01.componentID      = strComponentID.toULongLong();
                            sxygsZC_Target_01.vehiclePitch     = 0;
                            sxygsZC_Target_01.vehicleRoll      = 0;
                            sxygsZC_Target_01.vehicleYaw       = 0;
                            sxygsZC_Target_01.vehicleLon       = 0;
                            sxygsZC_Target_01.vehicleLat       = 0;
                            sxygsZC_Target_01.vehicleAlt       = 0;
                            sxygsZC_Target_01.useVehicle       = false;
                            sxygsZC_Target_01.sensorID         = strSensorID.toULongLong();
                            sxygsZC_Target_01.sensorPitch      = aMapPod54ControlStatus->podAttitudePitch * 1.0F / 100.0F;
                            sxygsZC_Target_01.sensorRoll       = aMapPod54ControlStatus->podAttitudeRoll * 1.0F / 100.0F;
                            sxygsZC_Target_01.sensorYaw        = aMapPod54ControlStatus->podAttitudeYaw * 1.0F / 100.0F;
                            sxygsZC_Target_01.laserDistance    = laserDistance;
                            sxygsZC_Target_01.useSensor        = true;
                            sxygsZC_Target_01.target_lon       = aMapPod54ControlStatus->targetLongitude;
                            sxygsZC_Target_01.target_lat       = aMapPod54ControlStatus->targetLatitude;
                            sxygsZC_Target_01.target_alt       = aMapPod54ControlStatus->targetAltitude;
                            sxygsZC_Target_01.useful           = true;
                            sxygsZC_Target_01.usePos           = true;
                            sxygsZC_Target_01.newBatch         = true;
                            sxygsZC_Target_01.newBatch         = targetBatch;
                            sxygsZC_Target_01.targetTime       = aMapPod54ControlStatusInfo.targetBatchTime;
                            sxygsZC_Target_01.targetID         = 0;
                            targetBatch                        = false;

                            auto pIUAVRealTimeStateManageService = IAerialShotSettingBundle54Activator::Get().getPIUAVRealTimeStateManageService();

                            if (pIUAVRealTimeStateManageService != nullptr)
                            {
                                qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stelemetryInformation;
                                bool result = pIUAVRealTimeStateManageService->GetUAVRealTimeState(strComponentID.toStdString(), stelemetryInformation);
                                if (result)
                                {
                                    sxygsZC_Target_01.useVehicle   = true;
                                    sxygsZC_Target_01.vehiclePitch = stelemetryInformation.telemetryInfo.angleofPitch;
                                    sxygsZC_Target_01.vehicleRoll  = stelemetryInformation.telemetryInfo.angleofRoll;
                                    sxygsZC_Target_01.vehicleYaw   = stelemetryInformation.telemetryInfo.angleofNose;
                                    sxygsZC_Target_01.vehicleLon   = stelemetryInformation.telemetryInfo.position.lng;
                                    sxygsZC_Target_01.vehicleLat   = stelemetryInformation.telemetryInfo.position.lat;
                                    sxygsZC_Target_01.vehicleAlt   = lround(stelemetryInformation.telemetryInfo.gpsHeigth * 10.0);
                                }
                            }

                            qnzkna::framework::MessageReferenceProps props;
                            props.put("TargetType",   std::to_string(NODE_TYPE_SENSOR));
                            props.put("TargetID",     strSensorID.toStdString());
                            props.put("WeaponID",     strComponentID.toStdString());

                            IAerialShotSettingBundle54Activator::Get().getIRegistryImpl()->messageNotify(
                                new qnzkna::framework::MessageReference("MsgZCTargetPosition", (char *)&sxygsZC_Target_01, sizeof(sxygsZC_Target_01), props));

                        }

                    }
                }
            }
            else
            {
                if (::time(nullptr) - aMapPod54ControlStatusInfo.laserDistanceTimes > 10)
                {

                    if (aMapPod54ControlStatusInfo.laserDistanceStarted)
                    {

                        aMapPod54ControlStatusInfo.receivedCmdFeedback = 0;
                        aMapPod54ControlStatusInfo.receivedLaserDistanceCount = 0;
                        aMapPod54ControlStatusInfo.laserStarted = false;
                        aMapPod54ControlStatusInfo.laserDistanceStarted         = false;
                        aMapPod54ControlStatusInfo.laserDistanceContinueStarted = false;
                        aMapPod54ControlStatusInfo.laserDistanceTimes           = 0;
                    }

                    if (aMapPod54ControlStatusInfo.laserDistanceContinueStarted)
                    {
                        if (::time(nullptr) - aMapPod54ControlStatusInfo.laserDistanceTimes > 20)
                        {

                            aMapPod54ControlStatusInfo.receivedCmdFeedback = 0;
                            aMapPod54ControlStatusInfo.receivedLaserDistanceCount = 0;
                            aMapPod54ControlStatusInfo.laserStarted                 = false;
                            aMapPod54ControlStatusInfo.laserDistanceStarted         = false;
                            aMapPod54ControlStatusInfo.laserDistanceContinueStarted = false;
                            aMapPod54ControlStatusInfo.laserDistanceTimes           = 0;
                        }
                    }

                    if (::time(nullptr) - aMapPod54ControlStatusInfo.laserDistanceTimes > 30)
                    {

                        aMapPod54ControlStatusInfo.receivedCmdFeedback = 0;
                        aMapPod54ControlStatusInfo.receivedLaserDistanceCount = 0;
                        aMapPod54ControlStatusInfo.laserStarted                 = false;
                        aMapPod54ControlStatusInfo.laserDistanceStarted         = false;
                        aMapPod54ControlStatusInfo.laserDistanceContinueStarted = false;
                        aMapPod54ControlStatusInfo.laserDistanceTimes           = 0;
                    }
                }
            }
        }

        if (!m_WidgetShow)
        {
            return ;
        }

        if (constFindIter->selectedCurrentSensor)
        {
            emit ShowAMapPodStatusInfoSig(strSensorID, message);
        }
    }
}

const QString IAerialShotSettingBundle54Dialog::GetComponentID(const QString &strSensorID)
{
    QString strComponentID("");
    for (const auto &button : m_pButtonGroupComponent->buttons())
    {
        const QVariant &variantComponentID = button->property(QPushButton_Property_ComponentID);
        const QVariant &variantSensorID    = button->property(QPushButton_Property_SensorID);
        if ((!variantComponentID.isValid()) || (!variantSensorID.isValid()))
        {
            continue ;
        }

        const QString &strComponentIDTemp = variantComponentID.toString();
        const QString &strSensorIDTemp    = variantSensorID.toString();

        if (strSensorID == strSensorIDTemp)
        {
            strComponentID = strComponentIDTemp;
            break;
        }
    }

    return strComponentID;
}

const std::map<std::string, std::vector<std::pair<unsigned int, std::list<std::pair<unsigned int, std::pair<double, double> > > > > > &IAerialShotSettingBundle54Dialog::getVehicleDeflectionDistYawPitch54Map() const
{
    return m_VehicleDeflectionDistYawPitch54Map;
}

void IAerialShotSettingBundle54Dialog::setVehicleDeflectionDistYawPitch54Map(const std::string &strVehicleID, const std::vector<std::pair<unsigned int, std::list<std::pair<unsigned int, std::pair<double, double> > > > > &vehicleDeflectionDistYawPitch54Map)
{
    if (strVehicleID.empty())
    {
        return ;
    }

    const auto iter = m_VehicleDeflectionDistYawPitch54Map.find(strVehicleID);
    if (iter != m_VehicleDeflectionDistYawPitch54Map.end())
    {
        iter->second = vehicleDeflectionDistYawPitch54Map;
    }
    else
    {
        m_VehicleDeflectionDistYawPitch54Map.insert(std::pair <std::string, std::vector< std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > > > >(strVehicleID, vehicleDeflectionDistYawPitch54Map));
    }
}

void IAerialShotSettingBundle54Dialog::getVehicleDeflectionDistYawPitch54Value(const std::string &strVehicleID)
{
    if (m_LightElectronicZoom != m_AMapPod54StatusInfo.pod2Status.lightElectronicZoom || m_LaserDistance != m_AMapPod54StatusInfo.laserDistance)
    {
        m_LightElectronicZoom    = m_AMapPod54StatusInfo.pod2Status.lightElectronicZoom;
        m_LaserDistance          = m_AMapPod54StatusInfo.laserDistance;
        const auto &VehicleDeflectionAngleofNoseMap = getVehicleDeflectionDistYawPitch54Map();
        std::map<std::string, std::vector<std::pair<unsigned int, std::list<std::pair<unsigned int, std::pair<double, double> > > > > >::const_iterator iter = VehicleDeflectionAngleofNoseMap.find(strVehicleID);
        if (iter != VehicleDeflectionAngleofNoseMap.end())
        {
            unsigned int deflectionMin      = 100000;
            int deflectionMinIndex = -1;
            for (const auto &deflectionDistYawPitch : iter->second)
            {
                if (std::labs(deflectionDistYawPitch.first - m_LightElectronicZoom) < deflectionMin)
                {
                    deflectionMin = std::labs(deflectionDistYawPitch.first - m_LightElectronicZoom);
                    deflectionMinIndex++;
                }
            }

            std::pair<unsigned int, std::pair<double, double> > deflectionDistYawPitchMin{0, {0, 0}};
            if (deflectionMinIndex > 0)
            {
                unsigned int deflectionDistMin      = 100000;
                for (const auto &deflectionDistYawPitch : iter->second.at(deflectionMinIndex).second)
                {
                    if (std::labs(deflectionDistYawPitch.first - m_LaserDistance) < deflectionDistMin)
                    {
                        deflectionDistMin = std::labs(deflectionDistYawPitch.first - m_LaserDistance);
                        deflectionDistYawPitchMin = deflectionDistYawPitch;
                    }
                }
            }

            m_DeflectionAngleofNose  = deflectionDistYawPitchMin.second.first;
            m_DeflectionAngleofPitch = deflectionDistYawPitchMin.second.second;
        }
    }
}

void IAerialShotSettingBundle54Dialog::getVehicleDeflectionDistYawPitch54ZoomDistanceValue(const std::string &strVehicleID, const QString &strSensorID, float &deflectionAngleofNose, float &deflectionAngleofPitch)
{
    const auto constFindIter = m_AMapPod54ControlStatusInfoMap.constFind(strSensorID);
    if (constFindIter != m_AMapPod54ControlStatusInfoMap.constEnd())
    {

        const unsigned int &tempLightElectronicZoom = constFindIter.value().aMapPod54ControlStatus.pod2Status.lightElectronicZoom;
        const unsigned int &tempLaserDistance = constFindIter.value().aMapPod54ControlStatus.laserDistance;

        std::map<std::string, std::pair<unsigned int, unsigned int> >::iterator iterZoomDistance = m_ZoomDistanceMap.find(strVehicleID);
        if (iterZoomDistance != m_ZoomDistanceMap.end())
        {
            if (iterZoomDistance->second.first != tempLightElectronicZoom || iterZoomDistance->second.second != tempLaserDistance)
            {
                const auto &VehicleDeflectionAngleofNoseMap = getVehicleDeflectionDistYawPitch54Map();
                std::map<std::string, std::vector<std::pair<unsigned int, std::list<std::pair<unsigned int, std::pair<double, double> > > > > >::const_iterator iter = VehicleDeflectionAngleofNoseMap.find(strVehicleID);
                if (iter != VehicleDeflectionAngleofNoseMap.end())
                {
                    unsigned int deflectionMin      = 100000;
                    int deflectionMinIndex = -1;
                    for (const auto &deflectionDistYawPitch : iter->second)
                    {
                        if (std::labs(deflectionDistYawPitch.first - tempLightElectronicZoom) < deflectionMin)
                        {
                            deflectionMin = std::labs(deflectionDistYawPitch.first - tempLightElectronicZoom);
                            deflectionMinIndex++;
                        }
                    }

                    std::pair<unsigned int, std::pair<double, double> > deflectionDistYawPitchMin{0, {0, 0}};
                    if (deflectionMinIndex > 0)
                    {
                        unsigned int deflectionDistMin      = 100000;
                        for (const auto &deflectionDistYawPitch : iter->second.at(deflectionMinIndex).second)
                        {
                            if (std::labs(deflectionDistYawPitch.first - tempLaserDistance) < deflectionDistMin)
                            {
                                deflectionDistMin = std::labs(deflectionDistYawPitch.first - tempLaserDistance);
                                deflectionDistYawPitchMin = deflectionDistYawPitch;
                            }
                        }
                    }

                    iterZoomDistance->second.first   = deflectionDistYawPitchMin.second.first;
                    iterZoomDistance->second.second  = deflectionDistYawPitchMin.second.second;

                    deflectionAngleofNose  = deflectionDistYawPitchMin.second.first;
                    deflectionAngleofPitch = deflectionDistYawPitchMin.second.second;
                }
            }
        }
        else
        {
            const auto &VehicleDeflectionAngleofNoseMap = getVehicleDeflectionDistYawPitch54Map();
            std::map<std::string, std::vector<std::pair<unsigned int, std::list<std::pair<unsigned int, std::pair<double, double> > > > > >::const_iterator iter = VehicleDeflectionAngleofNoseMap.find(strVehicleID);
            if (iter != VehicleDeflectionAngleofNoseMap.end())
            {
                unsigned int deflectionMin      = 100000;
                int deflectionMinIndex = -1;
                for (const auto &deflectionDistYawPitch : iter->second)
                {
                    if (std::labs(deflectionDistYawPitch.first - tempLightElectronicZoom) < deflectionMin)
                    {
                        deflectionMin = std::labs(deflectionDistYawPitch.first - tempLightElectronicZoom);
                        deflectionMinIndex++;
                    }
                }

                std::pair<unsigned int, std::pair<double, double> > deflectionDistYawPitchMin{0, {0, 0}};
                if (deflectionMinIndex > 0)
                {
                    unsigned int deflectionDistMin      = 100000;
                    for (const auto &deflectionDistYawPitch : iter->second.at(deflectionMinIndex).second)
                    {
                        if (std::labs(deflectionDistYawPitch.first - tempLaserDistance) < deflectionDistMin)
                        {
                            deflectionDistMin = std::labs(deflectionDistYawPitch.first - tempLaserDistance);
                            deflectionDistYawPitchMin = deflectionDistYawPitch;
                        }
                    }
                }

                m_ZoomDistanceMap.insert(std::pair<std::string, std::pair<unsigned int, unsigned int> > (strVehicleID, std::pair<unsigned int, unsigned int>(deflectionDistYawPitchMin.second.first, deflectionDistYawPitchMin.second.second)));

                deflectionAngleofNose  = deflectionDistYawPitchMin.second.first;
                deflectionAngleofPitch = deflectionDistYawPitchMin.second.second;
            }
        }
    }
}

void IAerialShotSettingBundle54Dialog::CommadAMapPodControlMsg(AMapPod54Control &aMapPod54Control, const QString strComponentID, const QString strSensorID)
{
}

void IAerialShotSettingBundle54Dialog::ShowAMapPodStatusInfo(const QString strSensorID, const QByteArray message)
{

    if (ui == nullptr)
    {
        return ;
    }

    if (message.size() <= 0)
    {
        return ;
    }

    if (m_CurrentComponentID.isEmpty() || m_CurrentSensorID.isEmpty())
    {
        return ;
    }

    if (m_CurrentSensorID != strSensorID)
    {
        return ;
    }

    if (message.count() != sizeof(AMapPod54ControlStatus))
    {
        return ;
    }

    const AMapPod54ControlStatus aMapPod54ControlStatus = *((const AMapPod54ControlStatus *)(message.constData()));

    if (m_Widget_54_All_Function)
    {
        if (m_AMapPod54StatusInfo.payloadType != aMapPod54ControlStatus.payloadType)
        {
            m_AMapPod54StatusInfo.payloadType = aMapPod54ControlStatus.payloadType;
            ui->label_54_Status_PalyloadType->setText(AMapPod54ControlStatus_PayloadType_ToStr(aMapPod54ControlStatus.payloadType));
        }

        if (m_AMapPod54StatusInfo.autoCheckResult.infraredMovement != aMapPod54ControlStatus.autoCheckResult.infraredMovement)
        {
            m_AMapPod54StatusInfo.autoCheckResult.infraredMovement = aMapPod54ControlStatus.autoCheckResult.infraredMovement;
            ui->label_54_Status_InfraredCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(aMapPod54ControlStatus.autoCheckResult.infraredMovement));
        }

        if (m_AMapPod54StatusInfo.autoCheckResult.lightMovement != aMapPod54ControlStatus.autoCheckResult.lightMovement)
        {
            m_AMapPod54StatusInfo.autoCheckResult.lightMovement = aMapPod54ControlStatus.autoCheckResult.lightMovement;
            ui->label_54_Status_LightCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(aMapPod54ControlStatus.autoCheckResult.lightMovement));
        }

        if (m_AMapPod54StatusInfo.autoCheckResult.gyroscopeData != aMapPod54ControlStatus.autoCheckResult.gyroscopeData)
        {
            m_AMapPod54StatusInfo.autoCheckResult.gyroscopeData = aMapPod54ControlStatus.autoCheckResult.gyroscopeData;
            ui->label_54_Status_GyroCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(aMapPod54ControlStatus.autoCheckResult.gyroscopeData));
        }

        if (m_AMapPod54StatusInfo.autoCheckResult.angleSensor != aMapPod54ControlStatus.autoCheckResult.angleSensor)
        {
            m_AMapPod54StatusInfo.autoCheckResult.angleSensor = aMapPod54ControlStatus.autoCheckResult.angleSensor;
            ui->label_54_Status_AngleSensorCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(aMapPod54ControlStatus.autoCheckResult.angleSensor));
        }

        if (m_AMapPod54StatusInfo.autoCheckResult.driveBoard != aMapPod54ControlStatus.autoCheckResult.driveBoard)
        {
            m_AMapPod54StatusInfo.autoCheckResult.driveBoard = aMapPod54ControlStatus.autoCheckResult.driveBoard;
            ui->label_54_Status_MotorCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(aMapPod54ControlStatus.autoCheckResult.driveBoard));
        }

        if (m_AMapPod54StatusInfo.autoCheckResult.compressStorage != aMapPod54ControlStatus.autoCheckResult.compressStorage)
        {
            m_AMapPod54StatusInfo.autoCheckResult.compressStorage = aMapPod54ControlStatus.autoCheckResult.compressStorage;
            ui->label_54_Status_CompressCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(aMapPod54ControlStatus.autoCheckResult.compressStorage));
        }

        if (m_AMapPod54StatusInfo.autoCheckResult.comprehensiveProcess != aMapPod54ControlStatus.autoCheckResult.comprehensiveProcess)
        {
            m_AMapPod54StatusInfo.autoCheckResult.comprehensiveProcess = aMapPod54ControlStatus.autoCheckResult.comprehensiveProcess;
            ui->label_54_Status_CompositeHandCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(aMapPod54ControlStatus.autoCheckResult.comprehensiveProcess));
        }

        if (m_AMapPod54StatusInfo.autoCheckResult.podReadyStatus != aMapPod54ControlStatus.autoCheckResult.podReadyStatus)
        {
            m_AMapPod54StatusInfo.autoCheckResult.podReadyStatus = aMapPod54ControlStatus.autoCheckResult.podReadyStatus;
            ui->label_54_Status_PrepareCheck->setText(AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(aMapPod54ControlStatus.autoCheckResult.podReadyStatus));
        }

        if (m_AMapPod54StatusInfo.pod1Status.photographStatus != aMapPod54ControlStatus.pod1Status.photographStatus)
        {
            m_AMapPod54StatusInfo.pod1Status.photographStatus = aMapPod54ControlStatus.pod1Status.photographStatus;
            ui->label_54_Status_TakePicture->setText((aMapPod54ControlStatus.pod1Status.photographStatus ? tr("Taking pictures") : tr("Not photographed")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.videoCapture != aMapPod54ControlStatus.pod1Status.videoCapture)
        {
            m_AMapPod54StatusInfo.pod1Status.videoCapture = aMapPod54ControlStatus.pod1Status.videoCapture;
            ui->label_54_Status_CapPicture->setText((aMapPod54ControlStatus.pod1Status.videoCapture ? tr("Video capture") : tr("No video capture")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.burstStatus != aMapPod54ControlStatus.pod1Status.burstStatus)
        {
            m_AMapPod54StatusInfo.pod1Status.burstStatus = aMapPod54ControlStatus.pod1Status.burstStatus;
            ui->label_54_Status_ContinueTakePicture->setText((aMapPod54ControlStatus.pod1Status.burstStatus ? tr("In continuous shooting") : tr("No continuous shot")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.videoBurstCapture != aMapPod54ControlStatus.pod1Status.videoBurstCapture)
        {
            m_AMapPod54StatusInfo.pod1Status.videoBurstCapture = aMapPod54ControlStatus.pod1Status.videoBurstCapture;
            ui->label_54_Status_ContinueCapPicture->setText((aMapPod54ControlStatus.pod1Status.videoBurstCapture ? tr("Continuous video capture") : tr("Continuous capture without video")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.recordingStatus != aMapPod54ControlStatus.pod1Status.recordingStatus)
        {
            m_AMapPod54StatusInfo.pod1Status.recordingStatus = aMapPod54ControlStatus.pod1Status.recordingStatus;
            ui->label_54_Status_RecodeVideo->setText((aMapPod54ControlStatus.pod1Status.recordingStatus ? tr("Recording") : tr("The video was not started")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.fileRenameImportComplete != aMapPod54ControlStatus.pod1Status.fileRenameImportComplete)
        {
            m_AMapPod54StatusInfo.pod1Status.fileRenameImportComplete = aMapPod54ControlStatus.pod1Status.fileRenameImportComplete;
            ui->label_54_Status_FileRename->setText((aMapPod54ControlStatus.pod1Status.fileRenameImportComplete ? tr("The file naming information is imported") : tr("Failed to import")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.TFCardInsert != aMapPod54ControlStatus.pod1Status.TFCardInsert)
        {
            m_AMapPod54StatusInfo.pod1Status.TFCardInsert = aMapPod54ControlStatus.pod1Status.TFCardInsert;
            ui->label_54_Status_LightTFStatus->setText((aMapPod54ControlStatus.pod1Status.TFCardInsert ? tr("TF card inserted") : tr("TF card is not inserted")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.networkConnection != aMapPod54ControlStatus.pod1Status.networkConnection)
        {
            m_AMapPod54StatusInfo.pod1Status.networkConnection = aMapPod54ControlStatus.pod1Status.networkConnection;
            ui->label_54_Status_LightNetLinkStatus->setText((aMapPod54ControlStatus.pod1Status.networkConnection ? tr("Network connection is fine") : tr("Network connection is abnormal")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.TFCardOperation != aMapPod54ControlStatus.pod1Status.TFCardOperation)
        {
            m_AMapPod54StatusInfo.pod1Status.TFCardOperation = aMapPod54ControlStatus.pod1Status.TFCardOperation;
            ui->label_54_Status_LightTFOperatorStatus->setText((aMapPod54ControlStatus.pod1Status.TFCardOperation ? tr("TF card operates normally") : tr("TF card operates abnormally")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.infraredImageEnhancement != aMapPod54ControlStatus.pod1Status.infraredImageEnhancement)
        {
            m_AMapPod54StatusInfo.pod1Status.infraredImageEnhancement = aMapPod54ControlStatus.pod1Status.infraredImageEnhancement;
            ui->label_54_Status_LightShowPictureRespond->setText(QString::number(aMapPod54ControlStatus.pod1Status.infraredImageEnhancement));
        }

        if (m_AMapPod54StatusInfo.pod2Status.videoOutput != aMapPod54ControlStatus.pod2Status.videoOutput)
        {
            m_AMapPod54StatusInfo.pod2Status.videoOutput = aMapPod54ControlStatus.pod2Status.videoOutput;
            ui->label_54_Status_VideoOutput->setText((aMapPod54ControlStatus.pod2Status.videoOutput ? tr("Visible light video output") : tr("Infrared video output")));
        }

        if (m_AMapPod54StatusInfo.pod2Status.traceSource != aMapPod54ControlStatus.pod2Status.traceSource)
        {
            m_AMapPod54StatusInfo.pod2Status.traceSource = aMapPod54ControlStatus.pod2Status.traceSource;
            ui->label_54_Status_TrackingSource->setText((aMapPod54ControlStatus.pod2Status.traceSource ? tr("Visible light") : tr("Infrared")));
        }

        if (m_AMapPod54StatusInfo.pod2Status.infraredElectronicZoom != aMapPod54ControlStatus.pod2Status.infraredElectronicZoom)
        {
            m_AMapPod54StatusInfo.pod2Status.infraredElectronicZoom = aMapPod54ControlStatus.pod2Status.infraredElectronicZoom;
            ui->label_54_Status_InfraredOut->setText(QString::number(aMapPod54ControlStatus.pod2Status.infraredElectronicZoom * 10));
        }

        if (m_AMapPod54StatusInfo.pod2Status.lightElectronicZoom != aMapPod54ControlStatus.pod2Status.lightElectronicZoom)
        {
            m_AMapPod54StatusInfo.pod2Status.lightElectronicZoom = aMapPod54ControlStatus.pod2Status.lightElectronicZoom;
            ui->label_54_Status_LightOut->setText(QString::number(std::pow(2, aMapPod54ControlStatus.pod2Status.lightElectronicZoom)));
        }

        if (m_AMapPod54StatusInfo.pod2Status.ribbon != aMapPod54ControlStatus.pod2Status.ribbon)
        {
            m_AMapPod54StatusInfo.pod2Status.ribbon = aMapPod54ControlStatus.pod2Status.ribbon;
            ui->label_54_Status_Ribbon->setText(AMapPod54Control_FalseColorBandModel_ToStr(aMapPod54ControlStatus.pod2Status.ribbon - 1));
        }

        if (m_AMapPod54StatusInfo.servoStatus != aMapPod54ControlStatus.servoStatus)
        {
            m_AMapPod54StatusInfo.servoStatus = aMapPod54ControlStatus.servoStatus;
            ui->label_54_Status_MotorStatus->setText(AMapPod54ControlStatus_ServoStatus_ToStr(aMapPod54ControlStatus.servoStatus));
        }

        {
            m_AMapPod54StatusInfo.podFrameworkYaw = aMapPod54ControlStatus.podFrameworkYaw;
            ui->label_54_Status_GimbalYaw->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podFrameworkYaw / 100.0)));
        }

        {
            m_AMapPod54StatusInfo.podFrameworkPitch = aMapPod54ControlStatus.podFrameworkPitch;
            ui->label_54_Status_GimbalPitch->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podFrameworkPitch / 100.0)));
        }

        {
            m_AMapPod54StatusInfo.podFrameworkRoll = aMapPod54ControlStatus.podFrameworkRoll;
            ui->label_54_Status_GimbalRoll->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podFrameworkRoll / 100.0)));
        }

        if ((m_AMapPod54StatusInfo.payloadVersion.payloadVersionB != aMapPod54ControlStatus.payloadVersion.payloadVersionB)
                || (m_AMapPod54StatusInfo.payloadVersion.payloadVersionA != aMapPod54ControlStatus.payloadVersion.payloadVersionA))
        {
            m_AMapPod54StatusInfo.payloadVersion.payloadVersionB = aMapPod54ControlStatus.payloadVersion.payloadVersionB;
            m_AMapPod54StatusInfo.payloadVersion.payloadVersionA = aMapPod54ControlStatus.payloadVersion.payloadVersionA;
            ui->label_54_Status_ABVersion->setText(QString::asprintf("A:%u,B:%u", m_AMapPod54StatusInfo.payloadVersion.payloadVersionA, m_AMapPod54StatusInfo.payloadVersion.payloadVersionB));
        }

        if (m_AMapPod54StatusInfo.infraredViewAngle != aMapPod54ControlStatus.infraredViewAngle)
        {
            m_AMapPod54StatusInfo.infraredViewAngle = aMapPod54ControlStatus.infraredViewAngle;
            ui->label_54_Status_InfraredViewAngle->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.infraredViewAngle / 10.0)));
        }

        if (m_AMapPod54StatusInfo.tfPercentageOfCapacityUsed != aMapPod54ControlStatus.tfPercentageOfCapacityUsed)
        {
            m_AMapPod54StatusInfo.tfPercentageOfCapacityUsed = aMapPod54ControlStatus.tfPercentageOfCapacityUsed;
            ui->label_54_Status_TFRemainPer->setText(QString::asprintf("%u%%", aMapPod54ControlStatus.tfPercentageOfCapacityUsed));
        }

        if (m_AMapPod54StatusInfo.lightViewAngle != aMapPod54ControlStatus.lightViewAngle)
        {
            m_AMapPod54StatusInfo.lightViewAngle = aMapPod54ControlStatus.lightViewAngle;
            ui->label_54_Status_LigthViewAngle->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.lightViewAngle / 10.0)));
        }

        if (m_AMapPod54StatusInfo.tfTotalCapacity != aMapPod54ControlStatus.tfTotalCapacity)
        {
            m_AMapPod54StatusInfo.tfTotalCapacity = aMapPod54ControlStatus.tfTotalCapacity;
            ui->label_54_Status_TFCapacity->setText(QString::asprintf("%.1fG", (aMapPod54ControlStatus.tfTotalCapacity / 10.0)));
        }

        if (m_AMapPod54StatusInfo.infraredFocalLength != aMapPod54ControlStatus.infraredFocalLength)
        {
            m_AMapPod54StatusInfo.infraredFocalLength = aMapPod54ControlStatus.infraredFocalLength;
            ui->label_54_Status_InfraredFocus->setText(QString::asprintf("%.1fmm", (aMapPod54ControlStatus.infraredFocalLength / 10.0)));
        }

        if (m_AMapPod54StatusInfo.lightFocalLength != aMapPod54ControlStatus.lightFocalLength)
        {
            m_AMapPod54StatusInfo.lightFocalLength = aMapPod54ControlStatus.lightFocalLength;
            ui->label_54_Status_LightFocus->setText(QString::asprintf("%.1fmm", (aMapPod54ControlStatus.lightFocalLength / 10.0)));
        }

        if (m_AMapPod54StatusInfo.laserDistance != aMapPod54ControlStatus.laserDistance)
        {
            m_AMapPod54StatusInfo.laserDistance = aMapPod54ControlStatus.laserDistance;
            ui->label_54_Status_TelemeterDistanceValue->setText(QString::asprintf("%.1fm", (aMapPod54ControlStatus.laserDistance / 10.0)));
        }

        {
            m_AMapPod54StatusInfo.targetLongitude = aMapPod54ControlStatus.targetLongitude;
            ui->label_54_Status_TargetLongitude->setText(QString::asprintf("%.6f", (aMapPod54ControlStatus.targetLongitude)));
        }

        {
            m_AMapPod54StatusInfo.targetLatitude = aMapPod54ControlStatus.targetLatitude;
            ui->label_54_Status_TargetLatitude->setText(QString::asprintf("%.6f", (aMapPod54ControlStatus.targetLatitude)));
        }

        {
            m_AMapPod54StatusInfo.targetAltitude = aMapPod54ControlStatus.targetAltitude;
            ui->label_54_Status_TargetAltitude->setText(QString::asprintf("%.2fm", (aMapPod54ControlStatus.targetAltitude / 10.0)));
        }

        {
            m_AMapPod54StatusInfo.temperature = aMapPod54ControlStatus.temperature;
            ui->label_54_Status_Temperature->setText(QString::asprintf(u8"%.1f°", (aMapPod54ControlStatus.temperature / 10.0)));
        }

        {
            m_AMapPod54StatusInfo.podAttitudeYaw = aMapPod54ControlStatus.podAttitudeYaw;
            ui->label_54_Status_GimbalAttitudeYaw->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podAttitudeYaw / 100.0)));
        }

        {
            m_AMapPod54StatusInfo.podAttitudePitch = aMapPod54ControlStatus.podAttitudePitch;
            ui->label_54_Status_GimbalAttitudePitch->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podAttitudePitch / 100.0)));
        }

        {
            m_AMapPod54StatusInfo.podAttitudeRoll = aMapPod54ControlStatus.podAttitudeRoll;
            ui->label_54_Status_GimbalAttitudeRoll->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podAttitudeRoll / 100.0)));
        }

        if (m_AMapPod54StatusInfo.currentDisplayImageFeedback != aMapPod54ControlStatus.currentDisplayImageFeedback)
        {
            m_AMapPod54StatusInfo.currentDisplayImageFeedback = aMapPod54ControlStatus.currentDisplayImageFeedback;
            ui->label_54_Status_LightShowPictureRespond->setText(AMapPod54ControlStatus_CurrentDisplayImageFeedback_ToStr(aMapPod54ControlStatus.currentDisplayImageFeedback));
        }

        {
            m_AMapPod54StatusInfo.receivedCmdFeedback = aMapPod54ControlStatus.receivedCmdFeedback;
            ui->label_54_Status_RecvCmdRespond->setText(QString::asprintf("%u", aMapPod54ControlStatus.receivedCmdFeedback));
        }

        if (m_AMapPod54StatusInfo.attitudeIndicationFeedback != aMapPod54ControlStatus.attitudeIndicationFeedback)
        {
            m_AMapPod54StatusInfo.attitudeIndicationFeedback = aMapPod54ControlStatus.attitudeIndicationFeedback;
            ui->label_54_Status_AttitudeIndicateComplageRespond->setText(AMapPod54ControlStatus_AttitudeIndicationFeedback_ToStr(aMapPod54ControlStatus.attitudeIndicationFeedback));
        }

        ui->label_54_Status_AttitudeIndicateComplageRespond->setText(AMapPod54ControlStatus_AttitudeIndicationFeedback_ToStr(aMapPod54ControlStatus.attitudeIndicationFeedback));

        if (m_AMapPod54StatusInfo.trackCenterWidthCoordinate != aMapPod54ControlStatus.trackCenterWidthCoordinate)
        {
            m_AMapPod54StatusInfo.trackCenterWidthCoordinate = aMapPod54ControlStatus.trackCenterWidthCoordinate;
            ui->label_54_Status_TrackCenterWidthPoint->setText(QString::asprintf("%d", aMapPod54ControlStatus.trackCenterWidthCoordinate));
        }

        if (m_AMapPod54StatusInfo.trackCenterHeightCoordinate != aMapPod54ControlStatus.trackCenterHeightCoordinate)
        {
            m_AMapPod54StatusInfo.trackCenterHeightCoordinate = aMapPod54ControlStatus.trackCenterHeightCoordinate;
            ui->label_54_Status_TrackCenterHeightPoint->setText(QString::asprintf("%d", aMapPod54ControlStatus.trackCenterHeightCoordinate));
        }

        if (m_AMapPod54StatusInfo.trackingWidthLength != aMapPod54ControlStatus.trackingWidthLength)
        {
            m_AMapPod54StatusInfo.trackingWidthLength = aMapPod54ControlStatus.trackingWidthLength;
            ui->label_TrackWidthLength->setText(QString::asprintf("%u", aMapPod54ControlStatus.trackingWidthLength));
        }

        if (m_AMapPod54StatusInfo.trackingHeightLength != aMapPod54ControlStatus.trackingHeightLength)
        {
            m_AMapPod54StatusInfo.trackingHeightLength = aMapPod54ControlStatus.trackingHeightLength;
            ui->label_54_Status_TrackHeightLength->setText(QString::asprintf("%u", aMapPod54ControlStatus.trackingHeightLength));
        }
    }
    else
    {

        if (m_AMapPod54StatusInfo.pod1Status.photographStatus != aMapPod54ControlStatus.pod1Status.photographStatus)
        {
            m_AMapPod54StatusInfo.pod1Status.photographStatus = aMapPod54ControlStatus.pod1Status.photographStatus;
            ui->label_54_Status_TakePicture_Common->setText((aMapPod54ControlStatus.pod1Status.photographStatus ? tr("Taking pictures") : tr("Not photographed")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.videoCapture != aMapPod54ControlStatus.pod1Status.videoCapture)
        {
            m_AMapPod54StatusInfo.pod1Status.videoCapture = aMapPod54ControlStatus.pod1Status.videoCapture;
            ui->label_54_Status_CapPicture_Common->setText((aMapPod54ControlStatus.pod1Status.videoCapture ? tr("Video capture") : tr("No video capture")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.burstStatus != aMapPod54ControlStatus.pod1Status.burstStatus)
        {
            m_AMapPod54StatusInfo.pod1Status.burstStatus = aMapPod54ControlStatus.pod1Status.burstStatus;
            ui->label_54_Status_ContinueTakePicture_Common->setText((aMapPod54ControlStatus.pod1Status.burstStatus ? tr("In continuous shooting") : tr("No continuous shot")));
        }

        if (m_AMapPod54StatusInfo.pod1Status.recordingStatus != aMapPod54ControlStatus.pod1Status.recordingStatus)
        {
            m_AMapPod54StatusInfo.pod1Status.recordingStatus = aMapPod54ControlStatus.pod1Status.recordingStatus;
            ui->label_54_Status_RecodeVideo_Common->setText((aMapPod54ControlStatus.pod1Status.recordingStatus ? tr("Recording") : tr("The video was not started")));
        }

        if (m_AMapPod54StatusInfo.pod2Status.videoOutput != aMapPod54ControlStatus.pod2Status.videoOutput)
        {
            m_AMapPod54StatusInfo.pod2Status.videoOutput = aMapPod54ControlStatus.pod2Status.videoOutput;
            ui->label_54_Status_VideoOutput_Common->setText((aMapPod54ControlStatus.pod2Status.videoOutput ? tr("Visible light video output") : tr("Visible light video output")));
        }

        if (m_AMapPod54StatusInfo.pod2Status.infraredElectronicZoom != aMapPod54ControlStatus.pod2Status.infraredElectronicZoom)
        {
            m_AMapPod54StatusInfo.pod2Status.infraredElectronicZoom = aMapPod54ControlStatus.pod2Status.infraredElectronicZoom;
            ui->label_54_Status_InfraredOut_Common->setText(QString::number(aMapPod54ControlStatus.pod2Status.infraredElectronicZoom * 10));
        }

        if (m_AMapPod54StatusInfo.pod2Status.lightElectronicZoom != aMapPod54ControlStatus.pod2Status.lightElectronicZoom)
        {
            m_AMapPod54StatusInfo.pod2Status.lightElectronicZoom = aMapPod54ControlStatus.pod2Status.lightElectronicZoom;
            ui->label_54_Status_LightOut_Common->setText(QString::number(std::pow(2, aMapPod54ControlStatus.pod2Status.lightElectronicZoom)));
        }

        if (m_AMapPod54StatusInfo.pod2Status.ribbon != aMapPod54ControlStatus.pod2Status.ribbon)
        {
            m_AMapPod54StatusInfo.pod2Status.ribbon = aMapPod54ControlStatus.pod2Status.ribbon;
            ui->label_54_Status_Ribbon_Common->setText(AMapPod54Control_FalseColorBandModel_ToStr(aMapPod54ControlStatus.pod2Status.ribbon - 1));
        }

        {
            m_AMapPod54StatusInfo.podFrameworkYaw = aMapPod54ControlStatus.podFrameworkYaw;
            ui->label_54_Status_GimbalYaw_Common->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podFrameworkYaw / 100.0)));
        }

        {
            m_AMapPod54StatusInfo.podFrameworkPitch = aMapPod54ControlStatus.podFrameworkPitch;
            ui->label_54_Status_GimbalPitch_Common->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podFrameworkPitch / 100.0)));
        }

        {
            m_AMapPod54StatusInfo.podFrameworkRoll = aMapPod54ControlStatus.podFrameworkRoll;
            ui->label_54_Status_GimbalRoll_Common->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podFrameworkRoll / 100.0)));
        }

        if (m_AMapPod54StatusInfo.tfPercentageOfCapacityUsed != aMapPod54ControlStatus.tfPercentageOfCapacityUsed)
        {
            m_AMapPod54StatusInfo.tfPercentageOfCapacityUsed = aMapPod54ControlStatus.tfPercentageOfCapacityUsed;
            ui->label_54_Status_TFCapacity_Common->setText(QString::asprintf("%u%%", aMapPod54ControlStatus.tfPercentageOfCapacityUsed));
        }

        if (m_AMapPod54StatusInfo.infraredFocalLength != aMapPod54ControlStatus.infraredFocalLength)
        {
            m_AMapPod54StatusInfo.infraredFocalLength = aMapPod54ControlStatus.infraredFocalLength;
            ui->label_54_Status_InfraredFocus_Common->setText(QString::asprintf("%.1fmm", (aMapPod54ControlStatus.infraredFocalLength / 10.0)));
        }

        if (m_AMapPod54StatusInfo.lightFocalLength != aMapPod54ControlStatus.lightFocalLength)
        {
            m_AMapPod54StatusInfo.lightFocalLength = aMapPod54ControlStatus.lightFocalLength;
            ui->label_54_Status_LightFocus_Common->setText(QString::asprintf("%.1fmm", (aMapPod54ControlStatus.lightFocalLength / 10.0)));
        }

        if (m_AMapPod54StatusInfo.laserDistance != aMapPod54ControlStatus.laserDistance)
        {
            m_AMapPod54StatusInfo.laserDistance = aMapPod54ControlStatus.laserDistance;
            ui->label_54_Status_TelemeterDistanceValue_Common->setText(QString::asprintf("%.1fm", (aMapPod54ControlStatus.laserDistance / 10.0)));
        }

        {
            m_AMapPod54StatusInfo.targetLongitude = aMapPod54ControlStatus.targetLongitude;
            ui->label_54_Status_TargetLongitude_Common->setText(QString::asprintf("%.6f", (aMapPod54ControlStatus.targetLongitude)));
        }

        {
            m_AMapPod54StatusInfo.targetLatitude = aMapPod54ControlStatus.targetLatitude;
            ui->label_54_Status_TargetLatitude_Common->setText(QString::asprintf("%.6f", (aMapPod54ControlStatus.targetLatitude)));
        }

        {
            m_AMapPod54StatusInfo.targetAltitude = aMapPod54ControlStatus.targetAltitude;
            ui->label_54_Status_TargetAltitude_Common->setText(QString::asprintf("%.2fm", (aMapPod54ControlStatus.targetAltitude / 10.0)));
        }

        {
            m_AMapPod54StatusInfo.podAttitudeYaw = aMapPod54ControlStatus.podAttitudeYaw;
            ui->label_54_Status_GimbalAttitudeYaw_Common->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podAttitudeYaw / 100.0)));
        }

        {
            m_AMapPod54StatusInfo.podAttitudePitch = aMapPod54ControlStatus.podAttitudePitch;
            ui->label_54_Status_GimbalAttitudePitch_Common->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podAttitudePitch / 100.0)));
        }

        {
            m_AMapPod54StatusInfo.podAttitudeRoll = aMapPod54ControlStatus.podAttitudeRoll;
            ui->label_54_Status_GimbalAttitudeRoll_Common->setText(QString::asprintf(u8"%.2f°", (aMapPod54ControlStatus.podAttitudeRoll / 100.0)));
        }

        if (m_AMapPod54StatusInfo.currentDisplayImageFeedback != aMapPod54ControlStatus.currentDisplayImageFeedback)
        {
            m_AMapPod54StatusInfo.currentDisplayImageFeedback = aMapPod54ControlStatus.currentDisplayImageFeedback;
            ui->label_54_Status_LightShowPictureRespond_Common->setText(AMapPod54ControlStatus_CurrentDisplayImageFeedback_ToStr(aMapPod54ControlStatus.currentDisplayImageFeedback));
        }

    }
}

QString IAerialShotSettingBundle54Dialog::AMapPod54Control_FalseColorBandModel_ToStr(const int falseColorBandModel)
{
    switch (falseColorBandModel)
    {
    case 0x00:
        return tr("white hot");
    case 0x01:
        return tr("Black hot");
    case 0x02:
        return tr("Blue red yellow");
    case 0x03:
        return tr("Purple red yellow");
    case 0x04:
        return tr("Blue green red");
    case 0x05:
        return tr("Rainbow1");
    case 0x06:
        return tr("Rainbow2");
    case 0x07:
        return tr("Black red");
    case 0x08:
        return tr("Black green red");
    case 0x09:
        return tr("Blue green red powder");
    case 0x0A:
        return tr("Secondary colours");
    case 0x0B:
        return tr("Red head");
    default:
        return tr("Unknown");
    }
    return tr("Unknown");
}

QString IAerialShotSettingBundle54Dialog::AMapPod54ControlStatus_PayloadType_ToStr(const AMapPod54ControlStatus_PayloadType payloadType)
{
    switch (payloadType)
    {
    case AMapPod54ControlStatus_PayloadType::AMapPod54ControlStatus_PayloadType_None:
        return tr("Unknown");
    case AMapPod54ControlStatus_PayloadType::AMapPod54ControlStatus_PayloadType_Light:
        return tr("Light");
    case AMapPod54ControlStatus_PayloadType::AMapPod54ControlStatus_PayloadType_Infrared:
        return tr("Infrared");
    case AMapPod54ControlStatus_PayloadType::AMapPod54ControlStatus_PayloadType_DoubleLight:
        return tr("DoubleLight");
    case AMapPod54ControlStatus_PayloadType::AMapPod54ControlStatus_PayloadType_ThreeLight:
        return tr("ThreeLight");
    default:
        return tr("Unknown");
    }
    return tr("Unknown");
}

QString IAerialShotSettingBundle54Dialog::AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(const uint8_t autoCheckResultStatus)
{
    switch (autoCheckResultStatus)
    {
    case 0:
        return tr("Abnormal");
    case 1:
        return tr("Normal");
    default:
        return tr("Unknown");
    }
    return tr("Unknown");
}

QString IAerialShotSettingBundle54Dialog::AMapPod54ControlStatus_ServoStatus_ToStr(const uint8_t servoStatus)
{
    switch (servoStatus)
    {
    case 0x01:
        return tr("Load closing");
    case 0x02:
        return tr("Manual");
    case 0x03:
        return tr("Collect");
    case 0x04:
        return tr("Number primer");
    case 0x05:
        return tr("Yaw lock");
    case 0x06:
        return tr("Scan");
    case 0x07:
        return tr("Track");
    case 0x08:
        return tr("Vertical hypovision");
    case 0x09:
        return tr("Gyroscope auto cal");
    case 0x0A:
        return tr("Gyroscope temperature cal");
    case 0x0B:
        return tr("Yaw follow-up");
    case 0x0C:
        return tr("Angelica");
    case 0x0D:
        return tr("Gyroscope Manual cal");
    case 0x0E:
        return tr("Attitude guidance");
    case 0x0F:
        return tr("Attitude guidance");
    default:
        return tr("Unknown");
    }
    return tr("Unknown");
}

QString IAerialShotSettingBundle54Dialog::AMapPod54ControlStatus_CurrentDisplayImageFeedback_ToStr(const uint8_t currentDisplayImageFeedback)
{
    switch (currentDisplayImageFeedback)
    {
    case 0x00:
        return tr("Infrared");
    case 0x01:
        return tr("Light");
    case 0x02:
        return tr("Infrared&Light");
    default:
        return tr("Unknown");
    }
    return tr("Unknown");
}

QString IAerialShotSettingBundle54Dialog::AMapPod54ControlStatus_AttitudeIndicationFeedback_ToStr(const uint8_t attitudeIndicationFeedback)
{
    switch (attitudeIndicationFeedback)
    {
    case 0x12:
        return tr("Attitude posturing");
    case 0x13:
        return tr("Attitude complete posturing");
    default:
        return tr("Unknown");
    }
    return tr("Unknown");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_Light_Simple_clicked()
{

    on_pushButton_54_InfraredLightSet_Light_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 载荷公用 可见光");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_Infrared_Simple_clicked()
{

    on_pushButton_54_InfraredLightSet_Infrared_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 载荷公用 红外");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_InfraredAndLight_Simple_clicked()
{

    on_pushButton_54_InfraredLightSet_InfraredAndLight_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 载荷公用 图像叠加");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_SingleLaserTelemeter_Simple_clicked()
{

    on_pushButton_54_SingleLaserTelemeter_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 载荷公用 单次激光测距");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StartContinueLaserTelemeter_Simple_clicked()
{

    on_pushButton_54_StartContinueLaserTelemeter_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 载荷公用 开启连续激光测距");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StopContinueLaserTelemeter_Simple_clicked()
{

    on_pushButton_54_StopContinueLaserTelemeter_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 载荷公用 停止连续激光测距");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalCenter_Simple_clicked()
{

    on_pushButton_54_GimbalCenter_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 云台功能 云台归中");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalLookDownVertical_Simple_clicked()
{

    on_pushButton_54_GimbalLookDownVertical_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 云台功能 垂直下视");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalYawLock_Common_Simple_clicked()
{

    on_pushButton_54_GimbalYawLock_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 云台功能 方位锁定");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_AttitudeGuideFollow_Simple_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    double attitudeGuideFollow_Yaw = ui->doubleSpinBox_54_AttitudeGuideFollow_Yaw_Simple->value();
    if ((attitudeGuideFollow_Yaw < -180.0) || (attitudeGuideFollow_Yaw > 180.0))
    {
        return ;
    }

    double attitudeGuideFollow_Pitch = ui->doubleSpinBox_54_AttitudeGuideFollow_Pitch_Simple->value();
    if ((attitudeGuideFollow_Pitch < -120.0) || (attitudeGuideFollow_Pitch > 90.0))
    {
        return ;
    }

    int16_t yaw   = static_cast<int16_t>(attitudeGuideFollow_Yaw   * 100);
    int16_t pitch = static_cast<int16_t>(attitudeGuideFollow_Pitch * 100);

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_AttitudeGuideFollow;
    aMapPod54Control.commonCommandArg.AttitudeLeadSet_7C.yaw   = yaw;
    aMapPod54Control.commonCommandArg.AttitudeLeadSet_7C.pitch = pitch;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2(" 简单 部分 简单 功能 界面 云台功能 姿态指引") << aMapPod54Control << yaw << pitch;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleOut_Simple_pressed()
{

    on_pushButton_54_LigthMultipleOut_pressed();

    logInfo() << tr2("简单 部分 简单 功能 界面 可见光载荷 变倍+ 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleOut_Simple_released()
{

    on_pushButton_54_LigthMultipleOut_released();

    logInfo() << tr2("简单 部分 简单 功能 界面 可见光载荷 变倍+ 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleOut_Simple_clicked()
{

    on_pushButton_54_LigthMultipleOut_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 可见光载荷 变倍+  按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleIn_Simple_pressed()
{

    on_pushButton_54_LigthMultipleIn_pressed();

    logInfo() << tr2("简单 部分 简单 功能 界面 可见光载荷 变倍- 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleIn_Simple_released()
{

    on_pushButton_54_LigthMultipleIn_released();

    logInfo() << tr2("简单 部分 简单 功能 界面 可见光载荷 变倍- 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleIn_Simple_clicked()
{

    on_pushButton_54_LigthMultipleIn_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 可见光载荷 变倍- 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusAuto_Simple_clicked()
{

    on_pushButton_54_LigthFacusAuto_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 可见光载荷 自动聚焦");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredShutterCompensate_Simple_clicked()
{

    on_pushButton_54_InfraredShutterCompensate_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 红外载荷 红外快门补偿");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusAuto_Simple_clicked()
{

    on_pushButton_54_InfraredFocus_FocusAuto_clicked();

    logInfo() << tr2("简单 部分 简单 功能 界面 红外载荷 自动聚焦");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredCutoverRibbonSet_Simple_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    const int currentIndex = ui->comboBox_54_InfraredCutoverRibbonSet_Simple->currentIndex();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredCutoverRibbonSwitch;
    aMapPod54Control.commonCommandArg.falseColorBandSwitchSet_53.falseColorBandSwitch = AMapPod54Control_FalseColorBandSwitch::AMapPod54Control_FalseColorBandSwitch_ManualSet;
    aMapPod54Control.commonCommandArg.falseColorBandSwitchSet_53.falseColorBandModel  = AMapPod54Control_FalseColorBandModel_FromIndex(currentIndex);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("简单 部分 简单 功能 界面  伪彩色带切换 红外载荷 指定伪彩色带切换") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Left_Up_Simple_pressed()
{
    if (!ui)
    {
        return ;
    }

    const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(-PTZControlSpend);
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(PTZControlSpend);

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("Left Up : 向左上按钮按下 按钮按下") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = [this]()
    {
        const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(-PTZControlSpend);
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(PTZControlSpend);

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("Left Up : 向左上按钮按下 按钮按下") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54TimerJoystickControlSimple.start();

    logInfo() << tr2("Left Up : 向左上按钮按下 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Left_Up_Simple_released()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Left Up : 向左上按钮  按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Left_Up_Simple_clicked()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Left Up : 向左上按钮  按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Up_Simple_pressed()
{
    if (!ui)
    {
        return ;
    }

    const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = 0;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(PTZControlSpend);

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("Up : 向上按钮按下 按钮按下") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = [this]()
    {
        const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = 0;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(PTZControlSpend);

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("Up : 向上按钮按下 按钮按下") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54TimerJoystickControlSimple.start();

    logInfo() << tr2("Up : 向上按钮按下 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Up_Simple_released()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Up : 向上按钮  按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Up_Simple_clicked()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Up : 向上按钮  按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Right_Up_Simple_pressed()
{
    if (!ui)
    {
        return ;
    }

    const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(PTZControlSpend);
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(PTZControlSpend);

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("Right Up : 向右上按钮按下 按钮按下") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = [this]()
    {
        const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(PTZControlSpend);
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(PTZControlSpend);

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("Right Up : 向右上按钮按下 按钮按下") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54TimerJoystickControlSimple.start();

    logInfo() << tr2("Right Up : 向右上按钮按下 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Right_Up_Simple_released()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Right Up : 向右上按钮  按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Right_Up_Simple_clicked()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Right Up : 向右上按钮  按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Left_Simple_pressed()
{
    if (!ui)
    {
        return ;
    }

    const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(-PTZControlSpend);
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = 0;

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("Left : 向左按钮按下 按钮按下") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = [this]()
    {
        const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(-PTZControlSpend);
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = 0;

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("Left : 向左按钮按下 按钮按下") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54TimerJoystickControlSimple.start();

    logInfo() << tr2("Left : 向左按钮按下 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Left_Simple_released()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    logInfo() << tr2("Left : 向左按钮  按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Left_Simple_clicked()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Left : 向左按钮  按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Right_Simple_pressed()
{
    if (!ui)
    {
        return ;
    }

    const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(PTZControlSpend);
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = 0;

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("Right : 向右按钮按下 按钮按下") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = [this]()
    {
        const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(PTZControlSpend);
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = 0;

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("Right : 向右按钮按下 按钮按下") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54TimerJoystickControlSimple.start();

    logInfo() << tr2("Right : 向右按钮按下 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Right_Simple_released()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Right : 向右按钮  按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Right_Simple_clicked()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Right : 向右按钮  按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Left_Down_Simple_pressed()
{
    if (!ui)
    {
        return ;
    }

    const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(-PTZControlSpend);
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(-PTZControlSpend);

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("Left Down : 向左下按钮按下 按钮按下") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = [this]()
    {
        const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(-PTZControlSpend);
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(-PTZControlSpend);

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("Left Down : 向左下按钮按下 按钮按下") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54TimerJoystickControlSimple.start();

    logInfo() << tr2("Left Down : 向左下按钮按下 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Left_Down_Simple_released()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    logInfo() << tr2("Left Down : 向左下按钮  按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Left_Down_Simple_clicked()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Left Down : 向左下按钮  按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Down_Simple_pressed()
{
    if (!ui)
    {
        return ;
    }

    const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = 0;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(-PTZControlSpend);

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("Down : 向下按钮按下 按钮按下") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = [this]()
    {
        const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = 0;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(-PTZControlSpend);

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("Down : 向下按钮按钮按下 按钮按下") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54TimerJoystickControlSimple.start();

    logInfo() << tr2("Down : 向下按钮按钮按下 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Down_Simple_released()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Down : 向下按钮  按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Down_Simple_clicked()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Down : 向下按钮  按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Right_Down_Simple_pressed()
{
    if (!ui)
    {
        return ;
    }

    const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(PTZControlSpend);
    aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(-PTZControlSpend);

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("Right Down : 向右下按钮按下 按钮按下 ") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = [this]()
    {
        const int PTZControlSpend = ui->spinBox_54_JoystickControl_Speed_Simple->value();

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlYaw   = static_cast<int16_t>(PTZControlSpend);
        aMapPod54Control.PTZJoyStickControlArg.PTZControl.PTZControlPitch = static_cast<int16_t>(-PTZControlSpend);

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("Right Down : 向右下按钮按下 按钮按下 ") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54TimerJoystickControlSimple.start();

    logInfo() << tr2("Right Down : 向右下按钮按下 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Right_Down_Simple_released()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Right Down : 向右下按钮 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_JoystickControl_Right_Down_Simple_clicked()
{

    m_AerialShotSettingBundle54TimerJoystickControlSimple.stop();
    m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction = nullptr;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("Right Down : 向右下按钮 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_Simple_Common_All_Function_Switch_Common_clicked()
{

    m_Widget_54_All_Function = false;

    ui->widget_54_Common_All_Function_Switch_Simple->hide();
    ui->widget_54_Common_All_Function_Switch_Simple->setVisible(false);
    ui->widget_54_Common_All_Function_Switch_Common->show();
    ui->widget_54_Common_All_Function_Switch_Common->setVisible(true);
    ui->widget_54_Common_All_Function_Switch_All->hide();
    ui->widget_54_Common_All_Function_Switch_All->setVisible(false);

    initAMapPodState();

    on_radioButton_JoystickDisEnable_clicked();
    on_radioButton_JoystickDisEnable_Common_clicked();
    ui->radioButton_JoystickDisEnable_Common->setChecked(false);
    ui->radioButton_JoystickDisEnable->setChecked(false);
    ui->radioButton_JoystickDisEnable_Common->setChecked(true);
    ui->radioButton_JoystickDisEnable->setChecked(true);
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_Batch_Simple_clicked()
{
    on_pushButton_54_CommonCommand_Batch_clicked();
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_Light_Common_clicked()
{

    on_pushButton_54_InfraredLightSet_Light_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 可见光");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_Infrared_Common_clicked()
{

    on_pushButton_54_InfraredLightSet_Infrared_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 红外");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_InfraredAndLight_Common_clicked()
{

    on_pushButton_54_InfraredLightSet_InfraredAndLight_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 图像叠加");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StartStopContinueGrab_Common_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    const int startStopContinueGrab = ui->spinBox_54_StartStopContinueGrab_Common->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StartStopContinueGrab;
    aMapPod54Control.commonCommandArg.captureIntervalTimeSet_34.captureIntervalTime = static_cast<uint16_t>(startStopContinueGrab);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2(" 常用 部分 简单 功能 界面 载荷公用 开始/停止连续抓图") << aMapPod54Control << startStopContinueGrab;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_Grab_Common_clicked()
{

    on_pushButton_54_Grab_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 抓图");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StartStopRecodeVideo_Common_clicked()
{

    on_pushButton_54_StartStopRecodeVideo_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 开始/停止录像");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_CommonCommand_AutoFocus_Common_clicked()
{

    on_pushButton_54_CommonCommand_AutoFocus_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 自动聚焦");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_SingleLaserTelemeter_Common_clicked()
{

    on_pushButton_54_SingleLaserTelemeter_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 单次激光测距");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StartContinueLaserTelemeter_Common_clicked()
{

    on_pushButton_54_StartContinueLaserTelemeter_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 开启连续激光测距");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StopContinueLaserTelemeter_Common_clicked()
{

    on_pushButton_54_StopContinueLaserTelemeter_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 停止连续激光测距");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_CommonCommand_SameView_Common_clicked()
{

    on_pushButton_54_CommonCommand_SameView_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 载荷公用 同视场");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_CommonCommand_Batch_Common_clicked()
{
    on_pushButton_54_CommonCommand_Batch_clicked();
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_Common_All_Function_Switch_All_clicked()
{

    m_Widget_54_All_Function = true;

    ui->widget_54_Common_All_Function_Switch_Simple->hide();
    ui->widget_54_Common_All_Function_Switch_Simple->setVisible(false);
    ui->widget_54_Common_All_Function_Switch_Common->hide();
    ui->widget_54_Common_All_Function_Switch_Common->setVisible(false);
    ui->widget_54_Common_All_Function_Switch_All->show();
    ui->widget_54_Common_All_Function_Switch_All->setVisible(true);

    initAMapPodState();

    on_radioButton_JoystickDisEnable_clicked();
    on_radioButton_JoystickDisEnable_Common_clicked();
    ui->radioButton_JoystickDisEnable_Common->setChecked(false);
    ui->radioButton_JoystickDisEnable->setChecked(false);
    ui->radioButton_JoystickDisEnable_Common->setChecked(true);
    ui->radioButton_JoystickDisEnable->setChecked(true);
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_Common_All_Function_Switch_Simple_clicked()
{

    m_Widget_54_All_Function = false;

    ui->widget_54_Common_All_Function_Switch_Simple->show();
    ui->widget_54_Common_All_Function_Switch_Simple->setVisible(true);
    ui->widget_54_Common_All_Function_Switch_Common->hide();
    ui->widget_54_Common_All_Function_Switch_Common->setVisible(false);
    ui->widget_54_Common_All_Function_Switch_All->hide();
    ui->widget_54_Common_All_Function_Switch_All->setVisible(false);

    initAMapPodState();

    on_radioButton_JoystickDisEnable_clicked();
    on_radioButton_JoystickDisEnable_Common_clicked();
    ui->radioButton_JoystickDisEnable_Common->setChecked(false);
    ui->radioButton_JoystickDisEnable->setChecked(false);
    ui->radioButton_JoystickDisEnable_Common->setChecked(true);
    ui->radioButton_JoystickDisEnable->setChecked(true);
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalCenter_Common_clicked()
{

    on_pushButton_54_GimbalCenter_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 云台功能 云台归中");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalLookDownVertical_Common_clicked()
{

    on_pushButton_54_GimbalLookDownVertical_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 云台功能 垂直下视");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalYawLock_Common_clicked()
{

    on_pushButton_54_GimbalYawLock_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 云台功能 方位锁定");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_YawFollow_Common_clicked()
{

    on_pushButton_54_YawFollow_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 云台功能 方位弱随动机头");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_CollectMode_Common_clicked()
{

    on_pushButton_54_CollectMode_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 云台功能 收藏模式");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalMoterClose_Common_clicked()
{

    on_pushButton_54_GimbalMoterClose_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 云台功能 关闭伺服");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_NumberFollow_Common_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    double numberFollowYaw = ui->doubleSpinBox_54_NumberFollowYaw_Common->value();
    if ((numberFollowYaw < -180) || (numberFollowYaw > 180))
    {
        return ;
    }

    double numberFollowPitch = ui->doubleSpinBox_54_NumberFollowPitch_Common->value();
    if ((numberFollowPitch < -120) || (numberFollowPitch > 90))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_NumberFollow;
    aMapPod54Control.commonCommandArg.countModelSet_72.yaw   = static_cast<int16_t>(numberFollowYaw * 100);
    aMapPod54Control.commonCommandArg.countModelSet_72.pitch = static_cast<int16_t>(numberFollowPitch * 100);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2(" 常用 部分 简单 功能 界面 云台功能 数引模式") << aMapPod54Control << numberFollowYaw << numberFollowPitch;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_AttitudeGuideFollow_Common_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    double attitudeGuideFollow_Yaw = ui->doubleSpinBox_54_AttitudeGuideFollow_Yaw_Common->value();
    if ((attitudeGuideFollow_Yaw < -180.0) || (attitudeGuideFollow_Yaw > 180.0))
    {
        return ;
    }

    double attitudeGuideFollow_Pitch = ui->doubleSpinBox_54_AttitudeGuideFollow_Pitch_Common->value();
    if ((attitudeGuideFollow_Pitch < -120.0) || (attitudeGuideFollow_Pitch > 90.0))
    {
        return ;
    }

    int16_t yaw   = static_cast<int16_t>(attitudeGuideFollow_Yaw   * 100);
    int16_t pitch = static_cast<int16_t>(attitudeGuideFollow_Pitch * 100);

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_AttitudeGuideFollow;
    aMapPod54Control.commonCommandArg.AttitudeLeadSet_7C.yaw   = yaw;
    aMapPod54Control.commonCommandArg.AttitudeLeadSet_7C.pitch = pitch;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2(" 常用 部分 简单 功能 界面 云台功能 姿态指引") << aMapPod54Control << yaw << pitch;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleOut_Common_pressed()
{

    on_pushButton_54_LigthMultipleOut_pressed();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 变倍+ 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleOut_Common_released()
{

    on_pushButton_54_LigthMultipleOut_released();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 变倍+ 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleOut_Common_clicked()
{

    on_pushButton_54_LigthMultipleOut_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 变倍+  按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleIn_Common_pressed()
{

    on_pushButton_54_LigthMultipleIn_pressed();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 变倍- 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleIn_Common_released()
{

    on_pushButton_54_LigthMultipleIn_released();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 变倍- 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleIn_Common_clicked()
{

    on_pushButton_54_LigthMultipleIn_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 变倍- 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusOut_Common_pressed()
{

    on_pushButton_54_LigthFacusOut_pressed();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 调焦+ 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusOut_Common_released()
{

    on_pushButton_54_LigthFacusOut_released();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 调焦+ 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusOut_Common_clicked()
{

    on_pushButton_54_LigthFacusOut_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 调焦+ 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusIn_Common_pressed()
{

    on_pushButton_54_LigthFacusIn_pressed();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 调焦- 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusIn_Common_released()
{

    on_pushButton_54_LigthFacusIn_released();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 调焦- 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusIn_Common_clicked()
{

    on_pushButton_54_LigthFacusIn_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 调焦- 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthLowilluminationClose_Common_clicked()
{

    on_pushButton_54_LigthLowilluminationClose_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 低照度关");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthLowilluminationOpen_Common_clicked()
{

    on_pushButton_54_LigthLowilluminationOpen_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 低照度开");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthZoomFacusStop_Common_clicked()
{

    on_pushButton_54_LigthZoomFacusStop_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 停止变倍调焦");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusAuto_Common_clicked()
{

    on_pushButton_54_LigthFacusAuto_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 自动聚焦");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthElectChange_Common_clicked()
{

    on_pushButton_54_LigthElectChange_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 可见光载荷 电子变倍");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusSet_Common_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    double ligthFacusSet = ui->doubleSpinBox_54_LigthFacusSet_Common->value() * 10;
    if ((ligthFacusSet < 43) || (ligthFacusSet > 1290))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthFacus;
    aMapPod54Control.commonCommandArg.lightFacusSet_4C.lightFacus = ligthFacusSet;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2(" 常用 部分 简单 功能 界面 可见光载荷 可见光载荷 焦距设置") << aMapPod54Control << aMapPod54Control.commonCommandArg.lightFacusSet_4C.lightFacus;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthShoot_Common_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthShoot;
    aMapPod54Control.commonCommandArg.ligthShoot_44.ligthShoot = static_cast<uint16_t>(ui->spinBox_54_LigthShoot_Common->value());

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2(" 常用 部分 简单 功能 界面 可见光载荷 开始停止连拍") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredShutterCompensate_Common_clicked()
{

    on_pushButton_54_InfraredShutterCompensate_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 红外快门补偿");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_SceneSupplement_Common_clicked()
{

    on_pushButton_54_SceneSupplement_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 场景补偿");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredGraphicsStrengthen_Common_clicked()
{

    on_pushButton_54_InfraredGraphicsStrengthen_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 红外图像增强");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusFar_Common_pressed()
{

    on_pushButton_54_InfraredFocus_FocusFar_pressed();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 远焦 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusFar_Common_released()
{

    on_pushButton_54_InfraredFocus_FocusFar_released();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 远焦 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusFar_Common_clicked()
{

    on_pushButton_54_InfraredFocus_FocusFar_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 远焦 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusNear_Common_pressed()
{

    on_pushButton_54_InfraredFocus_FocusNear_pressed();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 近焦 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusNear_Common_released()
{

    on_pushButton_54_InfraredFocus_FocusNear_released();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 近焦 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusNear_Common_clicked()
{

    on_pushButton_54_InfraredFocus_FocusNear_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 近焦 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusStop_Common_clicked()
{

    on_pushButton_54_InfraredFocus_FocusStop_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 停止聚焦");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusAuto_Common_clicked()
{

    on_pushButton_54_InfraredFocus_FocusAuto_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 自动聚焦");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleOut_Common_pressed()
{

    on_pushButton_54_InfraredMultipleOut_pressed();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 电子变倍+ 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleOut_Common_released()
{

    on_pushButton_54_InfraredMultipleOut_released();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 电子变倍+ 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleOut_Common_clicked()
{

    on_pushButton_54_InfraredMultipleOut_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 电子变倍+ 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleIn_Common_pressed()
{

    on_pushButton_54_InfraredMultipleIn_pressed();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 电子变倍- 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleIn_Common_released()
{

    on_pushButton_54_InfraredMultipleIn_released();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 电子变倍- 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleIn_Common_clicked()
{

    on_pushButton_54_InfraredMultipleIn_clicked();

    logInfo() << tr2("常用 部分 简单 功能 界面 红外载荷 电子变倍- 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredCutoverRibbonSet_Common_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    const int currentIndex = ui->comboBox_54_InfraredCutoverRibbonSet_Common->currentIndex();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredCutoverRibbonSwitch;
    aMapPod54Control.commonCommandArg.falseColorBandSwitchSet_53.falseColorBandSwitch = AMapPod54Control_FalseColorBandSwitch::AMapPod54Control_FalseColorBandSwitch_ManualSet;
    aMapPod54Control.commonCommandArg.falseColorBandSwitchSet_53.falseColorBandModel  = AMapPod54Control_FalseColorBandModel_FromIndex(currentIndex);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("常用 部分 简单 功能 界面  伪彩色带切换 红外载荷 指定伪彩色带切换") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnable_Common_clicked()
{
    ui->label_JoystickWidget_Common->setAxisVisible(true);
    m_Joy_FineTuning = false;

    if (m_JoystickEnableGimbal)
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("常用 部分 简单 功能 界面 启用摇杆 控制云台") << aMapPod54Control;
    }
    else
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("常用 部分 简单 功能 界面 选取跟踪目标坐标值") << aMapPod54Control;
    }
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnableFine_Common_clicked()
{

    ui->label_JoystickWidget_Common->setAxisVisible(true);
    m_Joy_FineTuning = true;

    if (m_JoystickEnableGimbal)
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("常用 部分 简单 功能 界面 微调 启用摇杆 或者 鼠标 控制云台") << aMapPod54Control;
    }
    else
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("常用 部分 简单 功能 界面 微调 选取跟踪目标坐标值") << aMapPod54Control;
    }
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnableMouse_Common_clicked()
{

    ui->label_JoystickWidget_Common->setAxisMouse(true);
    m_Joy_FineTuning = false;

    if (m_JoystickEnableGimbal)
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("常用 部分 简单 功能 界面 启用 摇杆 鼠标 控制云台") << aMapPod54Control;
    }
    else
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("常用 部分 简单 功能 界面 选取跟踪目标坐标值") << aMapPod54Control;
    }
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickDisEnable_Common_clicked()
{

    ui->label_JoystickWidget_Common->setAxisMouse(false);
    ui->label_JoystickWidget_Common->setAxisVisible(false);
    m_Joy_FineTuning = false;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("常用 部分 简单 功能 界面 禁用 摇杆 鼠标") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnableGimbal_Common_clicked()
{

    m_JoystickEnableGimbal = true;

    ui->radioButton_JoystickEnable->setCheckable(true);
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnableTracking_Common_clicked()
{

    m_JoystickEnableGimbal = false;

    ui->radioButton_JoystickEnableTracking->setCheckable(true);
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSwitch_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredLightSwitch;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外/可见光切换") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_Grab_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_Grab;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("抓图") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StartStopRecodeVideo_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StartStopRecodeVideo;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("开始/停止录像") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StartStopContinueGrab_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    const int startStopContinueGrab = ui->spinBox_54_StartStopContinueGrab->value();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StartStopContinueGrab;
    aMapPod54Control.commonCommandArg.captureIntervalTimeSet_34.captureIntervalTime = static_cast<uint16_t>(startStopContinueGrab);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("开始/停止连续抓图") << aMapPod54Control << startStopContinueGrab;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_SingleLaserTelemeter_clicked()
{
    const auto constFindIter = m_AMapPod54ControlStatusInfoMap.constFind(m_CurrentSensorID);
    if (constFindIter != m_AMapPod54ControlStatusInfoMap.constEnd())
    {
        auto &aMapPod54ControlStatusInfoMap = m_AMapPod54ControlStatusInfoMap[m_CurrentSensorID];
        aMapPod54ControlStatusInfoMap.laserStarted                 = true;
        aMapPod54ControlStatusInfoMap.laserDistanceContinueStarted = false;
        aMapPod54ControlStatusInfoMap.laserDistanceStarted         = true;
        aMapPod54ControlStatusInfoMap.receivedCmdFeedback          = 0;
        aMapPod54ControlStatusInfoMap.receivedLaserDistanceCount = 0;
        aMapPod54ControlStatusInfoMap.laserDistanceTimes           = ::time(nullptr);
        aMapPod54ControlStatusInfoMap.laserDistanceCount           = 0;

        if (aMapPod54ControlStatusInfoMap.laserTimer != nullptr)
        {
            aMapPod54ControlStatusInfoMap.laserTimer->stop();
        }

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_SingleLaserTelemeter;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("单次激光测距") << aMapPod54Control;
    }

    logInfo() << tr2("单次激光测距");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StartContinueLaserTelemeter_clicked()
{
    const auto constFindIter = m_AMapPod54ControlStatusInfoMap.constFind(m_CurrentSensorID);
    if (constFindIter != m_AMapPod54ControlStatusInfoMap.constEnd())
    {
        auto &aMapPod54ControlStatusInfoMap = m_AMapPod54ControlStatusInfoMap[m_CurrentSensorID];
        aMapPod54ControlStatusInfoMap.laserStarted                 = true;
        aMapPod54ControlStatusInfoMap.laserDistanceContinueStarted = true;
        aMapPod54ControlStatusInfoMap.laserDistanceStarted         = false;
        aMapPod54ControlStatusInfoMap.receivedCmdFeedback          = 0;
        aMapPod54ControlStatusInfoMap.receivedLaserDistanceCount = 0;
        aMapPod54ControlStatusInfoMap.laserDistanceTimes           = ::time(nullptr);
        aMapPod54ControlStatusInfoMap.laserDistanceCount           = 0;

        QString strCurrentSensorID    = m_CurrentSensorID;
        QString strCurrentComponentID = m_CurrentComponentID;

        {
            AMapPod54Control aMapPod54Control;
            memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
            aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_SingleLaserTelemeter;

            SendAMapPodControlMsg(aMapPod54Control, strCurrentComponentID, strCurrentSensorID);

            AMapPod54Control aMapPod54ControlZero;
            memset(&aMapPod54ControlZero, 0x00, sizeof(aMapPod54ControlZero));
            SendAMapPodControlMsg(aMapPod54ControlZero, strCurrentComponentID, strCurrentSensorID);
        }

        if (aMapPod54ControlStatusInfoMap.laserTimer == nullptr)
        {
            aMapPod54ControlStatusInfoMap.laserTimer = new QTimer(this);
            connect(aMapPod54ControlStatusInfoMap.laserTimer, &QTimer::timeout, [this, strCurrentSensorID, strCurrentComponentID]()
            {
                const auto constFindIter = m_AMapPod54ControlStatusInfoMap.constFind(strCurrentSensorID);
                if (constFindIter != m_AMapPod54ControlStatusInfoMap.constEnd())
                {
                    auto &aMapPod54ControlStatusInfoMap = m_AMapPod54ControlStatusInfoMap[strCurrentSensorID];
                    aMapPod54ControlStatusInfoMap.laserStarted                 = true;
                    aMapPod54ControlStatusInfoMap.laserDistanceContinueStarted = false;
                    aMapPod54ControlStatusInfoMap.laserDistanceStarted         = true;
                    aMapPod54ControlStatusInfoMap.receivedCmdFeedback          = 0;
                    aMapPod54ControlStatusInfoMap.receivedLaserDistanceCount = 0;
                    aMapPod54ControlStatusInfoMap.laserDistanceTimes           = ::time(nullptr);
                    aMapPod54ControlStatusInfoMap.laserDistanceCount           = aMapPod54ControlStatusInfoMap.laserDistanceCount + 1;

                    AMapPod54Control aMapPod54Control;
                    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_SingleLaserTelemeter;

                    SendAMapPodControlMsg(aMapPod54Control, strCurrentComponentID, strCurrentSensorID);

                    AMapPod54Control aMapPod54ControlZero;
                    memset(&aMapPod54ControlZero, 0x00, sizeof(aMapPod54ControlZero));
                    SendAMapPodControlMsg(aMapPod54ControlZero, strCurrentComponentID, strCurrentSensorID);

                    if (aMapPod54ControlStatusInfoMap.laserDistanceCount > 300)
                    {

                        if (aMapPod54ControlStatusInfoMap.laserTimer != nullptr)
                        {
                            aMapPod54ControlStatusInfoMap.laserTimer->stop();
                        }

                        aMapPod54ControlStatusInfoMap.laserStarted                 = false;
                        aMapPod54ControlStatusInfoMap.laserDistanceContinueStarted = false;
                        aMapPod54ControlStatusInfoMap.laserDistanceStarted         = false;
                        aMapPod54ControlStatusInfoMap.receivedCmdFeedback = 0;
                        aMapPod54ControlStatusInfoMap.receivedLaserDistanceCount = 0;
                        aMapPod54ControlStatusInfoMap.laserDistanceTimes           = 0;
                        aMapPod54ControlStatusInfoMap.laserDistanceCount           = 0;

                        AMapPod54Control aMapPod54Control;
                        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
                        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StopContinueLaserTelemeter;

                        SendAMapPodControlMsg(aMapPod54Control);

                        SendAMapPodControlZeroMsg();
                        logInfo() << tr2("停止连续激光测距 连续测距 300s后停止") << aMapPod54Control;
                    }
                    logInfo() << tr2("开启连续激光测距") << aMapPod54Control << aMapPod54ControlStatusInfoMap.laserDistanceCount;
                }
            });
        }

        if (aMapPod54ControlStatusInfoMap.laserTimer != nullptr)
        {
            aMapPod54ControlStatusInfoMap.laserTimer->stop();
            if (!aMapPod54ControlStatusInfoMap.laserTimer->isActive())
            {
                aMapPod54ControlStatusInfoMap.laserTimer->setInterval(1000);
                aMapPod54ControlStatusInfoMap.laserTimer->start();
            }
        }
    }

    logInfo() << tr2("开启连续激光测距");

}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StopContinueLaserTelemeter_clicked()
{
    const auto constFindIter = m_AMapPod54ControlStatusInfoMap.constFind(m_CurrentSensorID);
    if (constFindIter != m_AMapPod54ControlStatusInfoMap.constEnd())
    {
        auto &aMapPod54ControlStatusInfoMap = m_AMapPod54ControlStatusInfoMap[m_CurrentSensorID];
        aMapPod54ControlStatusInfoMap.laserStarted                 = false;
        aMapPod54ControlStatusInfoMap.laserDistanceContinueStarted = false;
        aMapPod54ControlStatusInfoMap.laserDistanceStarted         = false;
        aMapPod54ControlStatusInfoMap.receivedCmdFeedback          = 0;
        aMapPod54ControlStatusInfoMap.receivedLaserDistanceCount = 0;
        aMapPod54ControlStatusInfoMap.laserDistanceTimes = 0;
        aMapPod54ControlStatusInfoMap.laserDistanceCount           = 0;

        if (aMapPod54ControlStatusInfoMap.laserTimer != nullptr)
        {
            aMapPod54ControlStatusInfoMap.laserTimer->stop();
        }

        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StopContinueLaserTelemeter;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();
        logInfo() << tr2("停止连续激光测距") << aMapPod54Control;
    }
    logInfo() << tr2("停止连续激光测距");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StartSelectPointTracking_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    int  selectPointTrackingYawBiasPixel = ui->spinBox_54_SelectPointTrackingYawBiasPixel->value();
    if ((selectPointTrackingYawBiasPixel < -960) || (selectPointTrackingYawBiasPixel > 960))
    {
        return ;
    }

    int  selectPointTrackingPitchBiasPixel = ui->spinBox_54_SelectPointTrackingPitchBiasPixel->value();
    if ((selectPointTrackingPitchBiasPixel < -540) || (selectPointTrackingPitchBiasPixel > 540))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StartSelectPointTracking;
    aMapPod54Control.commonCommandArg.trackMarkOffsetPixels_3A.yawOffsetPixels   = static_cast<int16_t>(selectPointTrackingYawBiasPixel);
    aMapPod54Control.commonCommandArg.trackMarkOffsetPixels_3A.pitchOffsetPixels = static_cast<int16_t>(selectPointTrackingPitchBiasPixel);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("选点跟踪") << aMapPod54Control << selectPointTrackingYawBiasPixel << selectPointTrackingPitchBiasPixel;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_StopTracking_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_StopTracking;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("取消跟踪") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_ShowHideCharacter_clicked()
{

    static AMapPod54Control_CharacterOverlaySet characterOverlay = AMapPod54Control_CharacterOverlaySet::AMapPod54Control_haracterOverlaySet_Hide;
    if (characterOverlay == AMapPod54Control_CharacterOverlaySet::AMapPod54Control_haracterOverlaySet_Hide)
    {
        characterOverlay = AMapPod54Control_CharacterOverlaySet::AMapPod54Control_haracterOverlaySet_ShowPart;
    }
    else if (characterOverlay == AMapPod54Control_CharacterOverlaySet::AMapPod54Control_haracterOverlaySet_ShowPart)
    {
        characterOverlay = AMapPod54Control_CharacterOverlaySet::AMapPod54Control_haracterOverlaySet_ShowAll;
    }
    else if (characterOverlay == AMapPod54Control_CharacterOverlaySet::AMapPod54Control_haracterOverlaySet_ShowAll)
    {
        characterOverlay = AMapPod54Control_CharacterOverlaySet::AMapPod54Control_haracterOverlaySet_Hide;
    }
    else
    {
        characterOverlay = AMapPod54Control_CharacterOverlaySet::AMapPod54Control_haracterOverlaySet_Hide;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_ShowHideCharacter;
    aMapPod54Control.commonCommandArg.characterOverlaySet_37.characterOverlay = characterOverlay;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("字符叠加设置") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InquireRespond_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InquireRespond;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("查询反馈") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_SettingsSaveParameter_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_SettingsSaveParameter;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("设置参数保存") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_Light_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredLightSet;
    aMapPod54Control.commonCommandArg.infraredLightSet_25.infraredLightSet = AMapPod54Control_InfraredLightSet::AMapPod54Control_InfraredLightSet_Light;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外/可见光设置 可见光") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_Infrared_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredLightSet;
    aMapPod54Control.commonCommandArg.infraredLightSet_25.infraredLightSet = AMapPod54Control_InfraredLightSet::AMapPod54Control_InfraredLightSet_Infrared;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外/可见光设置 红外") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredLightSet_InfraredAndLight_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredLightSet;
    aMapPod54Control.commonCommandArg.infraredLightSet_25.infraredLightSet = AMapPod54Control_InfraredLightSet::AMapPod54Control_InfraredLightSet_InfraredLight;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外/可见光设置 红外/可见光设置") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_CommonCommand_AutoFocus_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_AutoFocus;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("自动聚焦") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_CommonCommand_SameView_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_SameView;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("同视场") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_Common_All_Function_Switch_Common_clicked()
{

    m_Widget_54_All_Function = false;

    ui->widget_54_Common_All_Function_Switch_Simple->hide();
    ui->widget_54_Common_All_Function_Switch_Simple->setVisible(false);
    ui->widget_54_Common_All_Function_Switch_Common->show();
    ui->widget_54_Common_All_Function_Switch_Common->setVisible(true);
    ui->widget_54_Common_All_Function_Switch_All->hide();
    ui->widget_54_Common_All_Function_Switch_All->setVisible(false);

    initAMapPodState();

    on_radioButton_JoystickDisEnable_clicked();
    on_radioButton_JoystickDisEnable_Common_clicked();
    ui->radioButton_JoystickDisEnable_Common->setChecked(false);
    ui->radioButton_JoystickDisEnable->setChecked(false);
    ui->radioButton_JoystickDisEnable_Common->setChecked(true);
    ui->radioButton_JoystickDisEnable->setChecked(true);
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_TrendYaw_clicked()
{
    LyWidget *m_pLyVehicleDeflectionDistYawPitchDialog = new LyWidget(tr("Deflection Dist Yaw Pitch Dialog"), LY_AerialShotSettingBundle54Service::Get().getPWnd());
    VehicleDeflectionDistYawPitchDialog *m_pVehicleDeflectionDistYawPitchDialog = new VehicleDeflectionDistYawPitchDialog(m_pLyVehicleDeflectionDistYawPitchDialog);
    if (m_pLyVehicleDeflectionDistYawPitchDialog != nullptr && m_pVehicleDeflectionDistYawPitchDialog != nullptr)
    {
        const auto &strVehicleID   = m_CurrentComponentID.toStdString();
        const auto &strVehicleName = m_CurrentComponentName.toStdString();

        m_pVehicleDeflectionDistYawPitchDialog->setVehicleIDAndName(strVehicleID, strVehicleName);

        m_pLyVehicleDeflectionDistYawPitchDialog->setAttribute(Qt::WA_DeleteOnClose);
        m_pLyVehicleDeflectionDistYawPitchDialog->resize(m_pVehicleDeflectionDistYawPitchDialog->size());
        m_pLyVehicleDeflectionDistYawPitchDialog->setResizable(true);
        QVBoxLayout *verticalLayout = new QVBoxLayout();
        if (verticalLayout)
        {
            verticalLayout->addWidget(m_pVehicleDeflectionDistYawPitchDialog);
            m_pLyVehicleDeflectionDistYawPitchDialog->setLayout(verticalLayout);
        }

        const auto &iter = m_VehicleDeflectionDistYawPitch54Map.find(strVehicleID);
        if (iter != m_VehicleDeflectionDistYawPitch54Map.end())
        {
            m_pVehicleDeflectionDistYawPitchDialog->setVehicleDeflectionDistYawPitchMap(iter->second);
        }
    }

    if (m_pLyVehicleDeflectionDistYawPitchDialog != nullptr)
    {
        m_pLyVehicleDeflectionDistYawPitchDialog->show();
        m_pLyVehicleDeflectionDistYawPitchDialog->raise();
    }
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_CommonCommand_Batch_clicked()
{
    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Warning"), tr("Batch : Are you sure you want to batch?"), QMessageBox::Yes | QMessageBox::No))
    {
        const auto constFindIter = m_AMapPod54ControlStatusInfoMap.constFind(m_CurrentSensorID);
        if (constFindIter != m_AMapPod54ControlStatusInfoMap.constEnd())
        {
            auto &aMapPod54ControlStatusInfoMap = m_AMapPod54ControlStatusInfoMap[m_CurrentSensorID];
            aMapPod54ControlStatusInfoMap.targetBatchTime              = 0;
            aMapPod54ControlStatusInfoMap.targetBatchStart             = !aMapPod54ControlStatusInfoMap.targetBatchStart;
            aMapPod54ControlStatusInfoMap.targetBatch                  = true;

            if (aMapPod54ControlStatusInfoMap.targetBatchStart)
            {
            }
            else
            {
            }

            if (aMapPod54ControlStatusInfoMap.targetBatchStart)
            {
            }
            else
            {
            }
        }
    }
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_PayLoadStartWorking_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_PayLoadStartWorking;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("任务载荷工作") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_PayLoadStopWorking_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_PayLoadStopWorking;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("任务载荷休眠") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_TrackTargetTemplate_16X16_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_TrackMarkTemplateSet;
    aMapPod54Control.commonCommandArg.trackMarkTempletSet_23.trackMarkTemplet = AMapPod54Control_TrackMarkTempletSet::AMapPod54Control_TrackMarkTempletSet_16X16;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("跟踪目标小模板(16X16)") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_TrackTargetTemplate_32X32_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_TrackMarkTemplateSet;
    aMapPod54Control.commonCommandArg.trackMarkTempletSet_23.trackMarkTemplet = AMapPod54Control_TrackMarkTempletSet::AMapPod54Control_TrackMarkTempletSet_32X32;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("跟踪目标小模板(32X32)") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_TrackTargetTemplate_64X64_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_TrackMarkTemplateSet;
    aMapPod54Control.commonCommandArg.trackMarkTempletSet_23.trackMarkTemplet = AMapPod54Control_TrackMarkTempletSet::AMapPod54Control_TrackMarkTempletSet_64X64;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("跟踪目标小模板(64X64)") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_TrackTargetTemplate_128X128_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_TrackMarkTemplateSet;
    aMapPod54Control.commonCommandArg.trackMarkTempletSet_23.trackMarkTemplet = AMapPod54Control_TrackMarkTempletSet::AMapPod54Control_TrackMarkTempletSet_128X128;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("跟踪目标小模板(128X128)") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_TrackTargetTemplate_AutoSet_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    int trackTargetTemplate_AutoSet_WidthPixel = ui->spinBox_54_TrackTargetTemplate_AutoSet_WidthPixel->value();
    if ((trackTargetTemplate_AutoSet_WidthPixel < 0) || (trackTargetTemplate_AutoSet_WidthPixel > 255))
    {
        return ;
    }

    int trackTargetTemplate_AutoSet_HeightPixel = ui->spinBox_54_TrackTargetTemplate_AutoSet_HeightPixel->value();
    if ((trackTargetTemplate_AutoSet_HeightPixel < 0) || (trackTargetTemplate_AutoSet_HeightPixel > 255))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_TrackMarkTemplateSet;
    aMapPod54Control.commonCommandArg.trackMarkTempletSet_23.trackMarkTemplet = AMapPod54Control_TrackMarkTempletSet::AMapPod54Control_TrackMarkTempletSet_AutoSet;
    aMapPod54Control.commonCommandArg.trackMarkTempletSet_23.trackMarkTempletWidth  = static_cast<uint8_t>(trackTargetTemplate_AutoSet_WidthPixel);
    aMapPod54Control.commonCommandArg.trackMarkTempletSet_23.trackMarkTempletHeight = static_cast<uint8_t>(trackTargetTemplate_AutoSet_HeightPixel);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("跟踪目标模板自定义设置 使用 6~7字节设置大小") << aMapPod54Control << trackTargetTemplate_AutoSet_WidthPixel << trackTargetTemplate_AutoSet_HeightPixel;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalCenter_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalCenter;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("云台归中") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalLookDownVertical_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalLookDownVertical;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("垂直下视") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalYawLock_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalLock;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("方位锁定") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_YawFollow_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_YawFollow;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("方位弱随动机头") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_CollectMode_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_CollectMode;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("收藏模式") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalMoterClose_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalMoterClose;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("关闭伺服") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_NumberFollow_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    double numberFollowYaw = ui->doubleSpinBox_54_NumberFollowYaw->value();
    if ((numberFollowYaw < -180) || (numberFollowYaw > 180))
    {
        return ;
    }

    double numberFollowPitch = ui->doubleSpinBox_54_NumberFollowPitch->value();
    if ((numberFollowPitch < -120) || (numberFollowPitch > 90))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_NumberFollow;
    aMapPod54Control.commonCommandArg.countModelSet_72.yaw   = static_cast<int16_t>(numberFollowYaw * 100);
    aMapPod54Control.commonCommandArg.countModelSet_72.pitch = static_cast<int16_t>(numberFollowPitch * 100);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("数引模式") << aMapPod54Control << numberFollowYaw << numberFollowPitch;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalRunScan_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalRunScan;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("云台扫描") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalScanParameterSet_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    uint16_t gimbalScanParameterAngle = static_cast<uint16_t>(ui->spinBox_54_GimbalScanParameterAngle->value());
    if ((gimbalScanParameterAngle < 0) || (gimbalScanParameterAngle > 360))
    {
        return ;
    }

    uint16_t gimbalScanParameterSpeed = static_cast<uint16_t>(ui->spinBox_54_GimbalScanParameterSpeed->value());
    if ((gimbalScanParameterSpeed < 0) || (gimbalScanParameterSpeed > 100))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalScanParameterSet;
    aMapPod54Control.commonCommandArg.podScannArgSet_77.scannAngle = gimbalScanParameterAngle;
    aMapPod54Control.commonCommandArg.podScannArgSet_77.scannSpeed = gimbalScanParameterSpeed;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("吊舱扫描参数设置") << aMapPod54Control << gimbalScanParameterAngle << gimbalScanParameterSpeed;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalGyroscopeExcursionAutoCalibrate_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalGyroscopeExcursionCalibrate;
    aMapPod54Control.commonCommandArg.gyroscopeCalibrateSet_76.gyroscopeCalibrateModel = AMapPod54Control_GyroscopeCalibrateModel::AMapPod54Control_GyroscopeCalibrateModel_AutoSet;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("陀螺漂移自动校准") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalGyroscopeExcursionManualCalibrate_clicked()
{
    if (ui == nullptr)
    {
        return ;
    }

    int gimbalGyroscopeExcursionManualCalibrate_Yaw_Left = ui->spinBox_54_GimbalGyroscopeExcursionManualCalibrate_Yaw_Left->value();
    if ((gimbalGyroscopeExcursionManualCalibrate_Yaw_Left < 0) || (gimbalGyroscopeExcursionManualCalibrate_Yaw_Left > 360))
    {
        return ;
    }

    int gimbalGyroscopeExcursionManualCalibrate_Pitch_Right = ui->spinBox_54_GimbalGyroscopeExcursionManualCalibrate_Pitch_Right->value();
    if ((gimbalGyroscopeExcursionManualCalibrate_Pitch_Right < 0) || (gimbalGyroscopeExcursionManualCalibrate_Pitch_Right > 100))
    {
        return ;
    }

    int gimbalGyroscopeExcursionManualCalibrate_Pitch = ui->spinBox_54_GimbalGyroscopeExcursionManualCalibrate_Pitch->value();
    if ((gimbalGyroscopeExcursionManualCalibrate_Pitch < -120) || (gimbalGyroscopeExcursionManualCalibrate_Pitch > 90))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalGyroscopeExcursionCalibrate;
    aMapPod54Control.commonCommandArg.gyroscopeCalibrateSet_76.gyroscopeCalibrateModel = AMapPod54Control_GyroscopeCalibrateModel::AMapPod54Control_GyroscopeCalibrateModel_Manual;
    aMapPod54Control.commonCommandArg.gyroscopeCalibrateSet_76.adjustmentAmount.adjustmentAmount1 = static_cast<int8_t>(gimbalGyroscopeExcursionManualCalibrate_Yaw_Left);
    aMapPod54Control.commonCommandArg.gyroscopeCalibrateSet_76.adjustmentAmount.adjustmentAmount2 = static_cast<int8_t>(gimbalGyroscopeExcursionManualCalibrate_Pitch_Right);
    aMapPod54Control.commonCommandArg.gyroscopeCalibrateSet_76.pitch = static_cast<int16_t>(gimbalGyroscopeExcursionManualCalibrate_Pitch);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("陀螺漂移校准 未实现") << aMapPod54Control << gimbalGyroscopeExcursionManualCalibrate_Yaw_Left << gimbalGyroscopeExcursionManualCalibrate_Pitch_Right << gimbalGyroscopeExcursionManualCalibrate_Pitch;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_GimbalGyroscopeExcursionTemperatureCalibrate_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    int gimbalGyroscopeExcursionManualCalibrate_Yaw_Left = ui->spinBox_54_GimbalGyroscopeExcursionManualCalibrate_Yaw_Left->value();
    if ((gimbalGyroscopeExcursionManualCalibrate_Yaw_Left < 0) || (gimbalGyroscopeExcursionManualCalibrate_Yaw_Left > 360))
    {
        return ;
    }

    int gimbalGyroscopeExcursionManualCalibrate_Pitch_Right = ui->spinBox_54_GimbalGyroscopeExcursionManualCalibrate_Pitch_Right->value();
    if ((gimbalGyroscopeExcursionManualCalibrate_Pitch_Right < 0) || (gimbalGyroscopeExcursionManualCalibrate_Pitch_Right > 100))
    {
        return ;
    }

    int gimbalGyroscopeExcursionManualCalibrate_Pitch = ui->spinBox_54_GimbalGyroscopeExcursionManualCalibrate_Pitch->value();
    if ((gimbalGyroscopeExcursionManualCalibrate_Pitch < -120) || (gimbalGyroscopeExcursionManualCalibrate_Pitch > 90))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_GimbalGyroscopeExcursionCalibrate;
    aMapPod54Control.commonCommandArg.gyroscopeCalibrateSet_76.gyroscopeCalibrateModel = AMapPod54Control_GyroscopeCalibrateModel::AMapPod54Control_GyroscopeCalibrateModel_Temperature;
    aMapPod54Control.commonCommandArg.gyroscopeCalibrateSet_76.adjustmentAmount.adjustmentAmount1 = static_cast<int8_t>(gimbalGyroscopeExcursionManualCalibrate_Yaw_Left);
    aMapPod54Control.commonCommandArg.gyroscopeCalibrateSet_76.adjustmentAmount.adjustmentAmount2 = static_cast<int8_t>(gimbalGyroscopeExcursionManualCalibrate_Pitch_Right);
    aMapPod54Control.commonCommandArg.gyroscopeCalibrateSet_76.pitch = static_cast<int16_t>(gimbalGyroscopeExcursionManualCalibrate_Pitch);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("陀螺校准 陀螺温度校准") << aMapPod54Control << gimbalGyroscopeExcursionManualCalibrate_Yaw_Left << gimbalGyroscopeExcursionManualCalibrate_Pitch_Right << gimbalGyroscopeExcursionManualCalibrate_Pitch;

}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_AttitudeGuideFollow_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    double attitudeGuideFollow_Yaw = ui->doubleSpinBox_54_AttitudeGuideFollow_Yaw->value();
    if ((attitudeGuideFollow_Yaw < -180.0) || (attitudeGuideFollow_Yaw > 180.0))
    {
        return ;
    }

    double attitudeGuideFollow_Pitch = ui->doubleSpinBox_54_AttitudeGuideFollow_Pitch->value();
    if ((attitudeGuideFollow_Pitch < -120.0) || (attitudeGuideFollow_Pitch > 90.0))
    {
        return ;
    }

    int16_t yaw   = static_cast<int16_t>(attitudeGuideFollow_Yaw   * 100);
    int16_t pitch = static_cast<int16_t>(attitudeGuideFollow_Pitch * 100);

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_AttitudeGuideFollow;
    aMapPod54Control.commonCommandArg.AttitudeLeadSet_7C.yaw   = yaw;
    aMapPod54Control.commonCommandArg.AttitudeLeadSet_7C.pitch = pitch;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("姿态指引") << aMapPod54Control << yaw << pitch;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredShutterCompensate_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredShutterCompensate;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外快门补偿") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_SceneSupplement_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_SceneSupplement;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("场景补偿") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredGraphicsStrengthen_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredGraphicsStrengthen;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外图像增强") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredCutoverRibbonSwitch_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredCutoverRibbonSwitch;
    aMapPod54Control.commonCommandArg.falseColorBandSwitchSet_53.falseColorBandSwitch = AMapPod54Control_FalseColorBandSwitch::AMapPod54Control_FalseColorBandSwitch_AutoSet;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("伪彩色带切换 循环切换") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredCutoverRibbonSet_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    const int currentIndex = ui->comboBox_54_InfraredCutoverRibbonSet->currentIndex();

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredCutoverRibbonSwitch;
    aMapPod54Control.commonCommandArg.falseColorBandSwitchSet_53.falseColorBandSwitch = AMapPod54Control_FalseColorBandSwitch::AMapPod54Control_FalseColorBandSwitch_ManualSet;
    aMapPod54Control.commonCommandArg.falseColorBandSwitchSet_53.falseColorBandModel  = AMapPod54Control_FalseColorBandModel_FromIndex(currentIndex);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("伪彩色带切换 指定伪彩色带切换") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusStop_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredFocus;
    aMapPod54Control.commonCommandArg.infraredFocusModel_50.focusModel = AMapPod54Control_InfraredFocusModel::AMapPod54Control_InfraredFocus_FocusStop;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外 停止聚焦") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusFar_pressed()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredFocus;
    aMapPod54Control.commonCommandArg.infraredFocusModel_50.focusModel = AMapPod54Control_InfraredFocusModel::AMapPod54Control_InfraredFocus_FocusFar;

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("1 : 红外 聚焦 远 按钮按下 ") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerFunction = [this]()
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredFocus;
        aMapPod54Control.commonCommandArg.infraredFocusModel_50.focusModel = AMapPod54Control_InfraredFocusModel::AMapPod54Control_InfraredFocus_FocusFar;

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("1 : 红外 聚焦 远 按钮按下 ") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54Timer.start();

    logInfo() << tr2("1 : 红外 聚焦 远 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusFar_released()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_InfraredFocus_FocusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("1 : 红外 聚焦 远 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusFar_clicked()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_InfraredFocus_FocusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("1 : 红外 聚焦 远 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusNear_pressed()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredFocus;
    aMapPod54Control.commonCommandArg.infraredFocusModel_50.focusModel = AMapPod54Control_InfraredFocusModel::AMapPod54Control_InfraredFocus_FocusNear;

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("2 : 红外 聚焦 近 按钮按下 ") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerFunction = [this]()
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredFocus;
        aMapPod54Control.commonCommandArg.infraredFocusModel_50.focusModel = AMapPod54Control_InfraredFocusModel::AMapPod54Control_InfraredFocus_FocusNear;

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("2 : 红外 聚焦 近 按钮按下 ") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54Timer.start();

    logInfo() << tr2("2 : 红外 聚焦 近 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusNear_released()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_InfraredFocus_FocusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("2 : 红外 聚焦 近 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusNear_clicked()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_InfraredFocus_FocusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("2 : 红外 聚焦 近 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredFocus_FocusAuto_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredFocus;
    aMapPod54Control.commonCommandArg.infraredFocusModel_50.focusModel = AMapPod54Control_InfraredFocusModel::AMapPod54Control_InfraredFocus_FocusAuto;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外 自动聚焦") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleOut_pressed()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredMultiple;
    aMapPod54Control.commonCommandArg.infraredElectronicZoomSet_54.infraredElectronicZoomModel = AMapPod54Control_InfraredElectronicZoomModel::AMapPod54Control_InfraredElectronicZoomModel_ZoomPlus;

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("红外载荷 电子变倍+ 按钮按下 ") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerFunction = [this]()
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredMultiple;
        aMapPod54Control.commonCommandArg.infraredElectronicZoomSet_54.infraredElectronicZoomModel = AMapPod54Control_InfraredElectronicZoomModel::AMapPod54Control_InfraredElectronicZoomModel_ZoomPlus;

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("红外载荷 电子变倍+ 按钮按下 ") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54Timer.start();

    logInfo() << tr2("红外载荷 电子变倍+ 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleOut_released()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外载荷 电子变倍+ 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleOut_clicked()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外载荷 电子变倍+ 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleIn_pressed()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredMultiple;
    aMapPod54Control.commonCommandArg.infraredElectronicZoomSet_54.infraredElectronicZoomModel = AMapPod54Control_InfraredElectronicZoomModel::AMapPod54Control_InfraredElectronicZoomModel_ZoomSub;

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("红外载荷 电子变倍- 按钮按下 ") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerFunction = [this]()
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredMultiple;
        aMapPod54Control.commonCommandArg.infraredElectronicZoomSet_54.infraredElectronicZoomModel = AMapPod54Control_InfraredElectronicZoomModel::AMapPod54Control_InfraredElectronicZoomModel_ZoomSub;

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("红外载荷 电子变倍- 按钮按下 ") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54Timer.start();

    logInfo() << tr2("红外载荷 电子变倍- 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleIn_released()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外载荷 电子变倍- 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleIn_clicked()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外载荷 电子变倍- 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_InfraredMultipleSet_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    int infraredMultipleSet = ui->spinBox_54_InfraredMultipleSet->value();
    if ((infraredMultipleSet < 10) || (infraredMultipleSet > 40))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_InfraredMultiple;
    aMapPod54Control.commonCommandArg.infraredElectronicZoomSet_54.infraredElectronicZoomModel     = AMapPod54Control_InfraredElectronicZoomModel::AMapPod54Control_InfraredElectronicZoomModel_ZoomSet;
    aMapPod54Control.commonCommandArg.infraredElectronicZoomSet_54.infraredElectronicZoomMultiplel = static_cast<uint8_t>(infraredMultipleSet);

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("红外载荷 第五字节指定变倍倍数") << aMapPod54Control << aMapPod54Control.commonCommandArg.infraredElectronicZoomSet_54.infraredElectronicZoomMultiplel;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleOut_pressed()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
    aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomPlus;

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("可见光载荷 变倍+ 按钮按下 LigthMultipleOut_pressed") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerFunction = [this]()
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
        aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomPlus;

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("可见光载荷 变倍+ 按钮按下 LigthMultipleOut_pressed") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54Timer.start();

    logInfo() << tr2("可见光载荷 变倍+ 按钮按下 LigthMultipleOut_pressed");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleOut_released()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_LigthZoomFacusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 变倍+ 按钮释放 发送停止 空指令 LigthMultipleOut_released");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleOut_clicked()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_LigthZoomFacusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 变倍+ 按钮单击 发送停止 空指令 LigthMultipleOut_clicked");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleIn_pressed()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
    aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomSub;

    SendAMapPodControlMsg(aMapPod54Control);
    logInfo() << tr2("可见光载荷 变倍- 按钮按下 ") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerFunction = [this]()
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
        aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomSub;

        SendAMapPodControlMsg(aMapPod54Control);
        logInfo() << tr2("可见光载荷 变倍- 按钮按下 ") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54Timer.start();

    logInfo() << tr2("可见光载荷 变倍- 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleIn_released()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_LigthZoomFacusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 变倍- 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthMultipleIn_clicked()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_LigthZoomFacusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 变倍- 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusOut_pressed()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
    aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomFocusPlus;

    SendAMapPodControlMsg(aMapPod54Control);
    logInfo() << tr2("可见光载荷 调焦+ 按钮按下 ") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerFunction = [this]()
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
        aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomFocusPlus;

        SendAMapPodControlMsg(aMapPod54Control);
        logInfo() << tr2("可见光载荷 调焦+ 按钮按下 ") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54Timer.start();

    logInfo() << tr2("可见光载荷 调焦+ 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusOut_released()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_LigthZoomFacusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 调焦+ 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusOut_clicked()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_LigthZoomFacusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 调焦+ 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusIn_pressed()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
    aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomFocusSub;

    SendAMapPodControlMsg(aMapPod54Control);

    logInfo() << tr2("见光载荷 调焦- 按钮按下 ") << aMapPod54Control;

    m_AerialShotSettingBundle54TimerFunction = [this]()
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
        aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomFocusSub;

        SendAMapPodControlMsg(aMapPod54Control);

        logInfo() << tr2("见光载荷 调焦- 按钮按下 ") << aMapPod54Control;
    };
    m_AerialShotSettingBundle54Timer.start();

    logInfo() << tr2("见光载荷 调焦- 按钮按下");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusIn_released()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_LigthZoomFacusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 调焦- 按钮释放 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusIn_clicked()
{

    m_AerialShotSettingBundle54Timer.stop();
    m_AerialShotSettingBundle54TimerFunction = nullptr;

    on_pushButton_54_LigthZoomFacusStop_clicked();

    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();
    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 调焦- 按钮单击 发送停止 空指令");
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusAuto_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
    aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomFocusAuto;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 自动聚焦") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthElectChange_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthElectChange;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 电子变倍") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthZoomFacusStop_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthMultiple;
    aMapPod54Control.commonCommandArg.lightZoomSet_45.lightZoomModel = AMapPod54Control_LightZoomModel::AMapPod54Control_LightZoomModel_ZoomFocusStop;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 停止变倍 调焦") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthLowilluminationClose_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthLowillumination;
    aMapPod54Control.commonCommandArg.ligthLowilluminationSet_4B.ligthLowilluminationModel = AMapPod54Control_LigthLowilluminationModel::AMapPod54Control_LigthLowilluminationModel_Close;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("见光载荷 低照度关") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthLowilluminationOpen_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthLowillumination;
    aMapPod54Control.commonCommandArg.ligthLowilluminationSet_4B.ligthLowilluminationModel = AMapPod54Control_LigthLowilluminationModel::AMapPod54Control_LigthLowilluminationModel_Open;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("见光载荷 低照度开") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFogClose_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthFog;
    aMapPod54Control.commonCommandArg.lightThroughFogSet_4A.lightThroughFogModel = AMapPod54Control_LightThroughFogModel::AMapPod54Control_LightThroughFogModel_Close;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 可见透雾模式 关闭") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthShoot_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthShoot;
    aMapPod54Control.commonCommandArg.ligthShoot_44.ligthShoot = static_cast<uint16_t>(ui->spinBox_54_LigthShoot->value());

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("开始停止连拍") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthContrastAdjust_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthContrastAdjust;
    aMapPod54Control.commonCommandArg.ligthContrastAdjust_49.ligthContrastAdjust = static_cast<uint8_t>(ui->spinBox_54_LigthContrastAdjust->value());

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("对比度调节") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFogLow_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthFog;
    aMapPod54Control.commonCommandArg.lightThroughFogSet_4A.lightThroughFogModel = AMapPod54Control_LightThroughFogModel::AMapPod54Control_LightThroughFogModel_Low;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 可见透雾模式 低") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFogMiddle_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthFog;
    aMapPod54Control.commonCommandArg.lightThroughFogSet_4A.lightThroughFogModel = AMapPod54Control_LightThroughFogModel::AMapPod54Control_LightThroughFogModel_Middle;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 可见透雾模式 中") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFogStrong_clicked()
{

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthFog;
    aMapPod54Control.commonCommandArg.lightThroughFogSet_4A.lightThroughFogModel = AMapPod54Control_LightThroughFogModel::AMapPod54Control_LightThroughFogModel_Strong;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 可见透雾模式 强") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_pushButton_54_LigthFacusSet_clicked()
{

    if (ui == nullptr)
    {
        return ;
    }

    double ligthFacusSet = ui->doubleSpinBox_54_LigthFacusSet->value() * 10;
    if ((ligthFacusSet < 43) || (ligthFacusSet > 1290))
    {
        return ;
    }

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.commonCommand = AMapPod54Control_CommonCommand::AMapPod54Control_CommonCommand_LigthFacus;
    aMapPod54Control.commonCommandArg.lightFacusSet_4C.lightFacus = ligthFacusSet;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("可见光载荷 焦距设置") << aMapPod54Control << aMapPod54Control.commonCommandArg.lightFacusSet_4C.lightFacus;
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnable_clicked()
{

    ui->label_JoystickWidget->setAxisVisible(true);
    m_Joy_FineTuning = false;

    if (m_JoystickEnableGimbal)
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("启用摇杆") << aMapPod54Control;
    }
    else
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("选取跟踪目标坐标值") << aMapPod54Control;
    }
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnableFine_clicked()
{

    ui->label_JoystickWidget->setAxisVisible(true);
    m_Joy_FineTuning = true;

    if (m_JoystickEnableGimbal)
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("微调 启用摇杆") << aMapPod54Control;
    }
    else
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("微调 选取跟踪目标坐标值") << aMapPod54Control;
    }
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnableMouse_clicked()
{

    ui->label_JoystickWidget->setAxisMouse(true);
    m_Joy_FineTuning = false;

    if (m_JoystickEnableGimbal)
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Enable;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("鼠标 启用摇杆") << aMapPod54Control;
    }
    else
    {
        AMapPod54Control aMapPod54Control;
        memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
        aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_SelectMarkPoint;

        SendAMapPodControlMsg(aMapPod54Control);

        SendAMapPodControlZeroMsg();

        logInfo() << tr2("鼠标 选取跟踪目标坐标值") << aMapPod54Control;
    }
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickDisEnable_clicked()
{

    ui->label_JoystickWidget->setAxisMouse(false);
    ui->label_JoystickWidget->setAxisVisible(false);
    m_Joy_FineTuning = false;

    AMapPod54Control aMapPod54Control;
    memset(&aMapPod54Control, 0x00, sizeof(aMapPod54Control));
    aMapPod54Control.PTZJoyStickControl = AMapPod54Control_PTZJoyStickControl::AMapPod54Control_PTZJoyStickControl_Disable;

    SendAMapPodControlMsg(aMapPod54Control);

    SendAMapPodControlZeroMsg();

    logInfo() << tr2("禁用摇杆") << aMapPod54Control;
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnableGimbal_clicked()
{

    m_JoystickEnableGimbal = true;

    ui->radioButton_JoystickEnableGimbal_Common->setCheckable(true);
}

void IAerialShotSettingBundle54Dialog::on_radioButton_JoystickEnableTracking_clicked()
{

    m_JoystickEnableGimbal = false;

    ui->radioButton_JoystickEnableTracking_Common->setCheckable(true);
}

