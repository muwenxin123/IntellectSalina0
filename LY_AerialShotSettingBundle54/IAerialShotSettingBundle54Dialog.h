#ifndef IAERIALSHOTSETTINGBUNDLE54DIALOG_H
#define IAERIALSHOTSETTINGBUNDLE54DIALOG_H

#include <QWidget>
#include <QTimer>
#include <QButtonGroup>
#include <QSettings>

#include <functional>

#include "Protocol/XygsZC54/Bag_XygsZC54Defines.h"
#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"
#include "Protocol/XygsZC/Bag_XygsZCDefines.h"

#include "IAerialShotSettingBundle54Activator.h"
namespace Ui
{
class IAerialShotSettingBundle54Dialog;
}

class IAerialShotSettingBundle54Dialog : public QWidget
{
    Q_OBJECT

public:
    explicit IAerialShotSettingBundle54Dialog(QWidget *parent = nullptr);
    ~IAerialShotSettingBundle54Dialog();

    void InitWeaponInfoList();
    void on_Message_Recv(const QString strSensorID, const QByteArray &message);
    const QString GetComponentID(const QString &strSensorID);

    const std::map<std::string, std::vector< std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > > > > &getVehicleDeflectionDistYawPitch54Map() const;
    void setVehicleDeflectionDistYawPitch54Map(const std::string &strVehicleID, const std::vector<std::pair<unsigned int, std::list<std::pair<unsigned int, std::pair<double, double> > > > > &vehicleDeflectionDistYawPitch54Map);
    void getVehicleDeflectionDistYawPitch54Value(const std::string &strVehicleID);
    void getVehicleDeflectionDistYawPitch54ZoomDistanceValue(const std::string &strVehicleID, const QString &strSensorID, float &deflectionAngleofNose, float &deflectionAngleofPitch);

    void CommadAMapPodControlMsg(AMapPod54Control &aMapPod54Control, const QString strComponentID, const QString strSensorID);

signals:
    void ShowAMapPodStatusInfoSig(const QString strSensorID, const QByteArray message);
    void AutoAmimingSig(const QString strComponentID, const QByteArray data);
signals:
    void joystickAxisAerialShotSettingBundle54(long int lX, long int lY, long int lYoumen = 0, long int lFangwei = 0);
    void mouseReleaseEventSigAerialShotSettingBundle54(long int lX, long int lY, long int lYoumen = 0, long int lFangwei = 0);

private slots:
    void on_Button_54_GroupComponent_buttonClicked(QAbstractButton *button);

    void on_pushButton_54_InfraredLightSet_Light_Simple_clicked();

    void on_pushButton_54_InfraredLightSet_Infrared_Simple_clicked();

    void on_pushButton_54_InfraredLightSet_InfraredAndLight_Simple_clicked();

    void on_pushButton_54_SingleLaserTelemeter_Simple_clicked();

    void on_pushButton_54_StartContinueLaserTelemeter_Simple_clicked();

    void on_pushButton_54_StopContinueLaserTelemeter_Simple_clicked();

    void on_pushButton_54_GimbalCenter_Simple_clicked();

    void on_pushButton_54_GimbalLookDownVertical_Simple_clicked();

    void on_pushButton_54_GimbalYawLock_Common_Simple_clicked();

    void on_pushButton_54_AttitudeGuideFollow_Simple_clicked();

    void on_pushButton_54_LigthMultipleOut_Simple_pressed();

    void on_pushButton_54_LigthMultipleOut_Simple_released();

    void on_pushButton_54_LigthMultipleOut_Simple_clicked();

    void on_pushButton_54_LigthMultipleIn_Simple_pressed();

    void on_pushButton_54_LigthMultipleIn_Simple_released();

    void on_pushButton_54_LigthMultipleIn_Simple_clicked();

    void on_pushButton_54_LigthFacusAuto_Simple_clicked();

    void on_pushButton_54_InfraredShutterCompensate_Simple_clicked();

    void on_pushButton_54_InfraredFocus_FocusAuto_Simple_clicked();

    void on_pushButton_54_InfraredCutoverRibbonSet_Simple_clicked();

    void on_pushButton_54_JoystickControl_Left_Up_Simple_pressed();

    void on_pushButton_54_JoystickControl_Left_Up_Simple_released();

    void on_pushButton_54_JoystickControl_Left_Up_Simple_clicked();

    void on_pushButton_54_JoystickControl_Up_Simple_pressed();

    void on_pushButton_54_JoystickControl_Up_Simple_released();

    void on_pushButton_54_JoystickControl_Up_Simple_clicked();

