#include "IUAVSuspendWgt.h"
#include "ui_IUAVSuspendWgt.h"
#include "IUAVNavigationArmShotAlone54Activator.h"
#include "PlayVideoManage/IPlayVideoManage.h"
#include "ImageStatusWidget.h"
#include "LY_UAVNavigationArmShotAlone54.h"
#include <QBuffer>
#include <QScreen>
#include <QSettings>
#include "IProj.h"

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

#include <QCString.h>
#include "QtSql/QSqlQuery"

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

#define OFFSET_RATE		5

IUAVSuspendWgt::IUAVSuspendWgt(const QString &currentComponentID, const QString &currentComponentName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::IUAVSuspendWgt)
    , m_VerticalBoxLayout(nullptr)
    , m_IPlayVideoManage(nullptr)
    , m_CurrentTime(0)
    , m_CurrentSensorID("")
    , m_currentComponentID(currentComponentID)
    , m_currentComponentName(currentComponentName)
    , m_videoDeviceType("")
    , m_videoDeviceURL("")
    , m_videoDeviceURLInfrared("")
    , m_videoDevicePort("")
    , m_videoDeviceChannel("")
    , m_videoDeviceUserName("")
    , m_videoDevicePassWord("")
{
    ui->setupUi(this);
	//setStyleSheet("background: rgb(255, 0, 0)");
	setWindowFlags(Qt::FramelessWindowHint);
	{
		m_VerticalBoxLayout = new UAVSuspendBoxLayout(this, this);
		m_VerticalBoxLayout->setSpacing(2);
		m_VerticalBoxLayout->setObjectName(QString::fromUtf8("UAVNavigationArmShotAlone54BoxLayout"));
		m_VerticalBoxLayout->setContentsMargins(2, 2, 2, 2);

		m_VerticalBoxLayout->addWidget(ui->widget_DY54_Top_Video);
	}
	init();
	this->resize(160, 100);
}

IUAVSuspendWgt::~IUAVSuspendWgt()
{
    if (ui != nullptr && ui->widget_DY54_Top_Video != nullptr && m_IPlayVideoManage != nullptr)
    {
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
        m_IPlayVideoManage->StopVideoManageThread();
#endif
    }

    delete ui;
}

bool IUAVSuspendWgt::initSensorAndBomb(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoFireControlMap)
{
	bool result = false;
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
		if ("FireControl54" != itrMapFirst->second.GetSensorType() || DetectType == "1")
		{
			return false;
		}
		const std::string strSensorIDFirst(itrMapFirst->second.GetSensorID());
		const std::string strSensorNameFirst(itrMapFirst->second.GetSensorName());
		const std::string strSensorSimpleNameFirst(itrMapFirst->second.GetSensorSimpleName());
		const std::string strSensorTypeFirst(itrMapFirst->second.GetSensorType());

		itrMapFirst->second.GetSensorParam(std::string("Start"), StartType);
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

		std::transform(bombTypeDY54First.begin(), bombTypeDY54First.end(), bombTypeDY54First.begin(), ::tolower);
		std::transform(videoDeviceTypeFirst.begin(), videoDeviceTypeFirst.end(), videoDeviceTypeFirst.begin(), ::tolower);
		m_videoDeviceType = videoDeviceTypeFirst;
		m_videoDeviceURL = videoDeviceURLFirst;
		m_videoDeviceURLInfrared = videoDeviceURLFirstInfrared;
		m_videoDevicePort = videoDevicePortFirst;
		m_videoDeviceChannel = videoDeviceChannelFirst;
		m_videoDeviceUserName = videoDeviceUserNameFirst;
		m_videoDevicePassWord = videoDevicePassWordFirst;

		m_CurrentSensorID = strSensorIDFirst;
		result = true;
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

		QVBoxLayout *verticalLayout = new QVBoxLayout(ui->widget_DY54_Top_Video);
		if (verticalLayout && m_IPlayVideoManage != nullptr)
		{
			verticalLayout->addWidget(m_IPlayVideoManage);
			ui->widget_DY54_Top_Video->setLayout(verticalLayout);
			const qnzkna::PlayVideoManage::CVideoDevice videoDevice(
				m_currentComponentID.toStdString(),
				QString::fromStdString(m_videoDeviceType).toLower().toStdString(), 
				m_videoDeviceURL, m_videoDeviceURLInfrared, 
				m_videoDevicePort, m_videoDeviceChannel, 
				m_videoDeviceUserName, m_videoDevicePassWord, "");
			m_IPlayVideoManage->StartVideoManageObjectHost(videoDevice);

			if (StartType == "Start")
			{
				m_IPlayVideoManage->hide();
			}
			else if (QString::fromStdString(m_videoDeviceType).compare("rtsp54", Qt::CaseInsensitive) == 0)
			{
				m_IPlayVideoManage->hide();
			}


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
					const qnzkna::PlayVideoManage::CVideoDevice videoDevice(m_currentComponentID.toStdString(), QString::fromStdString(m_videoDeviceType).toLower().toStdString(), m_videoDeviceURL, m_videoDeviceURLInfrared, m_videoDevicePort, m_videoDeviceChannel, m_videoDeviceUserName, m_videoDevicePassWord, "");

					m_IPlayVideoManage->StartVideoManageThread(videoDevice);

					logInfo() << __FILE__ << currect << __LINE__ << " " << m_currentComponentID.toStdString().c_str() << " " << m_videoDeviceURL.c_str() << " " << m_videoDeviceURLInfrared.c_str();
#endif
				}
			}
		}
	}
