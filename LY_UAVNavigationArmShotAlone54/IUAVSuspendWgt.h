#ifndef IYAVSUSPENDWGT_H
#define IYAVSUSPENDWGT_H

#include <QMutex>
#include <QWidget>
#include <QButtonGroup>
#include <QPainter>
#include <QTimer>
#include <LyDialog.h>

#include <ctime>

#include "DataManage/ISensorManageService.h"
#include "Protocol/XygsDY54/XygsDY54Head.h"
#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"
#include "Protocol/DY_ZC_Control/DY_ZC_HUD_Control.h"

#define  VIDEOHUDDIALOG_SHOW

class IProj;
namespace Ui
{
class IUAVSuspendWgt;
}

class UAVSuspendBoxLayout;

namespace qnzkna
{
namespace PlayVideoManage
{
class IPlayVideoManage;
}
}

class IUAVSuspendWgt : public QWidget
{
    Q_OBJECT

public:
    explicit IUAVSuspendWgt(const QString &currentComponentID, const QString &currentComponentName, QWidget *parent = nullptr);
    virtual ~IUAVSuspendWgt() override;

    bool initSensorAndBomb(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoFireControlMap);
    bool initSensorAndOptoelectronic(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoOptoelectronicMap);
    void InitWeaponInfoList();
    void on_Message_Recv(const std::string &strWeaponId, const QByteArray &message);
    std::string getCurrentSensorInfo() const;
    std::string getCurrentComponentInfo() const;
    qnzkna::PlayVideoManage::IPlayVideoManage *getPlayVideoManageWidget() const;
	bool canShow();
	void setShowFlag(bool flag);
	void ReflushView();

	void setWeibiaoOffset(int offsetX, int offsetY);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

    virtual bool event(QEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;
    virtual void changeEvent(QEvent *event) override;

private:
	void init();

private:
    Ui::IUAVSuspendWgt                            *ui;
	UAVSuspendBoxLayout*																m_VerticalBoxLayout;
    qnzkna::PlayVideoManage::IPlayVideoManage*							m_IPlayVideoManage;
	qint64																							 m_CurrentTime;
	std::string																						 m_CurrentSensorID;

	QString																						 m_currentComponentID;
	QString																						 m_currentComponentName;

	std::string																						  m_videoDeviceType;
	std::string																						  m_videoDeviceURL;
	std::string																						  m_videoDeviceURLInfrared;
	std::string																						  m_videoDevicePort;
	std::string																						  m_videoDeviceChannel;
	std::string																						  m_videoDeviceUserName;
	std::string																						  m_videoDevicePassWord;
	mutable QMutex																			  m_sMutex;
	bool																								  m_showFalg;
	double																							  m_weibiaoXOffiset;
	double																							  m_weibiaoYOffiset;
};

class UAVSuspendBoxLayout : public QVBoxLayout
{
    Q_OBJECT
public:
    explicit UAVSuspendBoxLayout(IUAVSuspendWgt *pIUAVSuspendWgt, QWidget *parent)
        : QVBoxLayout(parent)
        , m_pIUAVSuspendWgt(pIUAVSuspendWgt)
    {}

    virtual ~UAVSuspendBoxLayout() {}

    virtual void setGeometry(const QRect &) override;

private:
    IUAVSuspendWgt *m_pIUAVSuspendWgt;
};

#endif