    void on_pushButton_54_JoystickControl_Right_Up_Simple_pressed();

    void on_pushButton_54_JoystickControl_Right_Up_Simple_released();

    void on_pushButton_54_JoystickControl_Right_Up_Simple_clicked();

    void on_pushButton_54_JoystickControl_Left_Simple_pressed();

    void on_pushButton_54_JoystickControl_Left_Simple_released();

    void on_pushButton_54_JoystickControl_Left_Simple_clicked();

    void on_pushButton_54_JoystickControl_Right_Simple_pressed();

    void on_pushButton_54_JoystickControl_Right_Simple_released();

    void on_pushButton_54_JoystickControl_Right_Simple_clicked();

    void on_pushButton_54_JoystickControl_Left_Down_Simple_pressed();

    void on_pushButton_54_JoystickControl_Left_Down_Simple_released();

    void on_pushButton_54_JoystickControl_Left_Down_Simple_clicked();

    void on_pushButton_54_JoystickControl_Down_Simple_pressed();

    void on_pushButton_54_JoystickControl_Down_Simple_released();

    void on_pushButton_54_JoystickControl_Down_Simple_clicked();

    void on_pushButton_54_JoystickControl_Right_Down_Simple_pressed();

    void on_pushButton_54_JoystickControl_Right_Down_Simple_released();

    void on_pushButton_54_JoystickControl_Right_Down_Simple_clicked();

    void on_pushButton_54_InfraredLightSet_Batch_Simple_clicked();

    void on_pushButton_54_Simple_Common_All_Function_Switch_Common_clicked();

    void on_pushButton_54_InfraredLightSet_Light_Common_clicked();

    void on_pushButton_54_InfraredLightSet_Infrared_Common_clicked();

    void on_pushButton_54_InfraredLightSet_InfraredAndLight_Common_clicked();

    void on_pushButton_54_StartStopContinueGrab_Common_clicked();

    void on_pushButton_54_Grab_Common_clicked();

    void on_pushButton_54_StartStopRecodeVideo_Common_clicked();

    void on_pushButton_54_CommonCommand_AutoFocus_Common_clicked();

    void on_pushButton_54_SingleLaserTelemeter_Common_clicked();

    void on_pushButton_54_StartContinueLaserTelemeter_Common_clicked();

    void on_pushButton_54_StopContinueLaserTelemeter_Common_clicked();

    void on_pushButton_54_CommonCommand_SameView_Common_clicked();

    void on_pushButton_54_CommonCommand_Batch_Common_clicked();

    void on_pushButton_54_Common_All_Function_Switch_All_clicked();

    void on_pushButton_54_Common_All_Function_Switch_Simple_clicked();

    void on_pushButton_54_GimbalCenter_Common_clicked();

    void on_pushButton_54_GimbalLookDownVertical_Common_clicked();

    void on_pushButton_54_GimbalYawLock_Common_clicked();

    void on_pushButton_54_YawFollow_Common_clicked();

    void on_pushButton_54_CollectMode_Common_clicked();

    void on_pushButton_54_GimbalMoterClose_Common_clicked();

    void on_pushButton_54_NumberFollow_Common_clicked();

    void on_pushButton_54_AttitudeGuideFollow_Common_clicked();

    void on_pushButton_54_LigthMultipleOut_Common_pressed();

    void on_pushButton_54_LigthMultipleOut_Common_released();

    void on_pushButton_54_LigthMultipleOut_Common_clicked();

    void on_pushButton_54_LigthMultipleIn_Common_pressed();

    void on_pushButton_54_LigthMultipleIn_Common_released();

    void on_pushButton_54_LigthMultipleIn_Common_clicked();

    void on_pushButton_54_LigthFacusOut_Common_pressed();

    void on_pushButton_54_LigthFacusOut_Common_released();

    void on_pushButton_54_LigthFacusOut_Common_clicked();

    void on_pushButton_54_LigthFacusIn_Common_pressed();

    void on_pushButton_54_LigthFacusIn_Common_released();

    void on_pushButton_54_LigthFacusIn_Common_clicked();

    void on_pushButton_54_LigthLowilluminationClose_Common_clicked();

    void on_pushButton_54_LigthLowilluminationOpen_Common_clicked();

    void on_pushButton_54_LigthZoomFacusStop_Common_clicked();

    void on_pushButton_54_LigthFacusAuto_Common_clicked();

    void on_pushButton_54_LigthElectChange_Common_clicked();

    void on_pushButton_54_LigthFacusSet_Common_clicked();

    void on_pushButton_54_LigthShoot_Common_clicked();

    void on_pushButton_54_InfraredShutterCompensate_Common_clicked();