#endif
	return result;
}

bool IUAVSuspendWgt::initSensorAndOptoelectronic(qnzkna::SensorManage::SensorInfoMap &tmpSensorInfoOptoelectronicMap)
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

void IUAVSuspendWgt::InitWeaponInfoList()
{

}

//void IUAVSuspendWgt::IUAVNavigationArmShotAlone_DY54_LEFT_Video_Light()
//{
//    if (ui != nullptr && ui->widget_DY54_Top_Video != nullptr && m_IPlayVideoManage != nullptr)
//    {
//        ui->widget_DY54_Top_Video->show();
//#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
//        m_IPlayVideoManage->show();
//        m_IPlayVideoManage->setVideoType(2);
//#endif
//    }
//}

void IUAVSuspendWgt::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

bool IUAVSuspendWgt::event(QEvent *event)
{
    //if (event->type() == QEvent::Type::ScreenChangeInternal)
  //  {
   // }
    return QWidget::event(event);
}

void IUAVSuspendWgt::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
}

void IUAVSuspendWgt::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void IUAVSuspendWgt::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
}

void IUAVSuspendWgt::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
}

void IUAVSuspendWgt::changeEvent(QEvent *event)
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

void IUAVSuspendWgt::init()
{
	m_weibiaoXOffiset = 0;
	m_weibiaoYOffiset = 0;

	QSettings settingsWeiBiaoViewShow("../data/conf/WeiBiaoViewShow.ini", QSettings::IniFormat);
	settingsWeiBiaoViewShow.setIniCodec("UTF-8");
	settingsWeiBiaoViewShow.beginGroup("WeiBiaoViewShowSettings");

	int index = -1;
	if (settingsWeiBiaoViewShow.contains("WeiBiaoViewShow"))
	{
		const QVariant weiBiaoViewShowTemp = settingsWeiBiaoViewShow.value("WeiBiaoViewShow", false);
		if (weiBiaoViewShowTemp.canConvert(QMetaType::QStringList))
		{
			const QStringList weiBiaoViewShowStringList = weiBiaoViewShowTemp.toStringList();
			if (weiBiaoViewShowStringList.contains(m_currentComponentID))
			{
				index = weiBiaoViewShowStringList.indexOf(m_currentComponentID);
			}
		}
	}

	if (index >= 0 && settingsWeiBiaoViewShow.contains("WeiBiaoViewOffSetPosition"))
	{
		const QVariant weiBiaoViewOffSetPositionTemp = settingsWeiBiaoViewShow.value("WeiBiaoViewOffSetPosition", false);
		if (weiBiaoViewOffSetPositionTemp.canConvert(QMetaType::QStringList))
		{
			const QStringList weiBiaoViewOffSetPositionStringList = weiBiaoViewOffSetPositionTemp.toStringList();
			for (auto& str : weiBiaoViewOffSetPositionStringList)
			{
				QStringList offsetList = str.split(":", QString::SkipEmptyParts);
				if (offsetList.size() == 3 && offsetList.at(0) == m_currentComponentID)
				{
					const int weiBiaoViewOffSetPositionX = offsetList.at(1).toInt();
					const int weiBiaoViewOffSetPositionY = offsetList.at(2).toInt();
					if (weiBiaoViewOffSetPositionX <= 127 && weiBiaoViewOffSetPositionX >= -128 &&
						weiBiaoViewOffSetPositionY <= 127 && weiBiaoViewOffSetPositionY >= -128)
					{
						m_weibiaoXOffiset = weiBiaoViewOffSetPositionX;
						m_weibiaoYOffiset = weiBiaoViewOffSetPositionY;
					}
					break;
				}
			}
		}
	}
}

