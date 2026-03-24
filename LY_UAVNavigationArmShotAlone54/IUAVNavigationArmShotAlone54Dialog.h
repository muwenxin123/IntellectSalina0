#ifndef IUAVNAVIGATIONARMSHOTALONE54DIALOG_H
#define IUAVNAVIGATIONARMSHOTALONE54DIALOG_H

#include <QMutex>
#include <QWidget>
#include <QButtonGroup>
#include <QPainter>
#include <QTimer>
#include <LyDialog.h>
#include <QVBoxLayout>
#include <ctime>

#include "DataManage/ISensorManageService.h"
#include "Protocol/XygsDY54/XygsDY54Head.h"
#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"
#include "Protocol/DY_ZC_Control/DY_ZC_HUD_Control.h"
#include "Protocol/XygsDY38MM/XygsDY38MMHead.h"
#include "Protocol/XygsDY95_1/XygsDY95_1Head.h"

#include <QUdpSocket>
#include <QHostAddress>


#define  VIDEOHUDDIALOG_SHOW



namespace Ui
{
class IUAVNavigationArmShotAlone54Dialog;
}

class BombStatusWidget;
class ImageStatusWidget;
class UAVNavigationArmShotAlone54BoxLayout;
class IUAVNavigationArmShotAlone54CruiseControl;
class VideoHUDDialog;
class IUAVNavigationArmShotAlone54_ALL_Dialog;


namespace qnzkna
{
namespace PlayVideoManage
{

class IPlayVideoManage;

}
}

namespace qnzkna
{
namespace MessageObjectManage
{

}
}
class IUAVNavigationArmShotAlone54Dialog : public QWidget
{
    Q_OBJECT

public:
    explicit IUAVNavigationArmShotAlone54Dialog(const QString &currentComponentID, const QString &currentComponentName, QWidget *parent = nullptr);
    virtual ~IUAVNavigationArmShotAlone54Dialog() override;


    typedef enum __DY54_BombType_Enum
    {
        DY54_BombType_Enum_None      = 0x00,
        DY54_BombType_Enum_Detect    = 0x01,
        DY54_BombType_Enum_Rocket    = 0x02,
        DY54_BombType_Enum_Cruise    = 0x03,
        DY54_BombType_Enum_38MM      = 0x04,
        DY54_BombType_Enum_95_1      = 0x05,
        DY54_BombType_Enum_95_1_Wolf = 0x06,
        DY54_BombType_Enum_38MM_PWM  = 0x07,
    } DY54_BombType_Enum;


    bool initSensorAndBomb(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoFireControlMap);
    bool initSensorAndOptoelectronic(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoOptoelectronicMap);
    void InitWeaponInfoList();
    void on_Message_Recv(const std::string &strWeaponId, const QByteArray &message);
    void on_DY_ZC_HUD_ControlInfo(const std::string &strWeaponId, const QByteArray &message);

    static void bomBCallBackFunction(BombStatusWidget *bombStatusWidget, IUAVNavigationArmShotAlone54Dialog *dialog);

    QString getTitleInfo() const;
    std::string getCurrentSensorInfo() const;
    std::string getCurrentComponentInfo() const;
    const QString& getCurrentComponentID() const;
	const QString& getCurrentComponentName() const;

    std::string  getSensorType() const;
    DY54_BombType_Enum getBombType() const;

    qnzkna::PlayVideoManage::IPlayVideoManage *getPlayVideoManageWidget() const;

    void Recv_netheader_uctype_dy38mm_control(char *lpData, int nLength);
    void Deal_netheader_uctype_dy38mm_control(const char * const lpData, int nLength);

    void Update_XygsDY95_1_SBUS_CommandStatusSyn(const qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_CommandStatusSync* const xygsDY95_1_SBUS_CommandStatusSync);

private:

    bool InitBombState(bool resetBomb = false);

    void ResetCommandStatus(const XygsDY54KZ::XygsDY54KZMsg_Pill pill);

    void SendXygsDY54KZMsg(XygsDY54KZ::XygsDY54KZMsg &xygsDY54KZMsg, const std::string &currentSensorInfoID);
    void SendXygsDY54KZMsg(const unsigned char *const message, const unsigned int msglength, const std::string &currentSensorInfoID);