    void on_pushButton_54_SceneSupplement_Common_clicked();

    void on_pushButton_54_InfraredGraphicsStrengthen_Common_clicked();

    void on_pushButton_54_InfraredFocus_FocusFar_Common_pressed();

    void on_pushButton_54_InfraredFocus_FocusFar_Common_released();

    void on_pushButton_54_InfraredFocus_FocusFar_Common_clicked();

    void on_pushButton_54_InfraredFocus_FocusNear_Common_pressed();

    void on_pushButton_54_InfraredFocus_FocusNear_Common_released();

    void on_pushButton_54_InfraredFocus_FocusNear_Common_clicked();

    void on_pushButton_54_InfraredFocus_FocusStop_Common_clicked();

    void on_pushButton_54_InfraredFocus_FocusAuto_Common_clicked();

    void on_pushButton_54_InfraredMultipleOut_Common_pressed();

    void on_pushButton_54_InfraredMultipleOut_Common_released();

    void on_pushButton_54_InfraredMultipleOut_Common_clicked();

    void on_pushButton_54_InfraredMultipleIn_Common_pressed();

    void on_pushButton_54_InfraredMultipleIn_Common_released();

    void on_pushButton_54_InfraredMultipleIn_Common_clicked();

    void on_pushButton_54_InfraredCutoverRibbonSet_Common_clicked();

    void on_radioButton_JoystickEnable_Common_clicked();

    void on_radioButton_JoystickEnableFine_Common_clicked();

    void on_radioButton_JoystickEnableMouse_Common_clicked();

    void on_radioButton_JoystickDisEnable_Common_clicked();

    void on_radioButton_JoystickEnableGimbal_Common_clicked();

    void on_radioButton_JoystickEnableTracking_Common_clicked();

    void on_pushButton_54_InfraredLightSwitch_clicked();

    void on_pushButton_54_Grab_clicked();

    void on_pushButton_54_StartStopRecodeVideo_clicked();

    void on_pushButton_54_StartStopContinueGrab_clicked();

    void on_pushButton_54_SingleLaserTelemeter_clicked();

    void on_pushButton_54_StartContinueLaserTelemeter_clicked();

    void on_pushButton_54_StopContinueLaserTelemeter_clicked();

    void on_pushButton_54_StartSelectPointTracking_clicked();

    void on_pushButton_54_StopTracking_clicked();

    void on_pushButton_54_ShowHideCharacter_clicked();

    void on_pushButton_54_InquireRespond_clicked();

    void on_pushButton_54_SettingsSaveParameter_clicked();

    void on_pushButton_54_InfraredLightSet_Light_clicked();

    void on_pushButton_54_InfraredLightSet_Infrared_clicked();

    void on_pushButton_54_InfraredLightSet_InfraredAndLight_clicked();

    void on_pushButton_54_CommonCommand_AutoFocus_clicked();

    void on_pushButton_54_CommonCommand_SameView_clicked();

    void on_pushButton_54_Common_All_Function_Switch_Common_clicked();

    void on_pushButton_54_TrendYaw_clicked();

    void on_pushButton_54_CommonCommand_Batch_clicked();

    void on_pushButton_54_PayLoadStartWorking_clicked();

    void on_pushButton_54_PayLoadStopWorking_clicked();

    void on_pushButton_54_TrackTargetTemplate_16X16_clicked();

    void on_pushButton_54_TrackTargetTemplate_32X32_clicked();

    void on_pushButton_54_TrackTargetTemplate_64X64_clicked();

    void on_pushButton_54_TrackTargetTemplate_128X128_clicked();

    void on_pushButton_54_TrackTargetTemplate_AutoSet_clicked();

    void on_pushButton_54_GimbalCenter_clicked();

    void on_pushButton_54_GimbalLookDownVertical_clicked();

    void on_pushButton_54_GimbalYawLock_clicked();

    void on_pushButton_54_YawFollow_clicked();

    void on_pushButton_54_CollectMode_clicked();

    void on_pushButton_54_GimbalMoterClose_clicked();

    void on_pushButton_54_NumberFollow_clicked();

    void on_pushButton_54_GimbalRunScan_clicked();

    void on_pushButton_54_GimbalScanParameterSet_clicked();

    void on_pushButton_54_GimbalGyroscopeExcursionAutoCalibrate_clicked();

    void on_pushButton_54_GimbalGyroscopeExcursionManualCalibrate_clicked();

    void on_pushButton_54_GimbalGyroscopeExcursionTemperatureCalibrate_clicked();

    void on_pushButton_54_AttitudeGuideFollow_clicked();