qnzkna::PlayVideoManage::IPlayVideoManage *IUAVSuspendWgt::getPlayVideoManageWidget() const
{
    if (ui && ui->widget_DY54_Top_Video && m_IPlayVideoManage)
    {
        return m_IPlayVideoManage;
    }
    return nullptr;
}

bool IUAVSuspendWgt::canShow()
{
	return m_showFalg;
}

void IUAVSuspendWgt::setShowFlag(bool flag)
{
	m_showFalg = flag;
	if (flag)
	{
		show();
	}
	else
	{
		hide();
	}
}

void IUAVSuspendWgt::ReflushView()
{
	if (m_showFalg && isVisible())
	{
		auto pIUAVRealTimeStateManageService = IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService();
		if (nullptr != pIUAVRealTimeStateManageService)
		{
			qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t  teleInfomation;
			if (pIUAVRealTimeStateManageService->GetUAVRealTimeState(m_currentComponentID.toStdString(), teleInfomation))
			{
				auto pCoordinate = LY_UAVNavigationArmShotAlone54::Get().GetCoordinateTransformation();
				if (nullptr != pCoordinate)
				{
					QPoint pt = pCoordinate->Jwd2Screen(teleInfomation.telemetryInfo.position.lng, teleInfomation.telemetryInfo.position.lat);
					pt.rx() += m_weibiaoXOffiset * OFFSET_RATE;
					pt.ry() += m_weibiaoYOffiset * OFFSET_RATE;
					pt.rx() -= 5;
					pt.ry() -= 105;
					move(pt);
				}
			}
		}
	}
}

void IUAVSuspendWgt::setWeibiaoOffset(int offsetX, int offsetY)
{
	m_weibiaoXOffiset = offsetX;
	m_weibiaoYOffiset = offsetY;
}

std::string IUAVSuspendWgt::getCurrentSensorInfo() const
{
    return m_CurrentSensorID;
}

std::string IUAVSuspendWgt::getCurrentComponentInfo() const
{
    return m_currentComponentID.toStdString();
}

void IUAVSuspendWgt::on_Message_Recv(const std::string &strSensorId, const QByteArray &message)
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
}

void UAVSuspendBoxLayout::setGeometry(const QRect &rect)
{
#ifdef PLAY_VIDEO_STREAM_MANAGE_DEFINES
    if (m_pIUAVSuspendWgt && m_pIUAVSuspendWgt->getPlayVideoManageWidget())
    {
        m_pIUAVSuspendWgt->getPlayVideoManageWidget()->Lock();
        QVBoxLayout::setGeometry(rect);
        m_pIUAVSuspendWgt->getPlayVideoManageWidget()->UnLock();
    }
    else
    {
        QVBoxLayout::setGeometry(rect);
    }
#else
    QVBoxLayout::setGeometry(rect);
#endif
}
