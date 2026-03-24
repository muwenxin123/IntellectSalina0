
//!!!!!!!!
//!!!!!!!!注意：将本文件编码从utf-8改为ansi才能够使用vc2015编译通过，gcc可能不用更改
//!!!!!!!!


#include "LY_AgriVideoPlayer.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "LY_AgriVideoPlayerInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
#include <QCString.h>

#include <QDb.h>
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"

#include "LY_AgriVideoPlayer.h"
#include "LY_AgriVideoPlayerInterface.h"

#include "LY_AgriVideoPlayerActivator.h"
#include "IFunctionInterface.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)


qnzkna::framework::IRegistry           *LY_AgriVideoPlayer::s_IRegistryImpl = nullptr;


LY_AgriVideoPlayer:: LY_AgriVideoPlayer()
	:m_videoConfig(nullptr)
{
	LY_AgriVideoPlayerActivator::Get();

}

LY_AgriVideoPlayer::~LY_AgriVideoPlayer()
{
	m_videoConfig = nullptr;
}
void LY_AgriVideoPlayer::Reset()
{

}

void LY_AgriVideoPlayer::Startup()
{
    IFunctionInterface &ifi = LY_AgriVideoPlayerInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			LY_AgriVideoPlayerActivator::Get().start(pIRegistry);
		}
	}
}

void LY_AgriVideoPlayer::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriVideoPlayerInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			LY_AgriVideoPlayerActivator::Get().stop(pIRegistry);
		}
	}
}

bool LY_AgriVideoPlayer::OnNetRecv(int nLength, char *lpData)
{
	return false;
}
bool LY_AgriVideoPlayer::DrawGraph(QPainter &painter, IProj *pProj)
{

    return false;
}

bool LY_AgriVideoPlayer::DrawDynamic(QPainter &painter, IProj *pProj)
{

    return false;
}

bool LY_AgriVideoPlayer::OnMouseButtonDown(QMouseEvent *e)
{

    return false;
}

bool LY_AgriVideoPlayer::OnMouseDoubleClick(QMouseEvent *e)
{
    qDebug("OnMouseDoubleClick");
    return false;
}

bool LY_AgriVideoPlayer::OnCommand(int nID)
{
    switch (nID)
    {
    case 43385:{


	} break;
    default:
        break;
    }
    return false;
}

bool LY_AgriVideoPlayer::OnCreate(int wParam, QWidget *pWidget)
{
	
	if (m_videoConfig == nullptr)
	{
		m_videoConfig = new VideoConfig();
	}

    switch (wParam)
    {
	case 1000:
	{

	}
	break;

	case 0:
	{

	}
	break;
 //   case 1:   
 //   {
	//	LyDockWidget *pDockvideoPlay = LyDockManager::instance()->dockWidget("pDockvideoPlay");
	//	QVBoxLayout *VBoxLayout = new QVBoxLayout(pDockvideoPlay);
	//	VBoxLayout->setSpacing(4);
	//	VBoxLayout->setContentsMargins(4, 4, 4, 4);
	//	if (pDockvideoPlay == nullptr)
	//	{
	//			pDockvideoPlay = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("模型图像识别"), "pDockvideoPlay");
	//			LyDockManager::instance()->embedToolDockWidget(pDockvideoPlay, LyDockDef::Left);
	//			//pDockvideoPlay->setLayout(gridLayout);
	//	}
	//	if (m_videoConfig)
	//	{
	//		m_videoWidget.clear();
	//		m_videoPlayerMap.clear();
	//		for (const auto &InfoData : VideoInfoVec)
	//		{
	//			CameraWidget *videoPlay = new CameraWidget(InfoData.id);
	//			videoPlay->move(20, 20);
	//			m_videoPlayerMap[InfoData.id] = videoPlay;
	//			videoPlay->setMinimumSize(QSize(330, 260));
	//			videoPlay->hide();
	//			m_videoWidget.push_back(videoPlay);
	//			VBoxLayout->addWidget(videoPlay);
	//			//pDockvideoPlay->setCentralWidget(videoPlay);

	//		}	

	//	}

	//	//LyDockWidget *peventInformation = LyDockManager::instance()->dockWidget("peventInformation");

	//	LyDockWidget *peventInformationdocker = LyDockManager::instance()->dockWidget("peventInformation");
	//	if (peventInformationdocker == nullptr)
	//	{
	//		peventInformationdocker = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("事件信息"), /*QObject::tr("peventInformation"),*/ "peventInformation");
	//		LyDockManager::instance()->embedToolDockWidget(peventInformationdocker, LyDockDef::Right);
	//	}

	//	if (m_eventrecord == nullptr)
	//	{
	//		m_eventrecord = new EventRecord;
	//		peventInformationdocker->setCentralWidget(m_eventrecord);
	//		m_eventrecord->setWindowTitle(QObject::tr("TargetAssign"));
	//		LyDockWidget::connect(m_eventrecord, &EventRecord::windowTitleChanged, peventInformationdocker, &LyDockWidget::setWindowTitle);
	//	}
	//	//if (peventInformation == nullptr)
	//	//{
	//	//	peventInformation = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("事件信息"), "peventInformation");
	//	//	LyDockManager::instance()->embedToolDockWidget(peventInformation, LyDockDef::Left);
	//	//	peventInformation->setCentralWidget(m_eventrecord);
	//	//}
	//	
	//}
	case 1:
	{
		if (m_videoConfig)
		{
			m_videoWidget.clear();
			m_videoPlayerMap.clear();
			for (const auto &InfoData : VideoInfoVec)
			{
				CameraWidget *videoPlay = new CameraWidget(InfoData.id);
				videoPlay->move(20, 20);
				m_videoPlayerMap[InfoData.id] = videoPlay;
				videoPlay->setMinimumSize(QSize(330, 260));
				videoPlay->hide();
				m_videoWidget.push_back(videoPlay);
			}
		}
	}
        break;
	case 2:   //上
	{

	}
	break;
	case 3:   //下
	{

	}
	break;
	case 4:
		break;
	}
    return false;
}

bool LY_AgriVideoPlayer::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1://200ms
		LY_AgriVideoPlayerInterface::Get().Refresh(1);

		if (m_IsShowViedoPlayWidget && !m_videoWidget.empty())
		{
			for (const auto &videoWidget: m_videoWidget) {
				if (videoWidget && videoWidget->isHidden())
				{
					videoWidget->show();
				}
				else
				{
					continue;
				}
			}
		
			m_IsShowViedoPlayWidget = false;
		}
        break;
    case 2://1s


        break;
    case 3://60s
        break;
    }
    return true;
}
VideoConfig *LY_AgriVideoPlayer::GetVideoConfig() const
{
	return m_videoConfig;
}

std::map<int, CameraWidget*> LY_AgriVideoPlayer::GetVideoPlayerMap()
{
	return m_videoPlayerMap;
}

EventRecord * LY_AgriVideoPlayer::GetEventRecord()
{
	return m_eventrecord;
}