    void on_pushButton_54_InfraredShutterCompensate_clicked();

    void on_pushButton_54_SceneSupplement_clicked();

    void on_pushButton_54_InfraredGraphicsStrengthen_clicked();

    void on_pushButton_54_InfraredCutoverRibbonSwitch_clicked();

    void on_pushButton_54_InfraredCutoverRibbonSet_clicked();

    void on_pushButton_54_InfraredFocus_FocusStop_clicked();

    void on_pushButton_54_InfraredFocus_FocusFar_pressed();

    void on_pushButton_54_InfraredFocus_FocusFar_released();

    void on_pushButton_54_InfraredFocus_FocusFar_clicked();

    void on_pushButton_54_InfraredFocus_FocusNear_pressed();

    void on_pushButton_54_InfraredFocus_FocusNear_released();

    void on_pushButton_54_InfraredFocus_FocusNear_clicked();

    void on_pushButton_54_InfraredFocus_FocusAuto_clicked();

    void on_pushButton_54_InfraredMultipleOut_pressed();

    void on_pushButton_54_InfraredMultipleOut_released();

    void on_pushButton_54_InfraredMultipleOut_clicked();

    void on_pushButton_54_InfraredMultipleIn_pressed();

    void on_pushButton_54_InfraredMultipleIn_released();

    void on_pushButton_54_InfraredMultipleIn_clicked();

    void on_pushButton_54_InfraredMultipleSet_clicked();

    void on_pushButton_54_LigthMultipleOut_pressed();

    void on_pushButton_54_LigthMultipleOut_released();

    void on_pushButton_54_LigthMultipleOut_clicked();

    void on_pushButton_54_LigthMultipleIn_pressed();

    void on_pushButton_54_LigthMultipleIn_released();

    void on_pushButton_54_LigthMultipleIn_clicked();

    void on_pushButton_54_LigthFacusOut_pressed();

    void on_pushButton_54_LigthFacusOut_released();

    void on_pushButton_54_LigthFacusOut_clicked();

    void on_pushButton_54_LigthFacusIn_pressed();

    void on_pushButton_54_LigthFacusIn_released();

    void on_pushButton_54_LigthFacusIn_clicked();

    void on_pushButton_54_LigthFacusAuto_clicked();

    void on_pushButton_54_LigthElectChange_clicked();

    void on_pushButton_54_LigthZoomFacusStop_clicked();

    void on_pushButton_54_LigthLowilluminationClose_clicked();

    void on_pushButton_54_LigthLowilluminationOpen_clicked();

    void on_pushButton_54_LigthFogClose_clicked();

    void on_pushButton_54_LigthShoot_clicked();

    void on_pushButton_54_LigthContrastAdjust_clicked();

    void on_pushButton_54_LigthFogLow_clicked();

    void on_pushButton_54_LigthFogMiddle_clicked();

    void on_pushButton_54_LigthFogStrong_clicked();

    void on_pushButton_54_LigthFacusSet_clicked();

    void on_radioButton_JoystickEnable_clicked();

    void on_radioButton_JoystickEnableFine_clicked();

    void on_radioButton_JoystickEnableMouse_clicked();

    void on_radioButton_JoystickDisEnable_clicked();

    void on_radioButton_JoystickEnableGimbal_clicked();

    void on_radioButton_JoystickEnableTracking_clicked();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

    virtual void changeEvent(QEvent *event);

private:
    void initAMapPodState();
    void SendAMapPodControlMsg(AMapPod54Control &aMapPod54Control);
    void SendAMapPodControlMsg(AMapPod54Control &aMapPod54Control, const QString strComponentID, const QString strSensorID);
    void SendAMapPodControlZeroMsg();
    void StopAMapPodGimbalChange(int millisecondsTime = 100);
    void ShowAMapPodStatusInfo(const QString strSensorID, const QByteArray message);
    QString AMapPod54Control_FalseColorBandModel_ToStr(const int falseColorBandModel);
    QString AMapPod54ControlStatus_PayloadType_ToStr(const AMapPod54ControlStatus_PayloadType payloadType);
    QString AMapPod54ControlStatus_AutoCheckResultStatus_ToStr(const uint8_t autoCheckResultStatus);
    QString AMapPod54ControlStatus_ServoStatus_ToStr(const uint8_t servoStatus);
    QString AMapPod54ControlStatus_CurrentDisplayImageFeedback_ToStr(const uint8_t currentDisplayImageFeedback);
    QString AMapPod54ControlStatus_AttitudeIndicationFeedback_ToStr(const uint8_t attitudeIndicationFeedback);

