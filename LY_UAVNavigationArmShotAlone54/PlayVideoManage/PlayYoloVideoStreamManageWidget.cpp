#include "PlayYoloVideoStreamManageWidget.h"
#include "ui_PlayYoloVideoStreamManageWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QSettings>
#include <QWindow>
#include <QTextCodec>
#include <LyMessageBox.h>
#include <cmath>

#include "DataManage/ISensorManageService.h"
#include "LY_UAVNavigationArmShotAlone54.h"
#include "IUAVTargetTree.h"
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"

#ifdef WIN32
#include <windows.h>
#endif
#include "LY_UAVNavigationArmShotAlone54Interface.h"
#include "IFunctionInterface.h"

#define PI 3.14159265358979323

unsigned char  xorStr(unsigned char *buf, int start, int end)
{
    int sum = 0;
    unsigned char re;

    int i = start - 1;
    for (; i < end; i++)
    {
        sum = sum ^ buf[i];
    }
    re = (unsigned char)(sum & 0x000000FF);
    return re;
}

#if __DEBUG__LOGINFO__
    #define logInfo(format, ...)
#else

    #include <QDebug>

    #define logInfo(format, ...)
#endif
#include "IUAVNavigationArmShotAlone54Activator.h"
#include "../LY_AerialShotSettingBundle54/IAerialShotSettingBundle54Activator.h"
#include "QtPositioning/QGeoCoordinate"

extern const QString &GetVideoStreamRemoteObjectPrefix();
extern QWidget *GetVideoStreamWidgetParentPWnd();
extern void SendTargetZCVideoTargetNoPosition(const std::string &strSensorId, const XygsZC_Mark_01 &xygsZC_Target);
extern void SendGimbalDYZCControl(const std::string &strComponent, const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo &controlInfo);
extern void SendGimbalMoveCenter(const std::string &strComponent, const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo &controlInfo);

std::set<int> staticTargetSet
{
	{ 2 },
    { 3 },
    { 4 },
    { 10 }
};

using namespace qnzkna::PlayVideoManage;

const QString                 PlayYoloVideoStreamManageWidget::s_RemoteObjectPrefix(GetVideoStreamRemoteObjectPrefix());

#define TimerStep1 3000
#define  TimerStep01 3000

PlayYoloVideoStreamManageWidget::PlayYoloVideoStreamManageWidget(QWidget *parent)
    : IPlayVideoManage(parent)
    , ui(new Ui::PlayYoloVideoStreamManageWidget)
    , m_Mutex(QMutex::RecursionMode::NonRecursive)
    , m_WId(0)
    , m_VideoDevice("", "", "", "", "", "", "", "", "")
	, m_FocalDis(26.0)
	, m_LongFrame(36.0)
	, m_ShortFrame(24.0)
    , m_Stop(false)
    , m_Pause(false)
    , m_strFileName("")
    , m_ProcessExternalSoftware(nullptr)
    , m_ProcessExternalSoftwareTimes(0)
    , m_ProcessExternalSoftwareTimer()
    , m_GimbalMoveTimer()
    , m_laserMeasureTimer()
    , m_Mutex1(QMutex::RecursionMode::NonRecursive)
    , m_MutexLaser(QMutex::RecursionMode::NonRecursive)
{
    ui->setupUi(this);

    const QString strApplicationDirPath = QCoreApplication::applicationDirPath();
    QSettings settings(strApplicationDirPath + "/Yolo/YoloConfig.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    qDebug() << __FILE__ << __LINE__ << strApplicationDirPath << settings.allKeys() << settings.childKeys();
    settings.beginGroup("YoloConfig");

    bool widgetVideoYoloUse = false;
    if (settings.contains("YoloConfig_VideoYoloUse"))
    {
        widgetVideoYoloUse = settings.value("YoloConfig_VideoYoloUse", "false").toBool();
        qDebug() << __FILE__ << __LINE__ << "INI : YoloConfig_VideoYoloUse exists:" << widgetVideoYoloUse;
    }

    bool widgetVideoOpenCVUse = false;
    if (settings.contains("YoloConfig_VideoOpenCVUse"))
    {
        widgetVideoOpenCVUse = settings.value("YoloConfig_VideoOpenCVUse", "false").toBool();
        qDebug() << __FILE__ << __LINE__ << "INI : YoloConfig_VideoOpenCVUse exists:" << widgetVideoOpenCVUse;
    }
	settings.endGroup();
	settings.beginGroup("YoloModelConfig");
	if (settings.contains("1"))
	{
		YoloConfigTypeMap[1] = settings.value("1", "").toString();
	}
	if (settings.contains("2"))
	{
		YoloConfigTypeMap[2] = settings.value("2", "").toString();
	}
	if (settings.contains("3"))
	{
		YoloConfigTypeMap[3] = settings.value("3", "").toString();
	}
	if (settings.contains("4"))
	{
		YoloConfigTypeMap[4] = settings.value("4", "").toString();
	}


    settings.endGroup();

	ReadYoloVideoPodConfig();

	LoadSendUAVIPIni();

    {
        QWidget *widgetVideoYoloList[] =
        {
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_1,
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_2,
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_3,
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_4,
            ui->checkBox_PlayYoloVideoStreamManageWidget_Yolo,
        };

        for (auto *widgetVideoYolo : widgetVideoYoloList)
        {
            if (widgetVideoYolo)
            {
                widgetVideoYolo->hide();
                widgetVideoYolo->setVisible(false);
            }
        }
    }

    if (widgetVideoYoloUse)
    {
        QWidget *widgetVideoYoloList[] =
        {
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_1,
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_2,
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_3,
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_4,
            ui->checkBox_PlayYoloVideoStreamManageWidget_Yolo,
        };

        for (auto *widgetVideoYolo : widgetVideoYoloList)
        {
            if (widgetVideoYolo)
            {
                widgetVideoYolo->show();
                widgetVideoYolo->setVisible(true);
            }
        }
    }

    if (widgetVideoOpenCVUse)
    {
        QWidget *widgetVideoYoloList[] =
        {
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_1,
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_2,
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_3,
            ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_4,
        };

        for (auto *widgetVideoYolo : widgetVideoYoloList)
        {
            if (widgetVideoYolo)
            {
                widgetVideoYolo->show();
                widgetVideoYolo->setVisible(true);
            }
        }
    }

    m_ProcessExternalSoftwareTimer.setInterval(1000);
    m_ProcessExternalSoftwareTimer.setSingleShot(true);
    connect(&m_ProcessExternalSoftwareTimer, &QTimer::timeout, this, &PlayYoloVideoStreamManageWidget::ExternalSoftwareProcessStarted);

    //m_strDiscernUAVIDList = IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->GetDiscernUAVIDList();

	timerLaser = new QTimer;
	timerLaser->setInterval(1000);
	connect(timerLaser, &QTimer::timeout, this, &PlayYoloVideoStreamManageWidget::LaserMeasureDistance2);

	m_uavTrackTimer = new QTimer;
	m_uavTrackTimer->setInterval(1000);
	connect(m_uavTrackTimer, &QTimer::timeout, this, &PlayYoloVideoStreamManageWidget::uavTrackFlyTimeOut);

	connect(ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_1, &QRadioButton::clicked, this, &PlayYoloVideoStreamManageWidget::on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_1_clicked);
	connect(ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_2, &QRadioButton::clicked, this, &PlayYoloVideoStreamManageWidget::on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_2_clicked);
	connect(ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_3, &QRadioButton::clicked, this, &PlayYoloVideoStreamManageWidget::on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_3_clicked);
	connect(ui->radioButton_PlayYoloVideoStreamManageWidget_TargetType_4, &QRadioButton::clicked, this, &PlayYoloVideoStreamManageWidget::on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_4_clicked);

	connect(ui->checkBox_track, &QCheckBox::clicked, this, &PlayYoloVideoStreamManageWidget::on_checkBox_Track_stateChanged);
}

PlayYoloVideoStreamManageWidget::~PlayYoloVideoStreamManageWidget()
{
    if (m_ProcessExternalSoftware != nullptr)
    {
        emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().video_exit();

        m_ProcessExternalSoftware->terminate();
        m_ProcessExternalSoftware->kill();

        m_ProcessExternalSoftware->waitForFinished(3000);
    }

    delete ui;
}

QWidget *PlayYoloVideoStreamManageWidget::GetPlayYoloVideoStreamManageWidgetParentWidget()
{
    QWidget *pWnd = GetVideoStreamWidgetParentPWnd();
    if (pWnd == nullptr)
    {
        return nullptr;
    }
    return pWnd;
}

QPoint PlayYoloVideoStreamManageWidget::VideoWidgetPos() const
{
    if (ui != nullptr && ui->widget_PlayYoloVideoStreamManageWidget != nullptr)
    {
        return mapToParent(ui->widget_PlayYoloVideoStreamManageWidget->pos());
    }
    return pos();
}

QSize PlayYoloVideoStreamManageWidget::VideoWidgetSize() const
{
    if (ui != nullptr && ui->widget_PlayYoloVideoStreamManageWidget != nullptr)
    {
        return ui->widget_PlayYoloVideoStreamManageWidget->size();
    }
    return size();
}

std::string PlayYoloVideoStreamManageWidget::getStrFileName() const
{
    return m_strFileName;
}

void PlayYoloVideoStreamManageWidget::setStrFileName(const std::string &strFileName)
{
    m_strFileName = strFileName;
}

volatile bool PlayYoloVideoStreamManageWidget::getStop() const
{
    return m_Stop;
}

void PlayYoloVideoStreamManageWidget::setStop(volatile bool Stop)
{
    m_Stop = Stop;
}

volatile bool PlayYoloVideoStreamManageWidget::getPause() const
{
    return m_Pause;
}

void PlayYoloVideoStreamManageWidget::setPause(volatile bool Pause)
{
    m_Pause = Pause;
}

void PlayYoloVideoStreamManageWidget::Lock()
{
    m_Mutex.lock();
}

void PlayYoloVideoStreamManageWidget::UnLock()
{
    m_Mutex.unlock();
}

WId PlayYoloVideoStreamManageWidget::WinId() const
{
    return m_WId;
}

void PlayYoloVideoStreamManageWidget::Show()
{
    QMetaObject::invokeMethod(this, "show", Qt::QueuedConnection);
}

void PlayYoloVideoStreamManageWidget::setVideoType(int videoType)
{
    emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().videoTypeChanged(videoType);
}

#ifdef WIN32
namespace
{

struct handle_data
{
    unsigned long process_id;
    HWND best_handle;
};

BOOL IsMainWindow(HWND handle);

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);

HWND FindMainWindow(long long process_id);

BOOL IsMainWindow(HWND handle)
{
    return GetWindow(handle, GW_OWNER) == (HWND)nullptr && IsWindowVisible(handle);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    handle_data &data = *(handle_data *)lParam;
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);
    if (data.process_id != process_id || !IsMainWindow(handle))
    {
        return true;
    }
    data.best_handle = handle;
    return FALSE;
}

