#ifndef UAVNAVIGATIONARMSHOTAUTOAIMING_H
#define UAVNAVIGATIONARMSHOTAUTOAIMING_H

#include <LyMessageBox.h>
#include <QObject>
#include <QTimer>

#include "IProj.h"

#include "DataManage/Common/TypeDefine_Node.h"
#include "Protocol/XygsJL/Bag_XygsJLDefines.h"
#include "DataManage/XYWeaponManage/XYWeaponManage.h"
#include "DataManage/WeaponSystem/WeaponSystem.h"

#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"

class UAVNavigationArmShotAutoAiming;
class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QVBoxLayout;

class LyMessageBoxCruise : public LyDialog
{
    Q_OBJECT

public:
    LyMessageBoxCruise(const std::string &strComponentID, const std::string &strComponentName, UAVNavigationArmShotAutoAiming *pUAVNavigationArmShotAutoAiming, QWidget *parent = nullptr);

    void setUAVAndMarkPosition(const double longitudeUAV, const double latitudeUAV, const double altitudeUAV, const double altitudeRelUAV, const double longitudeMark, const double latitudeMark, const double altitudeMark);

private:
    void calAimingPositionFromUAVAndMarkPosition(bool userChange = false);

private:
    QWidget                         *widget_FlyGoto = nullptr;
    QVBoxLayout                     *verticalLayout = nullptr;
    QGroupBox                       *groupBox_FlyGoto = nullptr;
    QGridLayout                     *gridLayout_4 = nullptr;
    QLabel                          *label_FlyGoto_Longitude = nullptr;
    QDoubleSpinBox                  *doubleSpinBox_FlyGoto_Longitude = nullptr;
    QLabel                          *label_FlyGoto_Latitude = nullptr;
    QDoubleSpinBox                  *doubleSpinBox_FlyGoto_Latitude = nullptr;
    QLabel                          *label_FlyGoto_Altitude = nullptr;
    QDoubleSpinBox                  *doubleSpinBox_FlyGoto_Altitude = nullptr;
    QGridLayout                     *gridLayout_3 = nullptr;
    QLabel                          *label_FlyGoto_Velocity = nullptr;
    QSpinBox                        *spinBox_FlyGoto_Velocity = nullptr;
    QLabel                          *label_FlyGoto_Radius = nullptr;
    QSpinBox                        *spinBox_FlyGoto_Radius = nullptr;
    QLabel                          *label_FlyGoto_Time = nullptr;
    QSpinBox                        *spinBox_FlyGoto_Time = nullptr;
    QLabel                          *label_FlyGoto_Mode = nullptr;
    QComboBox                       *comboBox_FlyGoto_Mode = nullptr;
    QPushButton                     *pushButton_FlyGoto_Cancel = nullptr;
    QPushButton                     *pushButton_FlyGoto_OK = nullptr;
    QLabel                          *label_MarkPosition = nullptr;

private:
    UAVNavigationArmShotAutoAiming  *m_pUAVNavigationArmShotAutoAiming;
    const std::string                m_strComponentID;
    const std::string                m_strComponentName;

    double                           m_LongitudeUAV;
    double                           m_LatitudeUAV;
    double                           m_AltitudeUAV;
    double                           m_AltitudeRelUAV;

    double                           m_LongitudeMark;
    double                           m_LatitudeMark;
    double                           m_AltitudeMark;

    double                           m_LongitudeFlyGoto;
    double                           m_LatitudeFlyGoto;
    double                           m_AltitudeFlyGoto;
    double                           m_AltitudeFlyGoto_Mark_Difference;
    int                              m_VelocityFlyGoto;
    int                              m_RadiusFlyGoto;
    int                              m_TimeFlyGoto;
    int                              m_ModeFlyGoto;
};

class UAVNavigationArmShotAutoAiming : public QObject
{
    Q_OBJECT

public:
    explicit UAVNavigationArmShotAutoAiming(QObject *parent = nullptr);

    void InitWeaponComponent();

    void setPProj(IProj *pProj);

signals:
    void AutoAmimingSig(const QString strComponentID, const QByteArray data);

private slots:
    void StartAutoAmiming();
    void StopAutoAmiming() const;

private:
    void AutoAiming(const std::string &strComponentID) const;
    double HeadAngle(const double angle) const;