    void StopXygsDY54KZChange(int millisecondsTime = 100);

    void InitTargetShotControl();

    uint8_t GetCurrentHitEquip();

    std::vector<std::tuple<std::string, std::string>>    m_TargetList;


signals:
    void ShowXygsDY54KZMsgStatusInfoSig(const QString strQSensorId, const QByteArray message);
    void ShowDY_ZC_HUD_ControlInfoSig(const QString strQWeaponId, const QByteArray message);

private slots:
    void on_Button_DY54_GroupComponent_buttonClicked(QAbstractButton *button);

    void IUAVNavigationArmShotAlone_DY54_LEFT_Video_Light();
    void IUAVNavigationArmShotAlone_DY54_LEFT_Video_Infrared();

    void IUAVNavigationArmShotAlone_DY54_Pill_CheckBox_All_Shot(int state);
    void IUAVNavigationArmShotAlone_DY54_Pill_Command_All_Shot();

    void IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Insure();
    void IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Percussion();
    void IUAVNavigationArmShotAlone54Dialog_DY54_Pill_1_Command_Reset();

    void IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Insure();
    void IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Percussion();
    void IUAVNavigationArmShotAlone54Dialog_DY54_Pill_2_Command_Reset();

    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_UnLock();
    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Command_Lock();
    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_Command_Reset();
    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_1_Command_Shot();
    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_2_Command_Shot();
    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_3_Command_Shot();
    void IUAVNavigationArmShotAlone54Dialog_DY54_BombType_Cruise_Pill_4_Command_Shot();

    void IUAVNavigationArmShotAlone_DY54_StartAutoAiming();
    void IUAVNavigationArmShotAlone_DY54_StopAutoAiming();

    void ShowXygsDY54KZMsgStatusInfo(const QString strQSensorId, const QByteArray message);
    void ShowDY_ZC_HUD_ControlInfo(const QString strQWeaponId, const QByteArray message);

    void on_pushButton_Target_ScreenShot_clicked();
    void on_pushButton_Target_Record_clicked();

    void on_radioButton_Target_HaveFromTable_clicked();

    void on_radioButton_Target_New_clicked();

    //// 38mm
    void on_pushButton_38mmlauncher_Control_Video_Light();

    void on_pushButton_38mmlauncher_Control_Video_Infrared();

    void on_pushButton_38mmlauncher_Control_Video_Close();

    void on_pushButton_38mmlauncher_Control_fuyang_up();

    void on_pushButton_38mmlauncher_Control_puyang_down();

    void on_pushButton_38mmlauncher_Control_puyang_stop();

    void on_pushButton_38mmlauncher_Control_lock();

    void on_pushButton_38mmlauncher_Control_unlock();

    void on_pushButton_38mmlauncher_Control_Percussion();

    int sendMessage_38mm_Launcher_Control(const char * const pbuf, const int pbufSize);

    //// 95-1
    void on_pushButton_95_1launcher_Control_Video_Light();

    void on_pushButton_95_1launcher_Control_Video_Light_Infrared();

    void on_pushButton_95_1launcher_Control_Video_Infrared();

    void on_pushButton_95_1launcher_Control_Video_Close();

    void on_pushButton_95_1launcher_Control_jiguang_open();

    void on_pushButton_95_1launcher_Control_jiguang_close();

    void on_pushButton_95_1launcher_Control_focus_far();

    void on_pushButton_95_1launcher_Control_focus_near();

    void on_pushButton_95_1launcher_Control_focus_stop();

    void on_pushButton_95_1launcher_Control_fuyang_up();

    void on_pushButton_95_1launcher_Control_puyang_down();

    void on_verticalSlider_95_1launcher_Control_fuyang(int position);

    void on_pushButton_95_1launcher_Control_lock();

    void on_pushButton_95_1launcher_Control_unlock();

    void on_pushButton_95_1launcher_Control_Percussion();


    int sendMessage__95_1_StatusSync_control(const char * const pbuf, const int pbufSize);
    int sendMessage__95_1Launcher_Control(const char * const pbuf, const int pbufSize);
    int sendMessage__95_1_Wolf_Launcher_Control(const char * const pbuf, const int pbufSize);