HWND FindMainWindow(long long process_id)
{
    handle_data data;
    data.process_id = process_id;
    data.best_handle = nullptr;
    EnumWindows(EnumWindowsCallback, (LPARAM)&data);
    return data.best_handle;
}
#ifdef UNICODE
WId Pid2WidWC(long long pid, const char *pClassName)
{
    std::wstring wcClassName = QString(pClassName).toStdWString();
    wchar_t szBuf[256];
    HWND hwnd = GetTopWindow(GetDesktopWindow());
    while (hwnd)
    {
        DWORD wndProcID = 0;
        GetWindowThreadProcessId(hwnd, &wndProcID);
        if (wndProcID == pid)
        {
            GetClassName(hwnd, szBuf, sizeof(szBuf));
            if (wcClassName == szBuf)
            {
                return WId(hwnd);
            }
        }
        hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
    }
    return 0;
}
#else
WId Pid2Wid(long long pid, const char *pClassName)
{
    char szBuf[256];
    HWND hwnd = GetTopWindow(GetDesktopWindow());
    while (hwnd)
    {
        DWORD wndProcID = 0;
        GetWindowThreadProcessId(hwnd, &wndProcID);
        if (wndProcID == pid)
        {
            GetClassName(hwnd, szBuf, sizeof(szBuf));
            if (strcmp(szBuf, pClassName) == 0)
            {
                return WId(hwnd);
            }
        }
        hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
    }
    return 0;
}
#endif

}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::GimbalMoveAndCalaDis()
{
    m_Mutex1.lock();
    auto tempMap = m_detectTargetMap;
    m_Mutex1.unlock();
    static int tindex = 0;
    XygsZC_Mark_01 *tempTarget = new XygsZC_Mark_01;
    std::string strComponent = m_VideoDevice.GetVideoComponentID();
    auto &mtr = m_lastTargetInfoMap.find((QString::fromStdString(strComponent)).toLongLong());
    if (mtr != m_lastTargetInfoMap.end())
    {
        tempTarget = &(mtr->second);
    }

    if (!tempMap.empty())
    {
        if (tempTarget->videoWidth != 0)
        {
            bool isPos = false;
            bool hasDis = false;
            bool findLastTarget = false;
            for (auto matchLast : tempMap)
            {
                if (matchLast.second.targetID == tempTarget->targetID)
                {
                    findLastTarget = true;
                    calDIsIndex++;

                    break;
                }
            }

            if (findLastTarget == false)
            {
                tempTarget->videoWidth = 0;

                std::string strComponent = m_VideoDevice.GetVideoComponentID();
                qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo1;
                controlInfo1.componentID = (QString::fromStdString(strComponent)).toLongLong();
                controlInfo1.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_StopTracking;
                controlInfo1.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;
                SendGimbalDYZCControl(strComponent, controlInfo1);
                return;
            }

            auto &ntr = m_laserTargetMap.find((QString::fromStdString(strComponent)).toLongLong());

            if ( (ntr != m_laserTargetMap.end() && !ntr->second.empty()) || calDIsIndex > 5)
            {
                m_laserTargetVec.clear();
                ntr->second.clear();
                auto &mpt = bIslserMap.find((QString::fromStdString(strComponent)).toLongLong());
                if (mpt != bIslserMap.end())
                {
                    mpt->second = false;
                }
                bIslser = false;
                tempTarget->videoWidth = 0;
                calDIsIndex = 0;
                laserIndex = 0;
                return;
            }
            else
            {
                std::string strComponent = m_VideoDevice.GetVideoComponentID();
                qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
                controlInfo.componentID = (QString::fromStdString(strComponent)).toLongLong();
                controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_StopTracking;
                controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;
                SendGimbalDYZCControl(strComponent, controlInfo);

                for (auto matchLast : tempMap)
                {
                    if (matchLast.second.targetID == tempTarget->targetID)
                    {
                        controlInfo.componentID = (QString::fromStdString(strComponent)).toLongLong();
                        controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_MoveCenter;
                        controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;

                        controlInfo.x = matchLast.second.targetRectX + matchLast.second.targetRectWidth / 2;
                        controlInfo.y = matchLast.second.targetRectY + matchLast.second.targetRectHeight / 2;

                        controlInfo.width = matchLast.second.videoWidth;
                        controlInfo.heigth = matchLast.second.videoHeight;

                        if (controlInfo.x < matchLast.second.videoWidth && controlInfo.y < matchLast.second.videoHeight)
                        {
                            SendGimbalMoveCenter(strComponent, controlInfo);
                            return;
                        }
                    }
                }

            }
        }

        int currLaserTarrget = 1;
        auto index_mtr = nLaserIndexMap.find(QString::fromStdString(strComponent));
        if (index_mtr != nLaserIndexMap.end())
        {
            currLaserTarrget = index_mtr->second.second;
        }
        else
        {
            currLaserTarrget = 1;
        }

        QString temp = QString::fromStdString(strComponent) + QString::number(currLaserTarrget);
        auto currIter = tempMap.find(temp);
        if (currIter != tempMap.end())
        {
            if (!strComponent.empty())
            {
                qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
                controlInfo.componentID = currIter->second.componentID;
                controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_MoveCenter;
                controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;

                controlInfo.x = currIter->second.targetRectX + currIter->second.targetRectWidth / 2;
                controlInfo.y = currIter->second.targetRectY + currIter->second.targetRectHeight / 2;

                controlInfo.width = currIter->second.videoWidth;
                controlInfo.heigth = currIter->second.videoHeight;

                if (controlInfo.x < currIter->second.videoWidth && controlInfo.y < currIter->second.videoHeight)
                {
                    SendGimbalMoveCenter(strComponent, controlInfo);
                    auto &mtr = m_lastTargetInfoMap.find((QString::fromStdString(strComponent)).toLongLong());
                    if (mtr != m_lastTargetInfoMap.end())
                    {
                        mtr->second = currIter->second;
                    }
                    else
                    {
                        m_lastTargetInfoMap[(QString::fromStdString(strComponent)).toLongLong()] = currIter->second;
                    }
                    m_lastTargetInfo = currIter->second;

                    if (bIslserMap.find((QString::fromStdString(strComponent)).toLongLong()) == bIslserMap.end())
                    {
                        bIslserMap[(QString::fromStdString(strComponent)).toLongLong()] = false;
                    }

                    auto &mmtr = nLaserIndexMap.find(QString::fromStdString(strComponent));
                    if (mmtr != nLaserIndexMap.end())
                    {
                        if (mmtr->second.second == 1)
                        {
                            mmtr->second.first = true;
                        }
                        if (tempMap.size() > 1 && mmtr ->second.second == tempMap.size())
                        {
                            mmtr->second.first = false;
                        }
                        if (mmtr->second.first)
                        {
                            mmtr->second.second = mmtr->second.second + 1;
                        }
                        else
                        {
                            mmtr->second.second = mmtr->second.second - 1;
                        }
                    }
                    else
                    {
                        nLaserIndexMap[QString::fromStdString(strComponent)] = std::make_pair(true, 1);
                        TargetID_PMap[temp] = m_strDiscernUAVIDList[tindex];
                        tindex++;
                    }

                }
            }
        }
        else
        {
            auto &mmtr = nLaserIndexMap.find(QString::fromStdString(strComponent));
            if (mmtr != nLaserIndexMap.end())
            {
                mmtr->second.second = 1;
                mmtr->second.first = true;
            }
            else
            {
                nLaserIndexMap[QString::fromStdString(strComponent)] = std::make_pair(true, 1);
            }
        }
    }
    else
    {
    }
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::GimbalMoveAndCalaDis2()
{
	m_Mutex1.lock();
	auto tempMap = m_detectTargetMap;
	m_Mutex1.unlock();
	static int tindex = 0;
	XygsZC_Mark_01 *tempTarget = new XygsZC_Mark_01;
	std::string strComponent = m_VideoDevice.GetVideoComponentID();
	auto &mtr = m_lastTargetInfoMap.find((QString::fromStdString(strComponent)).toLongLong());
	if (mtr != m_lastTargetInfoMap.end())
	{
		tempTarget = &(mtr->second);
	}

	if (!tempMap.empty())
	{
		if (tempTarget->videoWidth != 0)
		{
			bool isPos = false;
			bool hasDis = false;
			bool findLastTarget = false;
			for (auto matchLast : tempMap)
			{
				if (matchLast.second.targetID == tempTarget->targetID)
				{
					findLastTarget = true;
					calDIsIndex++;

					break;
				}
			}

			auto &ntr = m_laserTargetMap.find((QString::fromStdString(strComponent)).toLongLong());

			if ((ntr != m_laserTargetMap.end() && !ntr->second.empty()) || calDIsIndex > 5)
			{
				m_laserTargetVec.clear();
				ntr->second.clear();
				auto &mpt = bIslserMap.find((QString::fromStdString(strComponent)).toLongLong());
				if (mpt != bIslserMap.end())
				{
					mpt->second = false;
				}
				bIslser = false;
				tempTarget->videoWidth = 0;
				calDIsIndex = 0;
				laserIndex = 0;
				return;
			}
			else
			{
				std::string strComponent = m_VideoDevice.GetVideoComponentID();
				qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
				controlInfo.componentID = (QString::fromStdString(strComponent)).toLongLong();
				controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_StopTracking;
				controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;
				//SendGimbalDYZCControl(strComponent, controlInfo);

				int curIndex = 0;
				for (auto matchLast = tempMap.begin(); matchLast != tempMap.end(); matchLast++)
				{
					if (matchLast->second.targetID == tempTarget->targetID)
					{
						if (curIndex < tempMap.size() -1)
						{
							tempTarget = &((matchLast++)->second);
							break;
						}
						else
						{
							tempTarget = &(tempMap.begin()->second);
							break;
						}
						
						//controlInfo.componentID = (QString::fromStdString(strComponent)).toLongLong();
						//controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_MoveCenter;
						//controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;
						//controlInfo.x = matchLast.second.targetRectX + matchLast.second.targetRectWidth / 2;
						//controlInfo.y = matchLast.second.targetRectY + matchLast.second.targetRectHeight / 2;
						//controlInfo.width = matchLast.second.videoWidth;
						//controlInfo.heigth = matchLast.second.videoHeight;
						//if (controlInfo.x < matchLast.second.videoWidth && controlInfo.y < matchLast.second.videoHeight)
						//{
						//	//SendGimbalMoveCenter(strComponent, controlInfo);
						//	return;
						//}
					}
					curIndex++;
				}

			}
		}

		int currLaserTarrget = 1;
		auto index_mtr = nLaserIndexMap.find(QString::fromStdString(strComponent));
		if (index_mtr != nLaserIndexMap.end())
		{
			currLaserTarrget = index_mtr->second.second;
		}
		else
		{
			currLaserTarrget = 1;
		}

		QString temp = QString::fromStdString(strComponent) + QString::number(currLaserTarrget);
		auto currIter = tempMap.find(temp);
		if (currIter != tempMap.end())
		{
			if (!strComponent.empty())
			{
				qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
				controlInfo.componentID = currIter->second.componentID;
				controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_MoveCenter;
				controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;

				controlInfo.x = currIter->second.targetRectX + currIter->second.targetRectWidth / 2;
				controlInfo.y = currIter->second.targetRectY + currIter->second.targetRectHeight / 2;

				controlInfo.width = currIter->second.videoWidth;
				controlInfo.heigth = currIter->second.videoHeight;

				if (controlInfo.x < currIter->second.videoWidth && controlInfo.y < currIter->second.videoHeight)
				{
					//SendGimbalMoveCenter(strComponent, controlInfo);
					
					auto &mtr = m_lastTargetInfoMap.find((QString::fromStdString(strComponent)).toLongLong());
					if (mtr != m_lastTargetInfoMap.end())
					{
						mtr->second = currIter->second;
					}
					else
					{
						m_lastTargetInfoMap[(QString::fromStdString(strComponent)).toLongLong()] = currIter->second;
					}
					m_lastTargetInfo = currIter->second;

					if (bIslserMap.find((QString::fromStdString(strComponent)).toLongLong()) == bIslserMap.end())
					{
						bIslserMap[(QString::fromStdString(strComponent)).toLongLong()] = false;
					}

					auto &mmtr = nLaserIndexMap.find(QString::fromStdString(strComponent));
					if (mmtr != nLaserIndexMap.end())
					{
						if (mmtr->second.second == 1)
						{
							mmtr->second.first = true;
						}
						if (tempMap.size() > 1 && mmtr->second.second == tempMap.size())
						{
							mmtr->second.first = false;
						}
						if (mmtr->second.first)
						{
							mmtr->second.second = mmtr->second.second + 1;
						}
						else
						{
							mmtr->second.second = mmtr->second.second - 1;
						}
					}
					else
					{
						nLaserIndexMap[QString::fromStdString(strComponent)] = std::make_pair(true, 1);
						TargetID_PMap[temp] = m_strDiscernUAVIDList[tindex];
						tindex++;
					}

				}
			}
		}
		else
		{
			auto &mmtr = nLaserIndexMap.find(QString::fromStdString(strComponent));
			if (mmtr != nLaserIndexMap.end())
			{
				mmtr->second.second = 1;
				mmtr->second.first = true;
			}
			else
			{
				nLaserIndexMap[QString::fromStdString(strComponent)] = std::make_pair(true, 1);
			}
		}
	}
	else
	{
	}
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::LaserMeasureDistance()
{
    std::string strComponent = m_VideoDevice.GetVideoComponentID();
    m_MutexLaser.lock();
    auto tempMap = m_detectTargetMap;
    m_MutexLaser.unlock();

    XygsZC_Mark_01 tempTarget ;
    auto &mtr = m_lastTargetInfoMap.find((QString::fromStdString(strComponent)).toLongLong());
    if (mtr != m_lastTargetInfoMap.end())
    {
        tempTarget = mtr->second;
    }

    qDebug() << "tempTarget11111111111111" << tempTarget.componentID << ":" << tempTarget.targetID;

    if (!tempMap.empty())
    {
        {
            for (auto matchLast : tempMap)
            {
                if (matchLast.second.targetID == tempTarget.targetID)
                {
                    if (abs(matchLast.second.targetRectX + matchLast.second.targetRectWidth / 2 - matchLast.second.videoWidth / 2) < 40 &&
                            abs(matchLast.second.targetRectY + matchLast.second.targetRectHeight / 2 - matchLast.second.videoHeight / 2) < 40)
                    {
                        QString temp = QString::fromStdString(strComponent) + QString::number(tempTarget.targetID);
                        IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->
                        SetCurrUAVRectID((QString::fromStdString(strComponent)).toLongLong(), matchLast.second);

                        if (staticTargetSet.find(matchLast.second.targetType) != staticTargetSet.end())
                        {
                            if (std::find(targetSetVec.begin(), targetSetVec.end(), tempTarget.targetID) == targetSetVec.end())
                            {
                                QString dtTime = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");
                                QString tar_id = dtTime + temp;
                                IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->
                                SetPicTargetID(QString::fromStdString(strComponent).toLongLong(), tar_id);
                                targetSetVec.push_back(tempTarget.targetID);
                            }
                        }

                        auto &mpt = bIslserMap.find((QString::fromStdString(strComponent)).toLongLong());
                        if (mpt != bIslserMap.end())
                        {
                            mpt->second = true;
                        }
                        bIslser = true;
                        qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
                        controlInfo.componentID = (QString::fromStdString(strComponent)).toLongLong();
                        controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_SingleLaserTelemeter;
                        controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;
                        SendGimbalDYZCControl(strComponent, controlInfo);
                        break;
                    }
                }
            }
        }
    }
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::LaserMeasureDistance2()
{
	std::string strComponent = m_VideoDevice.GetVideoComponentID();
	m_MutexLaser.lock();
	auto tempMap = m_LaserTargetMap;
	m_MutexLaser.unlock();

	XygsZC_Mark_01 tempTarget;
	auto &mtr = tempMap.find((QString::fromStdString(strComponent)));
	if (mtr != tempMap.end())
	{
		tempTarget = mtr->second;
	}

	if (!tempMap.empty())
	{
		{
			for (auto matchLast : tempMap)
			{
				if (matchLast.second.targetID == tempTarget.targetID)
				{
					//if (abs(matchLast.second.targetRectX + matchLast.second.targetRectWidth / 2 - matchLast.second.videoWidth / 2) < 40 &&
					//	abs(matchLast.second.targetRectY + matchLast.second.targetRectHeight / 2 - matchLast.second.videoHeight / 2) < 40)
					{
						QString temp = QString::fromStdString(strComponent) + QString::number(tempTarget.targetID);
						IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->
							SetCurrUAVRectID((QString::fromStdString(strComponent)).toLongLong(), matchLast.second);

						if (staticTargetSet.find(matchLast.second.targetType) != staticTargetSet.end())
						{
							if (std::find(targetSetVec.begin(), targetSetVec.end(), tempTarget.targetID) == targetSetVec.end())
							{
								QString dtTime = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");
								QString tar_id = dtTime + temp;
								IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->
									SetPicTargetID(QString::fromStdString(strComponent).toLongLong(), tar_id);
								targetSetVec.push_back(tempTarget.targetID);
							}
						}

						auto &mpt = bIslserMap.find((QString::fromStdString(strComponent)).toLongLong());
						if (mpt != bIslserMap.end())
						{
							mpt->second = true;
						}
						//bIslser = true;
						qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
						controlInfo.componentID = (QString::fromStdString(strComponent)).toLongLong();
						controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_SingleLaserTelemeter;
						controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;
						SendGimbalDYZCControl(strComponent, controlInfo);
						break;
					}
				}
			}
		}
	}
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::calcMeasureDistance()
{
	std::string strComponent = m_VideoDevice.GetVideoComponentID();
	m_MutexLaser.lock();
	auto tempMap = m_detectTargetMap;
	m_MutexLaser.unlock();

	XygsZC_Mark_01 tempTarget;
	auto &mtr = m_lastTargetInfoMap.find((QString::fromStdString(strComponent)).toLongLong());
	if (mtr != m_lastTargetInfoMap.end())
	{
		tempTarget = mtr->second;
	}

	if (!tempMap.empty())
	{
		for (auto matchLast : tempMap)
		{
			if (matchLast.second.targetID == tempTarget.targetID)
			{
				QString temp = QString::fromStdString(strComponent) + QString::number(tempTarget.targetID);
				IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->
					SetCurrUAVRectID((QString::fromStdString(strComponent)).toLongLong(), matchLast.second);
				//if (abs(matchLast.second.targetRectX + matchLast.second.targetRectWidth / 2 - matchLast.second.videoWidth / 2) < 40 &&
					//abs(matchLast.second.targetRectY + matchLast.second.targetRectHeight / 2 - matchLast.second.videoHeight / 2) < 40)
				{
					//QGeoCoordinate pos = CalcTargetPosFromPod(strComponent, tempTarget);
					QGeoCoordinate pos = CalcTargetPosFromPod1(strComponent, tempTarget);
					
					if (pos.longitude() != 0 && pos.latitude() != 0)
					{
						tempTarget.target_lon = pos.longitude();
						tempTarget.target_lat = pos.latitude();

						std::vector<std::string> sensorVec;
						std::string WeaponComponentName = strComponent;
						IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()
							->GetSensorIdsInWeaponComponent(WeaponComponentName, &sensorVec);
						QString strSensorID = "1000307";
						if (!sensorVec.empty())
						{
							strSensorID = QString::fromStdString(sensorVec[0]);
						}

						qnzkna::framework::MessageReferenceProps props;
						props.put("TargetType", std::to_string(NODE_TYPE_SENSOR));
						props.put("TargetID", strSensorID.toStdString());
						props.put("WeaponID", QString::number(tempTarget.componentID).toStdString());

						IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
							new qnzkna::framework::MessageReference("MsgZCTargetPosition", (char *)&tempTarget, sizeof(tempTarget), props));
						
					}

				}
			}
		}
	}
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::uavTrackFlyTimeOut()
{
	if (!trackFlyVec.empty())
	{
		std::string strComponent = m_VideoDevice.GetVideoComponentID();
		m_MtexUavTrack.lock();
		auto tempMap = trackFlyVec;
		m_MtexUavTrack.unlock();

		XygsZC_Mark_01 tempTarget = trackFlyVec[0];

		double x = tempTarget.targetRectX + tempTarget.targetRectWidth / 2;
		double y = tempTarget.targetRectY + tempTarget.targetRectHeight / 2;

		if (abs(x - tempTarget.videoWidth / 2)  > tempTarget.videoWidth / 4 || abs(y -tempTarget.videoHeight / 2) > tempTarget.videoHeight / 4)
		{
			qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stateInfo;
			const bool getUAVRealTimeState = IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService()->
				GetUAVRealTimeState(strComponent, stateInfo);
			QGeoCoordinate uavCurrPos;
			double uavAltitude = stateInfo.telemetryInfo.relativeGroundHeigth;
			if (getUAVRealTimeState )
			{
				uavCurrPos = QGeoCoordinate(stateInfo.telemetryInfo.position.lat, stateInfo.telemetryInfo.position.lng);
			}
			//QGeoCoordinate pos = CalcTargetPosFromPod(strComponent, tempTarget);
			QGeoCoordinate pos = CalcTargetPosFromPod1(strComponent, tempTarget);
		
			
			if (pos.longitude() != 0 && pos.latitude() != 0)
			{
				//限制飞机移动距离
				double delta_Lat = pos.latitude() - uavCurrPos.latitude();
				double delta_Lon = pos.longitude() - uavCurrPos.longitude();
				//设定系数
				double coeff = 0.2;
				pos = QGeoCoordinate(pos.latitude() + delta_Lat * coeff, pos.longitude() + (delta_Lon *coeff));

				tempTarget.target_lon = pos.longitude();
				tempTarget.target_lat = pos.latitude();
				//距离安全-5米

				bool isDisSafe = false;
				if (getUAVRealTimeState)
				{
					double dLon = stateInfo.telemetryInfo.position.lng;
					double dLan = stateInfo.telemetryInfo.position.lat;
					
					QGeoCoordinate posUAV(dLan, dLon);
					if (posUAV.distanceTo(pos) < 50)
					{
						isDisSafe = true;
					}
				}
				if (isDisSafe == false)
				{
					LyMessageBox::information(nullptr, tr2("提示"), tr2("超出安全距离"));
					return;
				}
				double dis_fly = uavCurrPos.distanceTo(pos);
				if (dis_fly > 5)
				{
					double azimuth_ = uavCurrPos.azimuthTo(pos);
					pos = uavCurrPos.atDistanceAndAzimuth(5, azimuth_);
					tempTarget.target_lon = pos.longitude();
					tempTarget.target_lat = pos.latitude();
				}

				////障碍判断ax+by+c=0
				//double safeDisAmong = 5;//
				//double safeDisHeight = 5;
				//double a = pos.latitude() - uavCurrPos.latitude();
				//double b = uavCurrPos.longitude() - pos.longitude();
				//double c = pos.longitude() * pos.latitude() - uavCurrPos.longitude()*pos.latitude();
				//qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap stateList;
				//bool bExistUAVReal = IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService()->GetUAVRealTimeStateList(&stateList);
				//
				//if (bExistUAVReal)
				//{
				//	for (auto uavID : stateList)
				//	{
				//		qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stateInfo1;
				//		const bool getUAVRealTimeState = IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService()->
				//			GetUAVRealTimeState(uavID.first, stateInfo1);
				//		if (getUAVRealTimeState)
				//		{
				//			if (uavID.first == strComponent)
				//			{
				//				continue;
				//			}
				//			double uav2lineDis = (a * stateInfo1.telemetryInfo.position.lng + b * stateInfo1.telemetryInfo.position.lat + c)
				//				/ (std::sqrt(a*a + b*b));
				//			double uav2Height = std::abs(uavAltitude - stateInfo1.telemetryInfo.relativeGroundHeigth);
				//			if (uav2lineDis < safeDisAmong && uav2Height < safeDisHeight)
				//			{
				//				if (QMessageBox::No != LyMessageBox::question(this, tr2("提示"), tr2("当前跟踪存在碰撞风险，是否继续跟踪？"), QMessageBox::Yes | QMessageBox::No))
				//				{
				//					return;
				//				}
				//			}
				//		}
				//	}
				//}

				//触发指点飞行
				std::vector<std::string> sensorVec;
				std::string WeaponComponentName = strComponent;
				IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()
					->GetSensorIdsInWeaponComponent(WeaponComponentName, &sensorVec);
				QString strSensorID = "1000307";
				if (!sensorVec.empty())
				{
					strSensorID = QString::fromStdString(sensorVec[0]);
				}

				//if (QMessageBox::Yes != LyMessageBox::information(nullptr, tr2("提示"), tr2("是否确认将 [%1] 装备移动到任务点？").arg(str), QMessageBox::Yes | QMessageBox::No))
				//{
				//	return;
				//}
				double dAlt = stateInfo.telemetryInfo.relativeGroundHeigth;
				char pBuf[MAX_BUFFER_LEN] = { 0 };
				int nBufLen = 0;
				qnzkna::framework::MessageReferenceProps props;
				props.put("TargetID", "");
				props.put("TargetLon", QString::number((tempTarget.target_lon),'f',7).toStdString());
				props.put("TargetLat", std::to_string(tempTarget.target_lat));
				props.put("TargetAlt", std::to_string(dAlt));
				props.put("UAVID", strComponent);
				IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgSelectTargetFlyGoTo", pBuf, nBufLen, props));

				//qnzkna::framework::MessageReferenceProps props;
				//props.put("TargetType", std::to_string(NODE_TYPE_SENSOR));
				//props.put("TargetID", strSensorID.toStdString());
				//props.put("WeaponID", QString::number(tempTarget.componentID).toStdString());
				//IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
				//	new qnzkna::framework::MessageReference("MsgUavTrackFly", (char *)&tempTarget, sizeof(tempTarget), props));
			}
		}
	}
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::ReadYoloVideoPodConfig()
{
	QSettings settings("Data/conf/PlayYoloVideoPodConfig.ini", QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("PlayYoloVideoPodConfig");

	if (settings.contains("AddTargetToDB") && settings.value("AddTargetToDB").canConvert(QVariant::Type::Bool)) {
		const double AddTargetToDB = settings.value("AddTargetToDB", "true").toBool();
		m_AddTargetToDB = AddTargetToDB;
	}

	if (settings.contains("FocalDis") && settings.value("FocalDis").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double FocalDis = settings.value("FocalDis", "26").toDouble(&ok);
		if (ok && FocalDis > 0) {
			m_FocalDis = FocalDis;
		}
	}

	if (settings.contains("LongFrame") && settings.value("LongFrame").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double LongFrame = settings.value("LongFrame", "36").toDouble(&ok);
		if (ok && LongFrame > 0) {
			m_LongFrame = LongFrame;
		}
	}

	if (settings.contains("ShortFrame") && settings.value("ShortFrame").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double ShortFrame = settings.value("ShortFrame", "24").toDouble(&ok);
		if (ok && ShortFrame > 0) {
			m_ShortFrame = ShortFrame;
		}
	}

	if (settings.contains("FocalMin") && settings.value("FocalMin").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double FocalMin = settings.value("FocalMin", "4.7").toDouble(&ok);
		if (ok && FocalMin > 0) {
			m_FocalMin = FocalMin;
		}
	}

	if (settings.contains("FocalMax") && settings.value("FocalMax").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double FocalMax = settings.value("FocalMax", "47").toDouble(&ok);
		if (ok && FocalMax > 0) {
			m_FocalMax = FocalMax;
		}
	}

	if (settings.contains("ViewFrontMax") && settings.value("ViewFrontMax").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double ViewFrontMax = settings.value("ViewFrontMax", "36.9").toDouble(&ok);
		if (ok && ViewFrontMax > 0) {
			m_ViewFrontMax = ViewFrontMax;
		}
	}

	if (settings.contains("ViewFrontMin") && settings.value("ViewFrontMin").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double ViewFrontMin = settings.value("ViewFrontMin", "3.9").toDouble(&ok);
		if (ok && ViewFrontMin > 0) {
			m_ViewFrontMin = ViewFrontMin;
		}
	}

	if (settings.contains("ViewLeftMax") && settings.value("ViewLeftMax").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double ViewLeftMax = settings.value("ViewLeftMax", "61.3").toDouble(&ok);
		if (ok && ViewLeftMax > 0) {
			m_ViewLeftMax = ViewLeftMax;
		}
	}

	if (settings.contains("ViewLeftMin") && settings.value("ViewLeftMin").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double ViewLeftMin = settings.value("ViewLeftMin", "6.8").toDouble(&ok);
		if (ok && ViewLeftMin > 0) {
			m_ViewLeftMin = ViewLeftMin;
		}
	}

	if (settings.contains("MaxZoom") && settings.value("MaxZoom").canConvert(QVariant::Type::Double)) {
		bool ok = false;
		const double MaxZoom = settings.value("MaxZoom", "10").toDouble(&ok);
		if (ok && MaxZoom > 0) {
			m_MaxZoom = MaxZoom;
		}
	}

	settings.setValue("AddTargetToDB", m_AddTargetToDB);
	settings.setValue("FocalDis", m_FocalDis);
	settings.setValue("LongFrame", m_LongFrame);
	settings.setValue("ShortFrame", m_ShortFrame);
	settings.endGroup();
}

QGeoCoordinate qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::CalcTargetPosFromPod(std::string ComponentID, XygsZC_Mark_01 xygsZC_Target)
{
	QGeoCoordinate tempPos(0,0);
	if (IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService() == nullptr) {
		return tempPos;
	}

	if (IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService() == nullptr) {
		return tempPos;
	}
	qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t teleInfomation;
	IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService()->GetUAVRealTimeState(QString::number(xygsZC_Target.componentID).toStdString(), teleInfomation);
	//传感器尺寸
	const double FocalDis = m_FocalDis;   //26.00;
	const double LongFrame = m_LongFrame;  //36.00;
	const double ShortFrame = m_ShortFrame; //24.00;
	//视野覆盖实际距离
	const double wid = teleInfomation.telemetryInfo.relativeOriginHeigth * LongFrame / FocalDis;
	const double Hig = teleInfomation.telemetryInfo.relativeOriginHeigth * ShortFrame / FocalDis;

	//框大小
	const double nrWid = xygsZC_Target.targetRectWidth *  wid / xygsZC_Target.videoWidth;
	const double nrHig = xygsZC_Target.targetRectHeight * Hig / xygsZC_Target.videoHeight;
	//计算目标位置
	const double TarAngle = /*90 - */atan2(xygsZC_Target.targetRectY, xygsZC_Target.targetRectX);//图上角度

	const int dx_ = std::abs((xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2) - xygsZC_Target.videoWidth / 2) *  wid / xygsZC_Target.videoWidth;
	const int dy_ = std::abs((xygsZC_Target.targetRectY - xygsZC_Target.targetRectHeight / 2) - xygsZC_Target.videoHeight / 2) * Hig / xygsZC_Target.videoHeight;
	const double picDis = sqrt(dx_*dx_ + dy_*dy_);

	const double CenterPosLon = teleInfomation.telemetryInfo.position.lng; // 0
	const double CneterPosLat = teleInfomation.telemetryInfo.position.lat;

	const QGeoCoordinate posCenter(CneterPosLat, CenterPosLon);
	//计算经纬度
	QGeoCoordinate posTarget = posCenter.atDistanceAndAzimuth(picDis, (TarAngle + teleInfomation.telemetryInfo.angleofRudder));
	tempPos = posTarget;
	return tempPos;
}

QGeoCoordinate qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::CalcTargetPosFromPod1(std::string ComponentID, XygsZC_Mark_01 xygsZC_Target)
{
	QGeoCoordinate tempPos(0, 0);
	if (IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService() == nullptr) {
		return tempPos;
	}

	if (IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService() == nullptr) {
		return tempPos;
	}
	qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t teleInfomation;
	IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService()->GetUAVRealTimeState(QString::number(xygsZC_Target.componentID).toStdString(), teleInfomation);

	//FOV
	const std::string strPlatformId = m_VideoDevice.GetVideoComponentID();
	//qnzkna::SensorManage::SensorInfoMap tmpSensorInfoMap;
	//bool bGetSensor = IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()
	//	->GetSensorInfosInWeaponComponent(strPlatformId, &tmpSensorInfoMap);
	//if (bGetSensor)
	//{

	//}
	//IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()->GetSensorInfo()
	//传感器尺寸
	//const double FocalDis = m_FocalDis;   //26.00;
	//const double LongFrame = m_LongFrame;  //36.00;
	//const double ShortFrame = m_ShortFrame; //24.00;

	//FOV暂时取最小值
	int dnFocal = LY_UAVNavigationArmShotAlone54::Get().currFocal;//currFocal;
	double deltaFocal = (dnFocal*1.0 - 1) / (m_MaxZoom*1.0 - 1);
	const double FocalVale = m_FocalMin + (m_FocalMax - m_FocalMin) * deltaFocal ;
	const double ViewFront = m_ViewFrontMax - (m_ViewFrontMax - m_ViewFrontMin) * deltaFocal;
	const double ViewLeft = m_ViewLeftMax - (m_ViewLeftMax - m_ViewLeftMin) * deltaFocal;
	//实际覆盖距离
	double FrontAngleHalf = ViewFront / 2.0;
	double LeftAngleHalf = ViewLeft / 2.0;

	double height_m = 0.2;
	double FontDis = (teleInfomation.telemetryInfo.relativeOriginHeigth - height_m) * tan(FrontAngleHalf * PI / 180);
	double WidthDis = (teleInfomation.telemetryInfo.relativeOriginHeigth - height_m) * tan(LeftAngleHalf* PI / 180);

	//视野覆盖实际距离
	//const double wid = teleInfomation.telemetryInfo.relativeOriginHeigth * LongFrame / FocalDis;
	//const double Hig = teleInfomation.telemetryInfo.relativeOriginHeigth * ShortFrame / FocalDis;
	const double wid = std::abs(WidthDis * 2.0);
	const double Hig = std::abs(FontDis * 2.0);

	////框大小
	//const double nrWid = xygsZC_Target.targetRectWidth *  wid / xygsZC_Target.videoWidth;
	//const double nrHig = xygsZC_Target.targetRectHeight * Hig / xygsZC_Target.videoHeight;

	//单位像素距离
	double delta_x = wid / xygsZC_Target.videoWidth;
	double delta_y = Hig / xygsZC_Target.videoHeight;

	////计算目标角度(弧度)
	//const double TarAngle = /*90 - */atan2(xygsZC_Target.targetRectY, xygsZC_Target.targetRectX);//图上角度
	//double TarAngle1 = atan2(((xygsZC_Target.targetRectY - xygsZC_Target.targetRectHeight / 2) - xygsZC_Target.videoHeight / 2)
	//	, (xygsZC_Target.videoHeight / 2 - (xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2)));


	double dx_ = ((xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2) - xygsZC_Target.videoWidth / 2) * delta_x;
	double dy_ = (xygsZC_Target.videoHeight / 2 - (xygsZC_Target.targetRectY + xygsZC_Target.targetRectHeight / 2)) * delta_y;
	//const double picDis = sqrt(dx_*dx_ + dy_*dy_);

	const double CenterPosLon = teleInfomation.telemetryInfo.position.lng; // 0
	const double CneterPosLat = teleInfomation.telemetryInfo.position.lat;

	const QGeoCoordinate posCenter(CneterPosLat, CenterPosLon);
	//////////////////////////////////////////////////////////////////////////
	double currHeading = teleInfomation.telemetryInfo.angleofRudder * PI / 180.0;
	QGeoCoordinate pos = posCenter.atDistanceAndAzimuth(0.2, currHeading);

	double dx_enu = dx_*cos(currHeading) + dy_*sin(currHeading);
	double dy_enu = dx_*(-sin(currHeading)) + dy_*cos(currHeading);
	//double dis_enu = sqrt(dx_enu*dx_enu + dy_enu*dy_enu);
	//double enu_Angle = atan2(dy_enu, dx_enu);

	double earth_r = 6371393;
	double delta_Lat = dy_enu / earth_r *180.0 / PI;
	double delta_Lon = dx_enu / (earth_r * cos(CneterPosLat))*180.0 / PI;

	QGeoCoordinate posTarget = QGeoCoordinate(pos.latitude() + delta_Lat, pos.longitude() + delta_Lon);
	//QGeoCoordinate posTarget = posCenter.atDistanceAndAzimuth(dis_enu, enu_Angle);
	//////////////////////////////////////////////////////////////////////////
	////计算经纬度
	////QGeoCoordinate posTarget = posCenter.atDistanceAndAzimuth(picDis, (TarAngle + teleInfomation.telemetryInfo.angleofRudder));
	//double curAngle = TarAngle1 * 180 / PI + teleInfomation.telemetryInfo.angleofRudder;
	//curAngle = curAngle > 360 ? curAngle - 360 : curAngle;
	//curAngle = curAngle * PI / 180;

	//double x_Angle = (xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2) - xygsZC_Target.videoWidth / 2
	//	 > 0 ? 90 : 270;
	//double y_Angle = (xygsZC_Target.targetRectY + xygsZC_Target.targetRectHeight / 2) - xygsZC_Target.videoHeight / 2
	//	> 0 ? 180 : 0;
	/////posCenter
	//QGeoCoordinate pos_X = posCenter.atDistanceAndAzimuth(dx_, teleInfomation.telemetryInfo.angleofRudder + 90);
	//QGeoCoordinate pos_Y = posCenter.atDistanceAndAzimuth(dy_, teleInfomation.telemetryInfo.angleofRudder);
	////QGeoCoordinate posTarget = posCenter.atDistanceAndAzimuth(picDis, curAngle/*(TarAngle1 + teleInfomation.telemetryInfo.angleofRudder )*/);
	//QGeoCoordinate posTarget = QGeoCoordinate(pos_Y.latitude(), pos_X.longitude());
	//double nDis = sqrt(dx_*dx_ + dy_*dy_);
	//QGeoCoordinate halfPos((pos_X.latitude() + pos_Y.latitude()) / 2, (pos_X.longitude() + pos_Y.longitude()) / 2);
	//double nAngle = posCenter.azimuthTo(halfPos);
	//posTarget = posCenter.atDistanceAndAzimuth(nDis, nAngle);

	////QGeoCoordinate posTarget = QGeoCoordinate(pos_X.latitude(), pos_Y.longitude());
	tempPos = posTarget;
	return tempPos;
}


void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::LoadSendUAVIPIni()
{
	QFile file("./Data/conf/TargetSendToServer.ini");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();
		QStringList values = line.split(" ");
		if (values.size() == 3)
		{
			QString strValue = values[0];
			m_strDiscernUAVIDList.push_back(strValue);
		}
	}
}

//QGeoCoordinate qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::CalcTargetPosFromPod1(std::string ComponentID, XygsZC_Mark_01 xygsZC_Target)
//{
//	QGeoCoordinate tempPos(0, 0);
//	if (IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService() == nullptr) {
//		return tempPos;
//	}
//
//	if (IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService() == nullptr) {
//		return tempPos;
//	}
//	qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t teleInfomation;
//	IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService()->GetUAVRealTimeState(QString::number(xygsZC_Target.componentID).toStdString(), teleInfomation);
//	
//	//FOV
//	const std::string strPlatformId = m_VideoDevice.GetVideoComponentID();
//	//qnzkna::SensorManage::SensorInfoMap tmpSensorInfoMap;
//	//bool bGetSensor = IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()
//	//	->GetSensorInfosInWeaponComponent(strPlatformId, &tmpSensorInfoMap);
//	//if (bGetSensor)
//	//{
//
//	//}
//	//IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()->GetSensorInfo()
//	//传感器尺寸
//	//const double FocalDis = m_FocalDis;   //26.00;
//	//const double LongFrame = m_LongFrame;  //36.00;
//	//const double ShortFrame = m_ShortFrame; //24.00;
//	//FOV暂时取最小值
//	const double FocalVale = m_FocalMin;
//	const double ViewFront = m_ViewFrontMax;
//	const double ViewLeft = m_ViewLeftMax;
//	//实际覆盖距离
//	double FrontAngleHalf = m_ViewFrontMax / 2;
//	double LeftAngleHalf = m_ViewLeftMax / 2;
//
//	double height_m = 50.0;
//	double LongDis = teleInfomation.telemetryInfo.relativeOriginHeigth * tan(FrontAngleHalf * PI / 180);
//	double WidthDis = teleInfomation.telemetryInfo.relativeOriginHeigth * tan(LeftAngleHalf* PI / 180);
//
//	//视野覆盖实际距离
//	//const double wid = teleInfomation.telemetryInfo.relativeOriginHeigth * LongFrame / FocalDis;
//	//const double Hig = teleInfomation.telemetryInfo.relativeOriginHeigth * ShortFrame / FocalDis;
//	const double wid = std::abs(LongDis * 2);
//	const double Hig = std::abs(WidthDis * 2);
//
//	////框大小
//	//const double nrWid = xygsZC_Target.targetRectWidth *  wid / xygsZC_Target.videoWidth;
//	//const double nrHig = xygsZC_Target.targetRectHeight * Hig / xygsZC_Target.videoHeight;
//
//	//单位像素距离
//	double delta_x = wid / xygsZC_Target.videoWidth;
//	double delta_y = Hig / xygsZC_Target.videoHeight;
//
//	//计算目标角度(弧度)
//	const double TarAngle = /*90 - */atan2(xygsZC_Target.targetRectY, xygsZC_Target.targetRectX);//图上角度
//	double TarAngle1 = atan2(((xygsZC_Target.targetRectY - xygsZC_Target.targetRectHeight / 2) - xygsZC_Target.videoHeight / 2)
//						, (xygsZC_Target.videoHeight / 2 -(xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2)));
//
//	//const int dx_ = std::abs((xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2) - xygsZC_Target.videoWidth / 2) *  wid / xygsZC_Target.videoWidth;
//	//const int dy_ = std::abs((xygsZC_Target.targetRectY - xygsZC_Target.targetRectHeight / 2) - xygsZC_Target.videoHeight / 2) * Hig / xygsZC_Target.videoHeight;
//	//const double picDis = sqrt(dx_*dx_ + dy_*dy_);
//
//	double dx_ = ((xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2) - xygsZC_Target.videoWidth / 2) * delta_x;
//	double dy_ = (xygsZC_Target.videoHeight / 2 - (xygsZC_Target.targetRectY + xygsZC_Target.targetRectHeight / 2)) * delta_y;
//	const double picDis = sqrt(dx_*dx_ + dy_*dy_);
//
//	const double CenterPosLon = teleInfomation.telemetryInfo.position.lng; // 0
//	const double CneterPosLat = teleInfomation.telemetryInfo.position.lat;
//
//	const QGeoCoordinate posCenter(CneterPosLat, CenterPosLon);
//	//计算经纬度
//	//QGeoCoordinate posTarget = posCenter.atDistanceAndAzimuth(picDis, (TarAngle + teleInfomation.telemetryInfo.angleofRudder));
//	double curAngle = TarAngle1 * 180 / PI + teleInfomation.telemetryInfo.angleofRudder;
//	curAngle = curAngle > 360 ? curAngle - 360 : curAngle;
//	curAngle = curAngle * PI / 180;
//
//	double x_Angle = (xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2) - xygsZC_Target.videoWidth / 2
//		 > 0 ? 90 : 270;
//	double y_Angle = (xygsZC_Target.targetRectY + xygsZC_Target.targetRectHeight / 2) - xygsZC_Target.videoHeight / 2
//		> 0 ? 180 : 0;
//	///posCenter
//	QGeoCoordinate pos_X = posCenter.atDistanceAndAzimuth(dx_, teleInfomation.telemetryInfo.angleofRudder + 90);
//	QGeoCoordinate pos_Y = posCenter.atDistanceAndAzimuth(dy_, teleInfomation.telemetryInfo.angleofRudder);
//	//QGeoCoordinate posTarget = posCenter.atDistanceAndAzimuth(picDis, curAngle/*(TarAngle1 + teleInfomation.telemetryInfo.angleofRudder )*/);
//	QGeoCoordinate posTarget = QGeoCoordinate(pos_Y.latitude(), pos_X.longitude());
//	double nDis = sqrt(dx_*dx_ + dy_*dy_);
//	QGeoCoordinate halfPos((pos_X.latitude() + pos_Y.latitude()) / 2, (pos_X.longitude() + pos_Y.longitude()) / 2);
//	double nAngle = posCenter.azimuthTo(halfPos);
//	posTarget = posCenter.atDistanceAndAzimuth(nDis, nAngle);
//
//	//QGeoCoordinate posTarget = QGeoCoordinate(pos_X.latitude(), pos_Y.longitude());
//	tempPos = posTarget;
//	return tempPos;
//}



void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::StartUAVsTimers()
{
    for (auto &mapitr : m_UAVsTimerMap)
    {
        auto &mpair = mapitr.second;
        if (mpair.first == false)
        {
            if (mpair.second.size() > 1)
            {
                connect(*mpair.second.begin(), &QTimer::timeout, this, &PlayYoloVideoStreamManageWidget::GimbalMoveAndCalaDis2);
                //connect(*(mpair.second.begin() + 1), &QTimer::timeout, this, &PlayYoloVideoStreamManageWidget::LaserMeasureDistance);
				connect(*(mpair.second.begin() + 1), &QTimer::timeout, this, &PlayYoloVideoStreamManageWidget::calcMeasureDistance);
				(*mpair.second.begin())->start();
                (*(mpair.second.begin() + 1))->start();
            }
        }
    }
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::StopUAVsTimers()
{
	for (auto &mapitr : m_UAVsTimerMap)
	{
		auto &mpair = mapitr.second;
		if (mpair.first == false)
		{
			if (mpair.second.size() > 1)
			{
				//disconnect(*mpair.second.begin(), &QTimer::timeout, this, &PlayYoloVideoStreamManageWidget::GimbalMoveAndCalaDis2);
				//connect(*(mpair.second.begin() + 1), &QTimer::timeout, this, &PlayYoloVideoStreamManageWidget::LaserMeasureDistance);
				//disconnect(*(mpair.second.begin() + 1), &QTimer::timeout, this, &PlayYoloVideoStreamManageWidget::calcMeasureDistance);
				(*mpair.second.begin())->stop();
				(*(mpair.second.begin() + 1))->stop();
			}
		}
	}
	//m_UAVsTimerMap.clear();
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::SendAMapPodControlMsg(std::string ComponentID, AMapPod54Control &aMapPod54Control)
{
    const std::string currentComponentID = ComponentID;
    std::string currentSensorID;
    qnzkna::SensorManage::SensorInfoMap tmpSensorInfoOptoelectronicMap;
    qnzkna::SensorManage::SensorInfoMap tmpSensorInfoMap;
    bool bGetSensor = IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()->GetSensorInfosInWeaponComponent(currentComponentID, &tmpSensorInfoMap);
    if (bGetSensor)
    {
        qnzkna::SensorManage::SensorInfoMap::iterator itrSensor = tmpSensorInfoMap.begin();
        for (; itrSensor != tmpSensorInfoMap.end(); itrSensor++)
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

    if (!tmpSensorInfoOptoelectronicMap.empty())
    {
        currentSensorID = tmpSensorInfoOptoelectronicMap.begin()->first;
    }
    for (const auto &tmpSensorInfoOptoelectronic : tmpSensorInfoOptoelectronicMap)
    {
        m_AMapPod54ControlStatusInfoMap.insert(QString::fromStdString(tmpSensorInfoOptoelectronic.second.GetSensorID()), AMapPod54ControlStatusInfo());
    }

    qnzkna::framework::MessageReferenceProps  props;
    props.put("ComponentID", ComponentID);
    props.put("SensorID", currentSensorID);
    IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgSingleLaserCalaDis", (const char *)(&aMapPod54Control), sizeof(aMapPod54Control), props));
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::SendAMapPodControlZeroMsg(std::string ComponentID)
{

}

std::pair<float, float> qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::getVehicleDeflectionDistYawPitch54Value(const std::string &strVehicleID)
{
    float                                                            m_DeflectionAngleofNose;
    float                                                            m_DeflectionAngleofPitch;
    return std::make_pair(m_DeflectionAngleofNose, m_DeflectionAngleofPitch);
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::getVehicleDeflectionDistYawPitch54ZoomDistanceValue(const std::string &strVehicleID, const QString &strSensorID, float &deflectionAngleofNose, float &deflectionAngleofPitch)
{
    return;
}

//void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::onFocalZoomChanged(int mFocal)
//{
//	currFocal = mFocal;
//}

#endif

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::on_pushButton_PlayYoloVideoStreamManageWidget_Start_clicked()
{
    m_strFileName = m_VideoDevice.GetVideoDeviceURL();
    if (m_strFileName.empty())
    {
        return ;
    }

    QString hostAddress("local:");
    hostAddress += s_RemoteObjectPrefix;
    hostAddress += QString::fromStdString(m_VideoDevice.GetVideoComponentID());

    const QString strApplicationDirPath = QCoreApplication::applicationDirPath();
#ifdef QT_DEBUG
    const QString strExternalSoftware(strApplicationDirPath + "/MatShow.exe");
#else
    const QString strExternalSoftware(strApplicationDirPath + "/MatShow.exe");
#endif

    QString strExternalSoftwareYoloVideoStream("");
    QSettings settings("./Data/conf/VideoStream.ini", QSettings::IniFormat);
    settings.beginGroup("YoloVideoStream");
    if (settings.contains("Yolo_VideoStream"))
    {
        strExternalSoftwareYoloVideoStream = settings.value("Yolo_VideoStream", "").toString();

        QDir dirExternalSoftwareYoloVideoStream(strExternalSoftwareYoloVideoStream);
        if (!dirExternalSoftwareYoloVideoStream.isAbsolute())
        {
            dirExternalSoftwareYoloVideoStream = strApplicationDirPath + "/" + strExternalSoftwareYoloVideoStream;
        }

        if (QFileInfo::exists(dirExternalSoftwareYoloVideoStream.absolutePath()))
        {
            qDebug() << __LINE__ << dirExternalSoftwareYoloVideoStream << strExternalSoftwareYoloVideoStream;
            strExternalSoftwareYoloVideoStream = dirExternalSoftwareYoloVideoStream.absolutePath();
        }

        qDebug() << __LINE__ << QFileInfo::exists(dirExternalSoftwareYoloVideoStream.absolutePath()) << dirExternalSoftwareYoloVideoStream.absolutePath();

    }
    settings.endGroup();

    if (strExternalSoftwareYoloVideoStream.isEmpty())
    {
        strExternalSoftwareYoloVideoStream = strExternalSoftware;
    }

    QStringList arguments;
    arguments << hostAddress;
    arguments << QString::fromStdString(m_VideoDevice.GetVideoComponentID());
    arguments << QString::fromStdString(m_VideoDevice.GetVideoDeviceURL());
    arguments << QString::fromStdString(m_VideoDevice.GetVideoDeviceURLInfrared());
    arguments << QString::fromStdString(m_VideoDevice.GetVideoDeviceType());
	arguments << QString::number(QCoreApplication::applicationPid());
    LaunchExternalSoftware(strExternalSoftwareYoloVideoStream, arguments);

    qDebug() << strExternalSoftwareYoloVideoStream << arguments;

    emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().video_start();
    m_GimbalMoveTimer.start();
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::on_pushButton_PlayYoloVideoStreamManageWidget_Stop_clicked()
{
    if (m_ProcessExternalSoftware == nullptr)
    {
        return;
    }

    m_GimbalMoveTimer.stop();
    emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().video_stop();

    emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().video_exit();

    qDebug() << __FILE__ << __LINE__ << m_ProcessExternalSoftware;
    if (m_ProcessExternalSoftware == nullptr)
    {
        return ;
    }

    m_ProcessExternalSoftware->terminate();
    m_ProcessExternalSoftware->kill();

    m_ProcessExternalSoftware->waitForFinished(3000);

    connect(m_ProcessExternalSoftware, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << __FILE__ << __LINE__;
        if (m_ProcessExternalSoftware != nullptr)
        {
            delete m_ProcessExternalSoftware;
            m_ProcessExternalSoftware = nullptr;

            if (ui != nullptr
                    && ui->widget_PlayYoloVideoStreamManageWidget != nullptr
                    && ui->widget_PlayYoloVideoStreamManageWidget->layout() != nullptr
                    && ui->widget_PlayYoloVideoStreamManageWidget->layout()->count() == 1)
            {
                ui->widget_PlayYoloVideoStreamManageWidget->layout()->takeAt(0);
            }
        }

    });

    connect(m_ProcessExternalSoftware, &QProcess::errorOccurred,
            [this]()
    {
        qDebug() << __FILE__ << __LINE__;

    });

    qDebug() << "exitStatus:" << m_ProcessExternalSoftware->exitStatus()
             << ",error:" <<  m_ProcessExternalSoftware->error()
             << ",state:" <<  m_ProcessExternalSoftware->state();
    if ((m_ProcessExternalSoftware->exitStatus() == QProcess::ExitStatus::NormalExit || m_ProcessExternalSoftware->exitStatus() == QProcess::ExitStatus::CrashExit)
            || (m_ProcessExternalSoftware->error() == QProcess::ProcessError::FailedToStart || m_ProcessExternalSoftware->error() == QProcess::ProcessError::Crashed)
            || (m_ProcessExternalSoftware->state() == QProcess::ProcessState::NotRunning)
       )
    {
        if (m_ProcessExternalSoftware != nullptr)
        {
            delete m_ProcessExternalSoftware;
            m_ProcessExternalSoftware = nullptr;

            if (ui != nullptr
                    && ui->widget_PlayYoloVideoStreamManageWidget != nullptr
                    && ui->widget_PlayYoloVideoStreamManageWidget->layout() != nullptr
                    && ui->widget_PlayYoloVideoStreamManageWidget->layout()->count() == 1)
            {
                ui->widget_PlayYoloVideoStreamManageWidget->layout()->takeAt(0);
            }
        }
    }
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_1_clicked()
{
	const QString strApplicationDirPath = QCoreApplication::applicationDirPath();
	QSettings settings(strApplicationDirPath + "/Yolo/YoloConfig.ini", QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("YoloConfig");
	QString cfg_file = YoloConfigTypeMap[1] + ".cfg";
	QString weights_file = YoloConfigTypeMap[1] + ".weights";
	QString name_file = YoloConfigTypeMap[1] + ".names";
	settings.setValue("YoloConfig_Cfg", cfg_file);
	settings.setValue("YoloConfig_Weights", weights_file);
	settings.setValue("YoloConfig_Names", name_file);
	settings.endGroup();
    //emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().targetTypeChanged(ESystemTargetType::ESystemTargetType_Person);
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_2_clicked()
{
	const QString strApplicationDirPath = QCoreApplication::applicationDirPath();
	QSettings settings(strApplicationDirPath + "/Yolo/YoloConfig.ini", QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("YoloConfig");
	QString cfg_file = YoloConfigTypeMap[2] + ".cfg";
	QString weights_file = YoloConfigTypeMap[2] + ".weights";
	QString name_file = YoloConfigTypeMap[2] + ".names";
	settings.setValue("YoloConfig_Cfg", cfg_file);
	settings.setValue("YoloConfig_Weights", weights_file);
	settings.setValue("YoloConfig_Names", name_file);
	settings.endGroup();
   // emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().targetTypeChanged(ESystemTargetType::ESystemTargetType_UAV);
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_3_clicked()
{
	const QString strApplicationDirPath = QCoreApplication::applicationDirPath();
	QSettings settings(strApplicationDirPath + "/Yolo/YoloConfig.ini", QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("YoloConfig");
	QString cfg_file = YoloConfigTypeMap[3] + ".cfg";
	QString weights_file = YoloConfigTypeMap[3] + ".weights";
	QString name_file = YoloConfigTypeMap[3] + ".names";
	settings.setValue("YoloConfig_Cfg", cfg_file);
	settings.setValue("YoloConfig_Weights", weights_file);
	settings.setValue("YoloConfig_Names", name_file);
	settings.endGroup();
   // emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().targetTypeChanged(ESystemTargetType::ESystemTargetType_Vehicle);
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::on_radioButton_PlayYoloVideoStreamManageWidget_TargetType_4_clicked()
{
	const QString strApplicationDirPath = QCoreApplication::applicationDirPath();
	QSettings settings(strApplicationDirPath + "/Yolo/YoloConfig.ini", QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("YoloConfig");
	QString cfg_file = YoloConfigTypeMap[4] + ".cfg";
	QString weights_file = YoloConfigTypeMap[4] + ".weights";
	QString name_file = YoloConfigTypeMap[4] + ".names";
	settings.setValue("YoloConfig_Cfg", cfg_file);
	settings.setValue("YoloConfig_Weights", weights_file);
	settings.setValue("YoloConfig_Names", name_file);
	settings.endGroup();
    //emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().targetTypeChanged(ESystemTargetType::ESystemTargetType_Building);
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::on_checkBox_PlayYoloVideoStreamManageWidget_Yolo_stateChanged(int arg1)
{
    if (ui && ui->checkBox_PlayYoloVideoStreamManageWidget_Yolo)
    {
        emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().yoloTypeChanged(ui->checkBox_PlayYoloVideoStreamManageWidget_Yolo->isChecked());
    }
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::on_checkBox_Track_stateChanged()
{
	int state = ui->checkBox_track->checkState();
	std::string strComponent = m_VideoDevice.GetVideoComponentID();
	if (state > 0)
	{

		//切悬停
		//触发指点飞行
		std::vector<std::string> sensorVec;
		std::string WeaponComponentName = strComponent;
		IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()
			->GetSensorIdsInWeaponComponent(WeaponComponentName, &sensorVec);

		char pBuf[MAX_BUFFER_LEN] = { 0 };
		int nBufLen = 0;
		qnzkna::framework::MessageReferenceProps props;
		props.put("UAVName", WeaponComponentName);
		//IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgSelectTargetFly_StopTask", pBuf, nBufLen, props));
		
		//启动跟踪
		uavTrackMap[strComponent] = true;
		m_uavTrackTimer->start();
	}
	else
	{
		//取消跟踪
		uavTrackMap[strComponent] = false;
		m_uavTrackTimer->stop();
	}
}

bool PlayYoloVideoStreamManageWidget::LaunchExternalSoftware(const QString &exePath, const QStringList &arguments)
{
    if (m_ProcessExternalSoftware != nullptr)
    {
        LyMessageBox::information(nullptr, tr("Tips"), tr("Software started"));
        return false;
    }

    QWidget *pWnd = GetPlayYoloVideoStreamManageWidgetParentWidget();
    if (pWnd == nullptr)
    {
        return false;
    }

    m_ProcessExternalSoftware = new QProcess(pWnd);
    if (m_ProcessExternalSoftware == nullptr)
    {
        return false;
    }

    m_ProcessExternalSoftwareTimes = 0;
    m_ProcessExternalSoftwareTimer.stop();
    connect(m_ProcessExternalSoftware, &QProcess::started, this, [this]()
    {
        ExternalSoftwareProcessStarted();
    }
           );
    m_ProcessExternalSoftware->setParent(this);
    m_ProcessExternalSoftware->start(exePath, arguments);

    qDebug() << __FILE__ << __LINE__  << m_ProcessExternalSoftware->errorString();
    return true;
}

void PlayYoloVideoStreamManageWidget::ExternalSoftwareProcessStarted()
{
    m_ProcessExternalSoftwareTimer.stop();
    qDebug() << "ExternalSoftwareProcessStarted : m_ProcessExternalSoftwareTimes: " << m_ProcessExternalSoftwareTimes << " ,Status: " << (m_ProcessExternalSoftware == nullptr);
    m_ProcessExternalSoftwareTimes++;
    if (m_ProcessExternalSoftwareTimes > 30)
    {
        m_ProcessExternalSoftwareTimes = 1e7;
        m_ProcessExternalSoftwareTimer.stop();
        return ;
    }

    QWidget *pWnd = GetPlayYoloVideoStreamManageWidgetParentWidget();
    if (pWnd == nullptr)
    {
        return ;
    }

    if (m_ProcessExternalSoftware == nullptr)
    {
        m_ProcessExternalSoftwareTimes = 1e7;
        m_ProcessExternalSoftwareTimer.stop();
        LyMessageBox::information(nullptr, tr("Tips"), tr("Software didn't start"));
        return;
    }

    qint64 id = m_ProcessExternalSoftware->processId();
    if (id == 0)
    {
        m_ProcessExternalSoftwareTimes = 1e7;
        m_ProcessExternalSoftwareTimer.stop();
        LyMessageBox::information(nullptr, tr("Tips"), tr("Software didn't start"));
        return;
    }

#ifdef WIN32
    QString hostAddress("local:");
    hostAddress += s_RemoteObjectPrefix;
    hostAddress += QString::fromStdString(m_VideoDevice.GetVideoComponentID());
    hostAddress = QString::fromStdString(m_VideoDevice.GetVideoComponentID()) + hostAddress;
    HWND mainwindowHwnd1 = FindWindow(nullptr, hostAddress.toStdWString().c_str());

    qDebug() << "Status: " << m_ProcessExternalSoftware->state();
    HWND mainwindowHwnd = FindMainWindow(id);

    qDebug() << "mainwindowHwnd: " << mainwindowHwnd;
    if (!mainwindowHwnd)
    {
        m_ProcessExternalSoftwareTimer.start();
        return;
    }

    if (mainwindowHwnd != mainwindowHwnd1)
    {
        m_ProcessExternalSoftwareTimer.start();
        return;
    }

#ifdef UNICODE
    WCHAR winText1[1024] = {0};
    WCHAR winText[1024] = {0};
#else
    CHAR winText1[1024] = {0};
    CHAR winText[1024] = {0};
#endif
    GetWindowText(mainwindowHwnd1, winText1, 1024);
    GetWindowText(mainwindowHwnd, winText, 1024);

#ifdef UNICODE
    const QString &strWinText1 = QString::fromWCharArray(winText1, 1024);
    const QString &strWinText = QString::fromWCharArray(winText, 1024);
#else
    const QString &strWinText1 = QString::fromUtf8(winText1, 1024);
    const QString &strWinText = QString::fromUtf8(winText, 1024);
#endif
    if (strWinText1 != strWinText)
    {
        m_ProcessExternalSoftwareTimer.start();
        m_GimbalMoveTimer.start();
        return;
    }

    m_ProcessExternalSoftwareTimes = 1e7;
    m_ProcessExternalSoftwareTimer.stop();

    ShowWindow(mainwindowHwnd, SW_NORMAL);

    WId wid = (WId)mainwindowHwnd;
    QWindow *window = QWindow::fromWinId(wid);
    if (window == nullptr)
    {
        return ;
    }
    window->setFlags(Qt::FramelessWindowHint);

    ShowWindow(mainwindowHwnd, SW_NORMAL);

    window->show();
    window->resize(50, 50);

    QWidget *widget = QWidget::createWindowContainer(window, pWnd);
    if (widget == nullptr)
    {
        return ;
    }
    widget->clearFocus();

    widget->resize(500, 500);

    widget->show();

    if (ui == nullptr
            || ui->widget_PlayYoloVideoStreamManageWidget == nullptr
            || ui->widget_PlayYoloVideoStreamManageWidget->layout() == nullptr)
    {
        return ;
    }
    ui->widget_PlayYoloVideoStreamManageWidget->layout()->addWidget(widget);
#endif

    emit m_YoloVideoStreamManageRepc.getYoloVideoStreamManage().video_start();
}

bool PlayYoloVideoStreamManageWidget::StartVideoManageObjectHost(const CVideoDevice &videoDevice)
{
    QString hostAddress("local:");
    hostAddress += s_RemoteObjectPrefix;
    hostAddress += QString::fromStdString(videoDevice.GetVideoComponentID());

	if ((!m_YoloVideoStreamManageRepc.GetCreateHost()) && (!hostAddress.isEmpty()))
	{
		m_YoloVideoStreamManageRepc.CreateHost(hostAddress);

		static std::map<QString, int>recordTargetIndexMap;

		connect(&m_YoloVideoStreamManageRepc.getYoloVideoStreamManage(), &YoloVideoStreamManageCommonInterface::video_selectrect_recv,
			this, [this](const QByteArray videostreamtargetinfo)
		{

			const int videostreamtargetinfoSize = videostreamtargetinfo.size();
			if (videostreamtargetinfoSize <= 0)
			{
				m_detectTargetMap.clear();
				return;
			}

			const std::string strSensorId = m_VideoDevice.GetVideoComponentID();
			if (strSensorId.empty())
			{
				return;
			}

			constexpr int VideoStreamTargetInfoSize = sizeof(qnzkna::PlayVideoManage::VideoStreamTargetInfo);

			if (videostreamtargetinfoSize % VideoStreamTargetInfoSize != 0)
			{
				return;
			}

			const int selectrectSizeRecv = (videostreamtargetinfoSize / VideoStreamTargetInfoSize);

			const qnzkna::PlayVideoManage::VideoStreamTargetInfo *const videoStreamTargetInfo = (const qnzkna::PlayVideoManage::VideoStreamTargetInfo * const)(videostreamtargetinfo.data());
			//m_detectTargetMap.clear();
			//trackFlyVec.clear();
			countTimes = 0;
			std::map<int, QVector<XygsZC_Mark_01>>		XygsZc_TargetMap;
			auto &TargetMap = LY_UAVNavigationArmShotAlone54::Get().m_XygsZc_TargetMap;
			if (selectrectSizeRecv == 1)
			{
				if (
					(videoStreamTargetInfo + 0)->targetId == 0 &&
					(videoStreamTargetInfo + 0)->videoWidth == 0 &&
					(videoStreamTargetInfo + 0)->videoHeight == 0)
				{
					XygsZC_Mark_01 tempXygsZC_Target;
					m_Q20AxSendLast = 0.0;
					m_Q20AySendLast = 0.0;
					qnzkna::framework::MessageReferenceProps props;
					props.put("WeaponID", QString::number(m_Q20AWeaponID).toStdString());
					props.put("DisX", QString::number(0.0).toStdString());
					props.put("DisY", QString::number(0.0).toStdString());

					IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
						new qnzkna::framework::MessageReference("MsgQ20AAimTrace", (char *)&tempXygsZC_Target, sizeof(tempXygsZC_Target), props));

					qnzkna::framework::MessageReferenceProps propsSendTrack;
					propsSendTrack.put("WeaponID", QString::number(m_Q20AWeaponID).toStdString());
					propsSendTrack.put("DisObject", QString::number(0).toStdString());
					propsSendTrack.put("DisX", QString::number(0).toStdString());
					propsSendTrack.put("ExpenctedDis", QString::number(5).toStdString());
					IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
						new qnzkna::framework::MessageReference("MsgQ20ATrackingObject", (char *)&tempXygsZC_Target, sizeof(tempXygsZC_Target), propsSendTrack));

					trackFlyVec.clear();
					qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
					controlInfo.componentID = (videoStreamTargetInfo + 0)->componentId;
					controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_StopTracking;
					controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;
					SendGimbalDYZCControl((QString::number((videoStreamTargetInfo + 0)->componentId)).toStdString(), controlInfo);
					TargetMap.clear();
					targetSetVec.clear();
					//LY_UAVNavigationArmShotAlone54::Get().GetUAVTargetTree()->ClearTargetList();
					//emit LY_UAVNavigationArmShotAlone54::Get().GetUAVTargetTree()->SignalsClearTree();
					LY_UAVNavigationArmShotAlone54::Get().m_XygsZc_TargetMap.clear();
					SetPointTargetIdMap.clear();
					StopUAVsTimers();
					m_detectTargetMap.clear();
					//auto pSensor = IUAVNavigationArmShotAlone54Activator::Get().getPISensorVedioService();
					//if (nullptr != pSensor)
					//	pSensor->SendClearComponentTargets(controlInfo.componentID);
					return;
				}
			}

			std::list<long long> targetIds;
			targetIds.clear();
			for (int i = 0; i < selectrectSizeRecv; i++)
			{
				XygsZC_Mark_01 xygsZC_Target;
				xygsZC_Target.target_lon = 0;
				xygsZC_Target.target_lat = 0;
				xygsZC_Target.target_alt = 0;
				xygsZC_Target.sensorID = 0;
				xygsZC_Target.componentID = (videoStreamTargetInfo + i)->componentId;
				xygsZC_Target.targetID = (videoStreamTargetInfo + i)->targetId;
				xygsZC_Target.targetTime = (videoStreamTargetInfo + i)->targetTime;
				xygsZC_Target.discernType = (videoStreamTargetInfo + i)->discernType;
				xygsZC_Target.targetType = (videoStreamTargetInfo + i)->targetType;
				xygsZC_Target.videoWidth = (videoStreamTargetInfo + i)->videoWidth;
				xygsZC_Target.videoHeight = (videoStreamTargetInfo + i)->videoHeight;
				xygsZC_Target.targetRectX = (videoStreamTargetInfo + i)->targetRectX;
				xygsZC_Target.targetRectY = (videoStreamTargetInfo + i)->targetRectY;
				xygsZC_Target.targetRectWidth = (videoStreamTargetInfo + i)->targetRectWidth;
				xygsZC_Target.targetRectHeight = (videoStreamTargetInfo + i)->targetRectHeight;
				QVector<XygsZC_Mark_01> TargetVec = XygsZc_TargetMap[xygsZC_Target.componentID];
				//////////////////////////////////////////////////////////////////////////
				xygsZC_Target.Z = (videoStreamTargetInfo + i)->componentId;
				xygsZC_Target.P = (videoStreamTargetInfo + i)->targetId;

				//auto pSensor = IUAVNavigationArmShotAlone54Activator::Get().getPISensorVedioService();
				//if (nullptr != pSensor)
				//	pSensor->SendUpdateComponentTargetData(xygsZC_Target.componentID, xygsZC_Target);
				targetIds.emplace_back(xygsZC_Target.targetID);

				int selectState = (videoStreamTargetInfo + i)->targetRectSelected;
				if (selectState == 1)
				{
					trackFlyVec.push_back(xygsZC_Target);
					break;
				}
				bool bExist = true;
				for (auto info : TargetVec)
				{
					if (info.targetID == xygsZC_Target.targetID)
					{
						bExist = false;
					}
				}
				if (bExist)
				{
					XygsZc_TargetMap[xygsZC_Target.componentID].push_back(xygsZC_Target);
				}
				SendTargetZCVideoTargetNoPosition(strSensorId, xygsZC_Target);

				//////////////////////////////////////////////////////////////////////////
				if (m_laserTargetMap.find(xygsZC_Target.componentID) == m_laserTargetMap.end())
				{
					std::vector<XygsZC_Mark_01> tempvec;
					m_laserTargetMap[xygsZC_Target.componentID] = tempvec;
				}
				//Auto
				if (xygsZC_Target.discernType == 0)
				{
					bool isfind = false;
					for (auto &miter : m_detectTargetMap)
					{
						if (miter.second.targetID == xygsZC_Target.targetID && miter.second.componentID == xygsZC_Target.componentID)
						{
							miter.second = xygsZC_Target;
							isfind = true;
							break;
						}
					}
					if (isfind == false && xygsZC_Target.targetID - 1 < m_strDiscernUAVIDList.size())
					{
						AgriSoilTestingFertilizer::SaltFieldEventInfo saltFeildInfo;
						if (xygsZC_Target.targetType == 1)
						{
							saltFeildInfo.eventType = 1;
						}
						else if (xygsZC_Target.targetType == 2)
						{
							saltFeildInfo.eventType = 2;
						}
						else if (xygsZC_Target.targetType == 3)
						{
							saltFeildInfo.eventType = 3;
						}
						else
						{
							saltFeildInfo.eventType = 1;
						}

						QString temp1 = QString::number(xygsZC_Target.componentID) + QString::number(xygsZC_Target.targetID);
						m_detectTargetMap[temp1] = xygsZC_Target;

						saltFeildInfo.id = 0;
						saltFeildInfo.name = temp1;
						saltFeildInfo.reporterID = QString::number(xygsZC_Target.componentID);
						//QString dtTime = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");
						saltFeildInfo.findTime = QDateTime::currentDateTime();
						QGeoCoordinate pos = CalcTargetPosFromPod1(QString::number(xygsZC_Target.componentID).toStdString(),
							xygsZC_Target);
						saltFeildInfo.lon = pos.longitude();
						saltFeildInfo.lat = pos.latitude();
						if (std::abs(pos.longitude()) < 1 || std::abs(pos.latitude()) < 1)
						{
							saltFeildInfo.lon = 91.38;
							saltFeildInfo.lat = 38.568;
						}

						QByteArray picArray;
						IUAVNavigationArmShotAlone54Activator::Get().getPISensorVedioService()->GetLastPixMapData(xygsZC_Target.componentID, picArray);
						saltFeildInfo.picData = picArray;
						int nsize = sizeof(saltFeildInfo);
						char pBuf[2000] = { 0 };
						int nBufLen = sizeof(saltFeildInfo);
						memcpy(pBuf, &saltFeildInfo, sizeof(saltFeildInfo));

						qnzkna::framework::MessageReferenceProps  props;
						props.put("SourceType", std::to_string(saltFeildInfo.eventType));
						props.put("SourceID", std::to_string(xygsZC_Target.componentID));
						//IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgSaltFieldEventInfo", pBuf, nBufLen, props));

						IFunctionInterface &pFunc = LY_UAVNavigationArmShotAlone54Interface::Get().GetFunctionInterface();
						IFunctionInterface &ifi = LY_UAVNavigationArmShotAlone54Interface::Get().GetFunctionInterface();
						QVariant vt = ifi.Invoke("Anzkna.AgriSoilAnalysisPredict.AddEvent", QVariant::fromValue(saltFeildInfo));
						if (vt.canConvert<AgriSoilTestingFertilizer::SaltFieldEventInfo>())
						{
							const AgriSoilTestingFertilizer::SaltFieldEventInfo &d = vt.value<AgriSoilTestingFertilizer::SaltFieldEventInfo>();
						}
						QString temp = QString::number(xygsZC_Target.componentID) + QString::number(xygsZC_Target.targetID);
						m_detectTargetMap[temp] = xygsZC_Target;
					}
					else//hand
					{
						bool isfind = false;
						for (auto &miter : m_detectTargetMap)
						{
							if (miter.second.targetID == xygsZC_Target.targetID && miter.second.componentID == xygsZC_Target.componentID)
							{
								miter.second = xygsZC_Target;
								isfind = true;
								break;
							}
						}
						if (isfind == false && xygsZC_Target.targetID - 1 < m_strDiscernUAVIDList.size())
						{
							AgriSoilTestingFertilizer::SaltFieldEventInfo saltFeildInfo;
							if (xygsZC_Target.targetType == 1)
							{
								saltFeildInfo.eventType = 1;
							}
							else if (xygsZC_Target.targetType == 2)
							{
								saltFeildInfo.eventType = 2;
							}
							else if (xygsZC_Target.targetType == 3)
							{
								saltFeildInfo.eventType = 3;
							}
							else
							{
								saltFeildInfo.eventType = 1;
							}

							QString temp1 = QString::number(xygsZC_Target.componentID) + QString::number(xygsZC_Target.targetID);
							m_detectTargetMap[temp1] = xygsZC_Target;

							saltFeildInfo.id = 0;
							saltFeildInfo.name = temp1;
							saltFeildInfo.reporterID = QString::number(xygsZC_Target.componentID);
							//QString dtTime = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");
							saltFeildInfo.findTime = QDateTime::currentDateTime();
							QGeoCoordinate pos = CalcTargetPosFromPod1(QString::number(xygsZC_Target.componentID).toStdString(),
								xygsZC_Target);
							saltFeildInfo.lon = pos.longitude();
							saltFeildInfo.lat = pos.latitude();
							if (std::abs(pos.longitude()) < 1 || std::abs(pos.latitude()) < 1)
							{
								saltFeildInfo.lon = 91.38;
								saltFeildInfo.lat = 38.568;
							}

							QByteArray picArray;
							IUAVNavigationArmShotAlone54Activator::Get().getPISensorVedioService()->GetLastPixMapData(xygsZC_Target.componentID, picArray);
							saltFeildInfo.picData = picArray;
							int nsize = sizeof(saltFeildInfo);
							char pBuf[2000] = { 0 };
							int nBufLen = sizeof(saltFeildInfo);
							memcpy(pBuf, &saltFeildInfo, sizeof(saltFeildInfo));

							qnzkna::framework::MessageReferenceProps  props;
							props.put("SourceType", std::to_string(saltFeildInfo.eventType));
							props.put("SourceID", std::to_string(xygsZC_Target.componentID));
							//IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgSaltFieldEventInfo", pBuf, nBufLen, props));

							IFunctionInterface &pFunc = LY_UAVNavigationArmShotAlone54Interface::Get().GetFunctionInterface();
							IFunctionInterface &ifi = LY_UAVNavigationArmShotAlone54Interface::Get().GetFunctionInterface();
							QVariant vt = ifi.Invoke("Anzkna.AgriSoilAnalysisPredict.AddEvent", QVariant::fromValue(saltFeildInfo));
							if (vt.canConvert<AgriSoilTestingFertilizer::SaltFieldEventInfo>())
							{
								const AgriSoilTestingFertilizer::SaltFieldEventInfo &d = vt.value<AgriSoilTestingFertilizer::SaltFieldEventInfo>();
							}



							//auto ret = pFunc.Invoke("my.test.funB", 0);


							//if (xygsZC_Target.targetType == 4 || xygsZC_Target.targetType == 3)//calc
							//{
							//	QString temp = QString::number(xygsZC_Target.componentID) + QString::number(xygsZC_Target.targetID);
							//	m_detectTargetMap[temp] = xygsZC_Target;
							//	qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t  teleInfomation;
							//	std::string weaponID = QString::number(xygsZC_Target.componentID).toStdString();
							//	IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService()->GetDefaultUAVRealTimeState(weaponID, teleInfomation);
							//	double uav_Lon = teleInfomation.telemetryInfo.position.lng;
							//	double uav_lat = teleInfomation.telemetryInfo.position.lat;
							//	double uav_alt = teleInfomation.telemetryInfo.relativeOriginHeigth;
							//	double heading = teleInfomation.telemetryInfo.angleofNose;
							//	QGeoCoordinate uavPos(uav_lat, uav_Lon);
							//	double dis = uav_alt / std::tan(18.45*3.141592653589 / 180);
							//	QGeoCoordinate targetPos = uavPos.atDistanceAndAzimuth(dis, heading);

							//	QGeoCoordinate pos = CalcTargetPosFromPod1(QString::number(xygsZC_Target.componentID).toStdString(),
							//		xygsZC_Target);
							//	xygsZC_Target.target_lon = pos.longitude();
							//	xygsZC_Target.target_lat = pos.latitude();

							//	xygsZC_Target.target_alt = 0;
							//	xygsZC_Target.Z = 0;
							//	xygsZC_Target.P = 0;

							//	//IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->
							//	//	SetCurrUAVRectID(xygsZC_Target.componentID, xygsZC_Target);

							//	//if (staticTargetSet.find(xygsZC_Target.targetType) != staticTargetSet.end())
							//	//{
							//	//	if (std::find(targetSetVec.begin(), targetSetVec.end(), xygsZC_Target.targetID) == targetSetVec.end())
							//	//	{
							//	//		QString dtTime = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");
							//	//		QString tar_id = dtTime + temp;
							//	//		IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->
							//	//			SetPicTargetID(xygsZC_Target.componentID, tar_id);
							//	//		targetSetVec.push_back(xygsZC_Target.targetID);

							//	//		std::vector<std::string> sensorVec;
							//	//		std::string WeaponComponentName = QString::number(xygsZC_Target.componentID).toStdString();
							//	//		IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()
							//	//			->GetSensorIdsInWeaponComponent(WeaponComponentName, &sensorVec);
							//	//		QString strSensorID = "1000307";
							//	//		if (!sensorVec.empty())
							//	//		{
							//	//			strSensorID = QString::fromStdString(sensorVec[0]);
							//	//		}

							//	//		qnzkna::framework::MessageReferenceProps props;
							//	//		props.put("TargetType", std::to_string(NODE_TYPE_SENSOR));
							//	//		props.put("TargetID", strSensorID.toStdString());
							//	//		props.put("WeaponID", QString::number(xygsZC_Target.componentID).toStdString());

							//	//		IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
							//	//			new qnzkna::framework::MessageReference("MsgZCTargetPosition", (char *)&xygsZC_Target, sizeof(xygsZC_Target), props));

							//	//	}
							//	//}

							//}
							//else if (xygsZC_Target.targetType == 5)
							//{
							//	//只显示框，不产生目标
							//}
							//else if (xygsZC_Target.targetType == -1)//laser
							//{
							//	//先连接吊舱

							//	QString temp = QString::number(xygsZC_Target.componentID) + QString::number(xygsZC_Target.targetID);
							//	m_detectTargetMap[temp] = xygsZC_Target;
							//	//laser
							//	//move center
							//	qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
							//	controlInfo.componentID = xygsZC_Target.componentID;
							//	controlInfo.oper = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_MoveCenter;
							//	controlInfo.operData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;

							//	controlInfo.x = xygsZC_Target.targetRectX;
							//	controlInfo.y = xygsZC_Target.targetRectY;

							//	controlInfo.width = this->width();
							//	controlInfo.heigth = this->height();

							//	std::string strComponent = QString::number(xygsZC_Target.componentID).toStdString();
							//	SendGimbalMoveCenter(strComponent, controlInfo);
							//	timerLaser->start();
							//}
							//else if (xygsZC_Target.targetType == 2)
							//{
							//	//指点飞行
							//	QString temp = QString::number(xygsZC_Target.componentID) + QString::number(xygsZC_Target.targetID);
							//	//m_detectTargetMap[temp] = xygsZC_Target;
							//	auto mtr = SetPointTargetIdMap.find(temp);
							//	if (mtr != SetPointTargetIdMap.end())
							//	{
							//		continue;
							//	}
							//	
							//	qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t  teleInfomation;
							//	std::string weaponID = QString::number(xygsZC_Target.componentID).toStdString();
							//	IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService()->GetDefaultUAVRealTimeState(weaponID, teleInfomation);
							//	double uav_Lon = teleInfomation.telemetryInfo.position.lng;
							//	double uav_lat = teleInfomation.telemetryInfo.position.lat;
							//	double uav_alt = teleInfomation.telemetryInfo.relativeOriginHeigth;
							//	double heading = teleInfomation.telemetryInfo.angleofNose;
							//	QGeoCoordinate uavPos(uav_lat, uav_Lon);
							//	double dis = uav_alt / std::tan(18.45*3.141592653589 / 180);
							//	QGeoCoordinate targetPos = uavPos.atDistanceAndAzimuth(dis, heading);
							//	xygsZC_Target.target_lon = targetPos.longitude();
							//	xygsZC_Target.target_lat = targetPos.latitude();
							//	xygsZC_Target.target_alt = 0;

							//	//IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->
							//	//	SetCurrUAVRectID(xygsZC_Target.componentID, xygsZC_Target);

							//	if (staticTargetSet.find(xygsZC_Target.targetType) != staticTargetSet.end())
							//	{
							//		if (std::find(targetSetVec.begin(), targetSetVec.end(), xygsZC_Target.targetID) == targetSetVec.end())
							//		{
							//			/*QString dtTime = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");
							//			QString tar_id = dtTime + temp;
							//			IUAVNavigationArmShotAlone54Activator::Get().getPITargetManageService()->
							//				SetPicTargetID(xygsZC_Target.componentID, tar_id);
							//			targetSetVec.push_back(xygsZC_Target.targetID);*/

							//			std::vector<std::string> sensorVec;
							//			std::string WeaponComponentName = QString::number(xygsZC_Target.componentID).toStdString();
							//			IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()
							//				->GetSensorIdsInWeaponComponent(WeaponComponentName, &sensorVec);
							//			QString strSensorID = "1000307";
							//			if (!sensorVec.empty())
							//			{
							//				strSensorID = QString::fromStdString(sensorVec[0]);
							//			}

							//			qnzkna::framework::MessageReferenceProps props;
							//			props.put("TargetType", std::to_string(NODE_TYPE_SENSOR));
							//			props.put("TargetID", strSensorID.toStdString());
							//			props.put("WeaponID", QString::number(xygsZC_Target.componentID).toStdString());
							//			props.put("MsgType", QString("true").toStdString());

							//			IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
							//				new qnzkna::framework::MessageReference("MsgZCTargetPosition", (char *)&xygsZC_Target, sizeof(xygsZC_Target), props));
							//			SetPointTargetIdMap[temp] = true;
							//		}

							//	}

							//}
							//else if (xygsZC_Target.targetType == 1)
							//{
							//	m_MtexUavTrack.lock();
							//	for (auto &tracku : trackFlyVec)
							//	{
							//		if (tracku.targetID == xygsZC_Target.targetID)
							//		{
							//			tracku = xygsZC_Target;
							//			break;
							//		}
							//	}
							//	m_MtexUavTrack.unlock();
							//	if (m_UAVsTimerMap.find(xygsZC_Target.componentID) == m_UAVsTimerMap.end())
							//	{
							//		QTimer *timer1 = new QTimer(this);
							//		timer1->setInterval(TimerStep1);
							//		QTimer *timer2 = new QTimer(this);
							//		timer2->setInterval(TimerStep01);
							//		std::vector<QTimer *> tempVec;
							//		tempVec.push_back(timer1);
							//		tempVec.push_back(timer2);
							//		std::pair<bool, std::vector<QTimer *>> tempPair = std::make_pair(false, tempVec);
							//		m_UAVsTimerMap[xygsZC_Target.componentID] = tempPair;
							//		StartUAVsTimers();
							//	}
							//}
							//else if (xygsZC_Target.targetType == 6)
							//{
							//	int videoCentX = (xygsZC_Target.videoWidth) / 2;
							//	int videoCentY = (xygsZC_Target.videoHeight) / 2;
							//	int dx, dy;
							//	dx = (videoCentX -( xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2)) * -1;
							//	dy = (videoCentY - (xygsZC_Target.targetRectY + xygsZC_Target.targetRectWidth / 2))/* * -1*/;
							//	float dxSend, dySend;
							//	if (abs(dx) > 30)
							//	{
							//		dxSend = (dx * 1.0) / videoCentX;
							//		m_Q20AxSendLast += (0.08 * dxSend);
							//	}
							//	if (abs(dy) > 15)
							//	{
							//		dySend = (dy * 1.0) / videoCentY;
							//		m_Q20AySendLast += (0.01 * dySend);
							//	}

							//	qDebug() << "Aim____videoCent: X: " << videoCentX << "/tY: " << videoCentY;
							//	qDebug() << "Aim____targetRectCent: X: " << xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2 <<
							//		"/tY: " << xygsZC_Target.targetRectY + xygsZC_Target.targetRectWidth / 2;
							//	qDebug() << "Aim____Dx: " << dx << "/tDy: " << dy << "/tdxD: " << dxSend << "/tDyD: " << dySend << "/tXSend: " <<
							//		m_Q20AxSendLast << "/tYSend: " << m_Q20AySendLast;

							//	if (std::abs(m_Q20AySendLast) > 1.0)
							//	{
							//		m_Q20AySendLast =0.9 * m_Q20AySendLast * std::abs(m_Q20AySendLast);
							//	}
							//	if (std::abs(m_Q20AxSendLast) > 1.0)
							//	{
							//		m_Q20AxSendLast = 0.9 * m_Q20AxSendLast * std::abs(m_Q20AxSendLast);
							//	}
							//	qnzkna::framework::MessageReferenceProps props;
							//	props.put("WeaponID", QString::number(xygsZC_Target.componentID).toStdString());
							//	props.put("DisX", QString::number(m_Q20AxSendLast).toStdString());
							//	props.put("DisY", QString::number(m_Q20AySendLast).toStdString());
							//	m_Q20AWeaponID = xygsZC_Target.componentID;
							//	IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
							//		new qnzkna::framework::MessageReference("MsgQ20AAimTrace", (char *)&xygsZC_Target, sizeof(xygsZC_Target), props));

							//}
							//else if (xygsZC_Target.targetType == 7)
							//{
							//	q20ATraTrackingObjectWalk(xygsZC_Target, 5, 0.5, 1.8);
							//}

							//0：动目标；1：跟踪；2：指点飞行；3和4：静态目标；5：不产生目标的框
							QString temp = QString::number(xygsZC_Target.componentID) + QString::number(xygsZC_Target.targetID);
							m_detectTargetMap[temp] = xygsZC_Target;
						}
					}
					//合批
					int nValue = CheckXygszcTargetMapChange(XygsZc_TargetMap);
					if (-1 != nValue)
					{
						//	if (!LY_UAVNavigationArmShotAlone54::Get().GetUAVTargetTree()->isVisible())
						//	{
						//		//if (QMessageBox::Yes == LyMessageBox::information(nullptr, tr2("提示"), tr2("识别到可疑目标，是否标记并启动无人机抵近侦察?"), QMessageBox::Yes | QMessageBox::No))
						//		if (LY_UAVNavigationArmShotAlone54::Get().GetUAVTargetTree()->on_MessageBoxWidgetShow())
						//		{
						//			//指点飞行
						//			QString temp = QString::number(xygsZC_Target.componentID) + QString::number(xygsZC_Target.targetID);
						//			//m_detectTargetMap[temp] = xygsZC_Target;
						//			auto mtr = SetPointTargetIdMap.find(temp);
						//			if (mtr != SetPointTargetIdMap.end())
						//			{
						//				continue;
						//			}

						//			qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t  teleInfomation;
						//			std::string weaponID = QString::number(xygsZC_Target.componentID).toStdString();
						//			IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService()->GetDefaultUAVRealTimeState(weaponID, teleInfomation);
						//			double uav_Lon = teleInfomation.telemetryInfo.position.lng;
						//			double uav_lat = teleInfomation.telemetryInfo.position.lat;
						//			double uav_alt = teleInfomation.telemetryInfo.relativeOriginHeigth;
						//			double heading = teleInfomation.telemetryInfo.angleofNose;
						//			QGeoCoordinate uavPos(uav_lat, uav_Lon);
						//			double dis = uav_alt / std::tan(18.45*3.141592653589 / 180);
						//			QGeoCoordinate targetPos = uavPos.atDistanceAndAzimuth(dis, heading);
						//			xygsZC_Target.target_lon = targetPos.longitude();
						//			xygsZC_Target.target_lat = targetPos.latitude();
						//			xygsZC_Target.target_alt = 0;

						//			//if (staticTargetSet.find(xygsZC_Target.targetType) != staticTargetSet.end())
						//			{
						//				if (std::find(targetSetVec.begin(), targetSetVec.end(), xygsZC_Target.targetID) == targetSetVec.end())
						//				{
						//					std::vector<std::string> sensorVec;
						//					std::string WeaponComponentName = QString::number(xygsZC_Target.componentID).toStdString();
						//					IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService()
						//						->GetSensorIdsInWeaponComponent(WeaponComponentName, &sensorVec);
						//					QString strSensorID = "1000307";
						//					if (!sensorVec.empty())
						//					{
						//						strSensorID = QString::fromStdString(sensorVec[0]);
						//					}
						//					targetSetVec.push_back(xygsZC_Target.targetID);
						//					//qnzkna::framework::MessageReferenceProps props;
						//					//props.put("TargetType", std::to_string(NODE_TYPE_SENSOR));
						//					//props.put("TargetID", strSensorID.toStdString());
						//					//props.put("WeaponID", QString::number(xygsZC_Target.componentID).toStdString());
						//					//props.put("MsgType", QString("true").toStdString());

						//					//IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
						//						//new qnzkna::framework::MessageReference("MsgZCTargetPosition", (char *)&xygsZC_Target, sizeof(xygsZC_Target), props));

						//					//弹出指点飞行界面
						//					qnzkna::framework::MessageReferenceProps props;
						//					props.put("dLon", QString::number(xygsZC_Target.target_lon, 'f', 6).toStdString());
						//					props.put("dLat", QString::number(xygsZC_Target.target_lat, 'f', 6).toStdString());

						//					if (xygsZC_Target.targetType != 2)//排除手框
						//					{
						//						IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
						//							new qnzkna::framework::MessageReference("MsgType_RevisePos", (char *)&xygsZC_Target, sizeof(xygsZC_Target), props));
						//					}
						//					SetPointTargetIdMap[temp] = true;
						//				}

						//			}
						//		}
						//		LY_UAVNavigationArmShotAlone54::Get().GetUAVTargetTree()->show();
						//		LY_UAVNavigationArmShotAlone54::Get().GetUAVTargetTree()->SetData(TargetMap, nValue);
						//	}
					}
				}
			}
			//auto pSensor = IUAVNavigationArmShotAlone54Activator::Get().getPISensorVedioService();
			//if (nullptr != pSensor)
			//	pSensor->SendUpdateComponentTargets(QString(strSensorId.c_str()).toLongLong(), targetIds);
		});
	}

    connect(&m_YoloVideoStreamManageRepc.getYoloVideoStreamManage(), &YoloVideoStreamManageCommonInterface::video_widow_show_recv,
            this, [this](const bool show)
    {
        qDebug() << __FILE__ << __LINE__ << "video_widow_show_recv " << show;
        ExternalSoftwareProcessStarted();
    });

    qDebug() << __FILE__ << __LINE__ << hostAddress;
    m_VideoDevice = videoDevice;
    return true;
}

int PlayYoloVideoStreamManageWidget::CheckXygszcTargetMapChange(std::map<int, QVector<XygsZC_Mark_01>> XygsZcTargetMap)
{
    int bChange = -1;
    auto &TargetMap = LY_UAVNavigationArmShotAlone54::Get().m_XygsZc_TargetMap;
    for (auto map : XygsZcTargetMap)
    {
        if (TargetMap.find(map.first) == TargetMap.end())
        {
            bChange = map.first;
            TargetMap[map.first] = map.second;
            break;
        }
        QVector<XygsZC_Mark_01> xygsZcMap = TargetMap[map.first];
        if (xygsZcMap.size() != map.second.size())
        {
            bChange = map.first;
            TargetMap[map.first] = map.second;
            break;
        }
        QVector<int> targetVec;
        for (auto target : map.second)
        {
            targetVec.push_back(target.targetID);
        }
        bool bTarget = false;
        for (auto target : xygsZcMap)
        {
            if (!targetVec.contains(target.targetID))
            {
                bTarget = true;
                break;
            }
        }
        if (bTarget)
        {
            bChange = map.first;
            TargetMap[map.first] = map.second;
            break;
        }
    }
    return bChange;
}

void qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget::q20ATraTrackingObjectWalk(
	XygsZC_Mark_01& xygsZC_Target, int disObject, float objectRealW, float objectRealH)
{
	double videoHeight = xygsZC_Target.videoHeight * 1.0;
	double videoWidth = xygsZC_Target.videoWidth * 1.0;
	double fov_wR = FOV_w * PI / 180.0;
	double fov_hR = fov_wR * (videoHeight / videoWidth);
	double Fpixel_w = videoWidth / (2 * tan(fov_wR / 2));
	double Fpixel_h = videoHeight / (2 * tan(fov_hR / 2));

	float objectDisW = (objectRealW * Fpixel_w) / xygsZC_Target.targetRectWidth;
	float objectDisH = (objectRealH * Fpixel_h) / xygsZC_Target.targetRectHeight;

	float objectDis = (objectDisW + objectDisH) / 2;
	
	//计算方向上的偏差
	int fdx = ((videoWidth / 2) - (xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2)) * -1;
	
	float dxSend = (fdx * 1.0 / (videoWidth / 2));
	

	qDebug() << "TrackingObject____videoCent: X: " << videoWidth / 2;
	qDebug() << "TrackingObject____targetRectCent: X: " << xygsZC_Target.targetRectX + xygsZC_Target.targetRectWidth / 2 <<
		"/tY: " << xygsZC_Target.targetRectY + xygsZC_Target.targetRectWidth / 2;
	qDebug() << "TrackingObject____FOV_WR: " << fov_wR << "/tFOV_HR: " << fov_hR << "/tFpixel_W: "
		<< Fpixel_w << "/tFpixel_H: " << Fpixel_h;
	qDebug() << "TrackingObject____DISObjectW: " << objectDisW << "/tDISObjectH: " << objectDisH
		<< "/tDISObject: "<< objectDis;
	qDebug() << "TrackingObject____fdx: " << fdx << "/tdxSend: " << dxSend;

	m_Q20AWeaponID = xygsZC_Target.componentID;
	qnzkna::framework::MessageReferenceProps props;
	props.put("WeaponID", QString::number(xygsZC_Target.componentID).toStdString());
	props.put("DisObject", QString::number(objectDis).toStdString());
	props.put("DisX", QString::number(dxSend).toStdString());
	props.put("ExpenctedDis", QString::number(disObject).toStdString());
	IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
		new qnzkna::framework::MessageReference("MsgQ20ATrackingObject", (char *)&xygsZC_Target, sizeof(xygsZC_Target), props));

}

bool PlayYoloVideoStreamManageWidget::StartVideoManageThread(const CVideoDevice &videoDevice)
{
    bool bResult = false;
    if (videoDevice.GetVideoDeviceType().empty() || videoDevice.GetVideoDeviceURL().empty())
    {
        return false;
    }

    if (videoDevice.GetVideoDeviceType() == "opencv" || videoDevice.GetVideoDeviceType() == "opencv54")
    {
        on_pushButton_PlayYoloVideoStreamManageWidget_Start_clicked();
    }

    if (bResult)
    {
        logInfo("播放失败");
    }
    return bResult;
}

bool PlayYoloVideoStreamManageWidget::StopVideoManageThread()
{
    bool bResult = false;

    on_pushButton_PlayYoloVideoStreamManageWidget_Stop_clicked();
    if (!bResult)
    {
        logInfo("播放停止! %s", m_VideoDevice.GetVideoDeviceURL().c_str());
    }
    else
    {
        logInfo("播放停止失败! %s", m_VideoDevice.GetVideoDeviceURL().c_str());
    }

    return bResult;
}

void PlayYoloVideoStreamManageWidget::StartVideoPlay()
{
    StartVideoManageThread(m_VideoDevice);
}

void PlayYoloVideoStreamManageWidget::StopVideoPlay()
{
    StopVideoManageThread();
}

void PlayYoloVideoStreamManageWidget::mousePressEvent(QMouseEvent *event)
{
}

void PlayYoloVideoStreamManageWidget::mouseReleaseEvent(QMouseEvent *event)
{
}

void PlayYoloVideoStreamManageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{

    if (event != nullptr)
    {
        std::string strComponent = m_VideoDevice.GetVideoComponentID();
        if (!strComponent.empty())
        {
            bool ok = false;
            long long int componentID = QString::fromStdString(strComponent).toLongLong(&ok);
            if (ok)
            {
                qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
                controlInfo.componentID = componentID;
                controlInfo.oper        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_MoveCenter;
                controlInfo.operData    = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;

                controlInfo.x           = event->x();
                controlInfo.y           = event->y();

                controlInfo.width       = this->width();
                controlInfo.heigth      = this->height();

                SendGimbalMoveCenter(strComponent, controlInfo);
            }
        }
    }
}

void PlayYoloVideoStreamManageWidget::mouseMoveEvent(QMouseEvent *event)
{
}