    void FlyMode_StabilizeMode(const std::string &strComponentID) const;

    void CommOper_HoverStart(const std::string &strComponentID) const;
    void CommOper_HoverStop(const std::string &strComponentID) const;

    void FlyAction_GoForward(const double goForwardDist, const std::string &strComponentID) const;
    void FlyAction_TurnLeft(const double turnLeftAngle, const std::string &strComponentID) const;
    void FlyAction_TurnLeftStationStick(const double turnLeftAngle, const std::string &strComponentID) const;
    void FlyAction_TurnRight(const double turnRightAngle, const std::string &strComponentID) const;
    void FlyAction_TurnRightStationStick(const double turnRightAngle, const std::string &strComponentID) const;
    void FlyAction_GoBack(const double backDist, const std::string &strComponentID) const;
    void FlyAction_GoUp(const double upDist, const std::string &strComponentID) const;
    void FlyAction_ShiftLeft(const double shiftLeftDist, const std::string &strComponentID) const;
    void FlyAction_ShiftRight(const double shiftRightDist, const std::string &strComponentID) const;
    void FlyAction_GoDown(const double downDist, const std::string &strComponentID) const;

    void FlyGoto_FlyGotoPoint(const double longitudeFlyGoto, const double latitudeFlyGoto, const double altitudeFlyGoto, const int velocityFlyGoto, const int radiusFlyGoto, const int timeFlyGoto, const int modeFlyGoto, const std::string &strComponentID);
    void SendMessageBuffer(const char *const message, const unsigned int msglength, qnzkna::NodeManage::NODE_TYPE_UINT nNodeType, const std::string &strName, const std::string &strNodeId, const std::string &strChId, const std::string &strComponentID, const std::string &strProtocolType);

    void SendFlyCmd(unsigned char nCmd, unsigned char nExtParam, const std::string &strComponentID) const;
    void SendFlyCmdStationStick(const xygsJL_15 &bag, const std::string &strComponentID) const;
    void SendFlyCmdMsg(const xygsJL_17 &bag, qnzkna::NodeManage::NODE_TYPE_UINT nNodeType, const std::string &strName, const std::string &strNodeId, const std::string &strChId, const std::string &strComponentID, const std::string &strProtocolType) const;
    void SendFlyCmdStationStickMsg(const xygsJL_15 &bag, qnzkna::NodeManage::NODE_TYPE_UINT nNodeType, const std::string &strName, const std::string &strNodeId, const std::string &strChId, const std::string &strComponentID, const std::string &strProtocolType) const;

private:
    const CXYWeaponManage                         m_CXYWeaponManage;
    double                                        m_DistMarkCalculate;
    double                                        m_YawAngleDifferenceBeforeHeight;
    double                                        m_YawAngleDifference;
    double                                        m_HeightDifference;
    double                                        m_DistDifference;
    double                                        m_DistAutoAimingReday;
    double                                        m_DistVehicleMark;
    double                                        m_HeightVehicleMark;
    double                                        m_LowVehicleMark;
    bool                                          m_GetVehiclePitchStart;
    qnzkna::WeaponManage::MultiWeaponComponentMap m_MultiWeaponComponentMap;
    IProj                                         *m_pProj;
    mutable double                                m_VehiclePitchStart;
    mutable unsigned long long int                m_VehiclePitchStartNumber;
    mutable QVector<double>                       m_VehiclePitchStartList;
    unsigned long long int                        m_AutoAimingAdjustTimesMax;
    mutable unsigned long long int                m_AutoAimingAdjustYawTimes;
    mutable unsigned long long int                m_AutoAimingAdjustHeightTimes;
    mutable unsigned long long int                m_AutoAimingAdjustDistTimes;
    unsigned long long int                        m_AutoAimingNoAdjustTimesMax;
    mutable unsigned long long int                m_AutoAimingNoAdjustTimes;
    int                                           m_AutoAimingTimerDuration;
    mutable QTimer                                m_AutoAimingTimer;
    std::string                                   m_StrComponentID;
    qnzkna::DY_ZC_Control::DY_ZC_ControlInfo      m_DY_ZC_ControlInfoData;

    friend class LyMessageBoxCruise;
};

#endif