    typedef struct __AMapPod54ControlStatusInfo
    {
        AMapPod54ControlStatus    aMapPod54ControlStatus;
        bool                      selectedCurrentSensor;
        unsigned long long int    targetBatchTime;
        float32_t                 targetLongitude;
        float32_t                 targetLatitude;
        int16_t                   targetAltitude;
        bool                      targetBatchStart;
        bool                      targetBatch;
        bool                      laserStarted;
        bool                      laserDistanceContinueStarted;
        bool                      laserDistanceStarted;
        unsigned int              receivedCmdFeedback;
        unsigned int              receivedLaserDistanceCount;;
        time_t                    laserDistanceTimes;
        unsigned long int         laserDistanceCount;
        unsigned long int         Z;
        unsigned long int         P;
        QTimer                   *laserTimer;
        __AMapPod54ControlStatusInfo()
        {
            memset(&aMapPod54ControlStatus, 0, sizeof(aMapPod54ControlStatus));
            selectedCurrentSensor         = false;
            targetBatchTime               = 0;
            targetLongitude               = 0;
            targetLatitude                = 0;
            targetAltitude                = 0;
            targetBatchStart              = false;
            targetBatch                   = true;
            laserStarted                  = false;
            laserDistanceContinueStarted  = false;
            laserDistanceStarted          = false;
            receivedCmdFeedback = 0;
            receivedLaserDistanceCount = 0;
            laserDistanceTimes            = 0;
            laserDistanceCount            = 0;
            Z                             = 0;
            P                             = 0;
            laserTimer                    = nullptr;
        }
    } AMapPod54ControlStatusInfo;

    typedef struct __Component_Property_Component54
    {
        unsigned long int sysID;
        std::string       component_ComponentSysID;
        std::string       component_ComponentName;
        std::string       component_ComponentID;
        std::string       component_SensorName;
        std::string       component_SensorID;
        __Component_Property_Component54()
        {
            sysID                                = 0;
            component_ComponentSysID = "";
            component_ComponentName  = "";
            component_ComponentID    = "";
            component_SensorName     = "";
            component_SensorID       = "";
        }
    } Component_Property_Component54;

private:
    const static char *QPushButton_Property_ComponentSysID;
    const static char *QPushButton_Property_ComponentName;
    const static char *QPushButton_Property_ComponentID;
    const static char *QPushButton_Property_SensorName;
    const static char *QPushButton_Property_SensorID;

private:
    Ui::IAerialShotSettingBundle54Dialog                             *ui;
    bool                                                             m_WidgetShow;
    float                                                            m_DeflectionAngleofNose;
    float                                                            m_DeflectionAngleofPitch;
    unsigned int                                                     m_LightElectronicZoom;
    unsigned int                                                     m_LaserDistance;
    AMapPod54ControlStatus                                           m_AMapPod54StatusInfo;
    QString                                                          m_CurrentSensorID;
    QString                                                          m_CurrentComponentID;
    QString                                                          m_CurrentComponentName;
    QAbstractButton                                                 *m_pButtonCurrentComponent;
    QButtonGroup                                                    *m_pButtonGroupComponent;
    bool                                                             m_Widget_54_All_Function;
    bool                                                             m_Joy_FineTuning;
    bool                                                             m_JoyGimbalLock;
    bool                                                             m_JoystickEnableGimbal;
    bool                                                             m_Telemeter_Timer_Start;
    QTimer                                                           m_Telemeter_Timer;
    bool                                                             m_AerialShotSettingTelemetrySend54;
    bool                                                             m_AerialShotSetting54AllFunction;
    bool                                                             m_FlyControl54TelemetrySend;
    time_t                                                           m_FlyControl54TelemetrySendTime;
    QTimer                                                           m_FlyControl54TelemetryTimer;
    QTimer                                                           m_AerialShotSettingBundle54Timer;
    std::function<void ()>                                           m_AerialShotSettingBundle54TimerFunction;
    QTimer                                                           m_AerialShotSettingBundle54TimerJoystickControlSimple;
    std::function<void ()>                                           m_AerialShotSettingBundle54TimerJoystickControlSimpleFunction;
    QMap<QString, Component_Property_Component54>                    m_Component_Property_Component54Map;
    QMap<QString, AMapPod54ControlStatusInfo>                        m_AMapPod54ControlStatusInfoMap;
    std::map<std::string, std::pair<unsigned int, unsigned int> >    m_ZoomDistanceMap;
    std::map<std::string, std::vector< std::pair<unsigned int, std::list< std::pair<unsigned int, std::pair<double, double> > > > > >  m_VehicleDeflectionDistYawPitch54Map;

};

#endif
