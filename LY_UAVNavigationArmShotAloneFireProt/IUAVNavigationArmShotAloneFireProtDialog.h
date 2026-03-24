#ifndef IUAVNAVIGATIONARMSHOTALONEFIREPROTDIALOG_H
#define IUAVNAVIGATIONARMSHOTALONEFIREPROTDIALOG_H

#include <QWidget>
#include <QButtonGroup>
#include <QPainter>
#include <QTimer>
#include <LyDialog.h>

#include <ctime>

#include "DataManage/ISensorManageService.h"
#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"
#include "Protocol/DY_ZC_Control/DY_ZC_HUD_Control.h"
#include "QMutex"

class BombStatusWidget;
class ImageStatusWidget;
class UAVNavigationArmShotAloneFireProtBoxLayout;
class VideoHUDDialog;

namespace qnzkna { namespace PlayVideoManage
{

class IPlayVideoManage;

}}

namespace Ui
{
class IUAVNavigationArmShotAloneFireProtDialog;
}

class IUAVNavigationArmShotAloneFireProtDialog : public QWidget
{
    Q_OBJECT

public:
    explicit IUAVNavigationArmShotAloneFireProtDialog(const QString& currentComponentID, const QString& currentComponentName, QWidget* parent = nullptr);
    ~IUAVNavigationArmShotAloneFireProtDialog();
    void StopIUAVNavigationArmShotAloneFireProtVideoManage();

    bool initSensorAndBomb(qnzkna::SensorManage::SensorInfoMap& tmpSensorInfoFireControlMap);
    bool initSensorAndOptoelectronic(qnzkna::SensorManage::SensorInfoMap& tmpSensorInfoOptoelectronicMap);
    void InitWeaponInfoList();
    const QString GetWindowTitle();
    void on_Message_Recv(const std::string &strWeaponId, const QByteArray& message);
    void OnCommand(int nID, void *lParam);

    static void bomBCallBackFunction(BombStatusWidget *bombStatusWidget, IUAVNavigationArmShotAloneFireProtDialog* dialog);

    std::string getCurrentSensorInfo() const;
    std::string getCurrentComponentInfo() const;

    qnzkna::PlayVideoManage::IPlayVideoManage *getPlayVideoManageWidget() const;

signals:
    void ShowXygsDYFireProtKZMsgStatusInfoSig(const QString strQSensorId, const QByteArray message);
    void ShowDY_ZC_HUD_ControlInfoSig(const QString strQWeaponId, const QByteArray message);

private slots:
    void on_Button_DYFireProt_GroupComponent_buttonClicked(QAbstractButton *button);

    void IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_1_Command_Insure();
    void IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_1_Command_Percussion();
    void IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_1_Command_Reset();

    void IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_2_Command_Insure();
    void IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_2_Command_Percussion();
    void IUAVNavigationArmShotAloneFireProtDialog_DYFireProt_Pill_2_Command_Reset();

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

private:
    typedef enum __DYFireProt_BombType_Enum
    {
        DYFireProt_BombType_Enum_None       = 0x00,
        DYFireProt_BombType_Enum_Detect     = 0x01,
        DYFireProt_BombType_Enum_Rocket     = 0x02,
        DYFireProt_BombType_Enum_Cruise     = 0x03,
    }DYFireProt_BombType_Enum;

    bool InitBombState(bool resetBomb = false);
    void ResetCommandStatus();

    void SendXygsDYFireProtKZMsg(const QString strQWeaponId, const std::string &currentSensorInfoID);
    void SendXygsDYFireProtKZMsg(const unsigned char* const message, const unsigned int msglength, const std::string& currentSensorInfoID);

    void ShowXygsDYFireProtKZMsgStatusInfo(const QString strQSensorId, const QByteArray message);
    void ShowDY_ZC_HUD_ControlInfo(const QString strQWeaponId, const QByteArray message);

private:
    const static char* QPushButton_Property_ComponentName;
    const static char* QPushButton_Property_ComponentID;

private:
    Ui::IUAVNavigationArmShotAloneFireProtDialog                      *ui;
    QString                                                            m_WindowTitle;
    UAVNavigationArmShotAloneFireProtBoxLayout                        *m_VerticalBoxLayout;

    qnzkna::PlayVideoManage::IPlayVideoManage                         *m_IPlayVideoManage;
    qint64                                                             m_CurrentTime;
    QTimer                                                             m_Timer;
    bool                                                               m_Pill_CheckBox_All_Shot_Checked;

    bool                                                               m_singleSelectedBomb;

    std::string                                                        m_CurrentSensorID;

    std::list<ImageStatusWidget*>                                      m_DYFireProt_ImageStatusList_Pill_1;
    std::list<ImageStatusWidget*>                                      m_DYFireProt_ImageStatusList_Pill_2;

    time_t                                                             m_TimeOutBomb;
    time_t                                                             m_TimeOut_DY_ZC_HUD_ControlInfo;

    QString                                                            m_currentComponentID;
    QString                                                            m_currentComponentName;

    QAbstractButton*                                                   m_pButtonCurrentComponent;
    QButtonGroup*                                                      m_pButtonGroupComponent;

    std::string                                                        m_videoDeviceType;
    std::string                                                        m_videoDeviceURL;
    std::string                                                        m_videoDeviceURLInfrared;
    std::string                                                        m_videoDevicePort;
    std::string                                                        m_videoDeviceChannel;
    std::string                                                        m_videoDeviceUserName;
    std::string                                                        m_videoDevicePassWord;

    DYFireProt_BombType_Enum                                           m_DYFireProt_BombType;

#ifdef VIDEOHUDDIALOG_SHOW
    VideoHUDDialog*                                                    m_VideoHUDDialog;
#endif

    mutable QMutex                                                     m_sMutex;
};

class UAVNavigationArmShotAloneFireProtBoxLayout : public QVBoxLayout
{
    Q_OBJECT
public:
    explicit UAVNavigationArmShotAloneFireProtBoxLayout(IUAVNavigationArmShotAloneFireProtDialog* pIUAVNavigationArmShotAloneFireProtDialog, QWidget *parent)
        : QVBoxLayout(parent)
          , m_pIUAVNavigationArmShotAloneFireProtDialog(pIUAVNavigationArmShotAloneFireProtDialog)
    {}

    virtual ~UAVNavigationArmShotAloneFireProtBoxLayout() {}

    virtual void setGeometry(const QRect&) override;

private:
    IUAVNavigationArmShotAloneFireProtDialog* m_pIUAVNavigationArmShotAloneFireProtDialog;
};

#endif