    //// 38mm  95-1 加锁 解锁 击发
    void Control__95_1__38MM__lock();

    void Control__95_1__38MM__unlock();

    void Control__95_1__38MM__Percussion();




protected:
    virtual void paintEvent(QPaintEvent *event) override;

    virtual bool event(QEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

    virtual void changeEvent(QEvent *event) override;

    void moveVideoHUDDialog();
    void resizeVideoHUDDialog();

    bool getCurrentTarget(QString &strId);
    QString createTargetID();

private:
    const static char *QPushButton_Property_ComponentName;
    const static char *QPushButton_Property_ComponentID;

private:
    Ui::IUAVNavigationArmShotAlone54Dialog                            *ui;
    QString                                                            m_StrTitle;
    UAVNavigationArmShotAlone54BoxLayout                              *m_VerticalBoxLayout;
    qnzkna::PlayVideoManage::IPlayVideoManage                         *m_IPlayVideoManage;
    qint64                                                             m_CurrentTime;
    QTimer                                                             m_Timer;
    bool                                                               m_Pill_CheckBox_All_Shot_Checked;

    bool                                                               m_singleSelectedBomb;

    std::string                                                        m_CurrentSensorID;

    std::list<ImageStatusWidget *>                                     m_DY54_ImageStatusList_Pill_1;
    std::list<ImageStatusWidget *>                                     m_DY54_ImageStatusList_Pill_2;

    time_t                                                             m_TimeOutBomb;
    time_t                                                             m_TimeOutBomb_38MM;
    time_t                                                             m_TimeOut_DY_ZC_HUD_ControlInfo;

    QString                                                            m_currentComponentID;
    QString                                                            m_currentComponentName;

    QAbstractButton                                                   *m_pButtonCurrentComponent;
    QButtonGroup                                                      *m_pButtonGroupComponent;

    std::string                                                        m_videoDeviceType;
    std::string                                                        m_videoDeviceURL;
    std::string                                                        m_videoDeviceURLInfrared;
    std::string                                                        m_videoDevicePort;
    std::string                                                        m_videoDeviceChannel;
    std::string                                                        m_videoDeviceUserName;
    std::string                                                        m_videoDevicePassWord;
    std::string                                                        m_VideoType_54;

    QString                                                            m_videoControlSensorInfoID;


    std::string                                                        m_SensorType;
    DY54_BombType_Enum                                                 m_DY54_BombType;

    IUAVNavigationArmShotAlone54CruiseControl                         *m_IUAVNavigationArmShotAlone54CruiseControl;

    qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command m_XygsDY95_1_SBUS_Command;
    bool                                                               m_UseUdpSocket;
    QHostAddress                                                       m_UdpHostAddressRemote;
    int                                                                m_UdpHostAddressLocalPort;
    int                                                                m_UdpHostAddressRemotePort;
    QUdpSocket*                                                        m_udpSocket;

    bool                                                               m_UseUdpSocket_95_1_StatusSync;

    //变焦-停止变焦
    bool                                                               bIsFocusFar = false;
    bool                                                               bIsFocusNear = false;
#ifdef VIDEOHUDDIALOG_SHOW
    VideoHUDDialog                                                    *m_VideoHUDDialog;
#endif

    mutable QMutex                                                     m_sMutex;

    friend class IUAVNavigationArmShotAlone54_ALL_Dialog;
};

class UAVNavigationArmShotAlone54BoxLayout : public QVBoxLayout
{
    Q_OBJECT
public:
    explicit UAVNavigationArmShotAlone54BoxLayout(IUAVNavigationArmShotAlone54Dialog *pIUAVNavigationArmShotAlone54Dialog, QWidget *parent)
        : QVBoxLayout(parent)
        , m_pIUAVNavigationArmShotAlone54Dialog(pIUAVNavigationArmShotAlone54Dialog)
    {}

    virtual ~UAVNavigationArmShotAlone54BoxLayout() {}

    virtual void setGeometry(const QRect &) override;

private:
    IUAVNavigationArmShotAlone54Dialog *m_pIUAVNavigationArmShotAlone54Dialog;
};

#endif
