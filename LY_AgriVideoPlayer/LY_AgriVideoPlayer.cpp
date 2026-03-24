
//!!!!!!!!
//!!!!!!!!×￠òa￡o??±????t±à??′óutf-8???aansi2??ü1?ê1ó?vc2015±àò?í¨1y￡?gcc?é?ü2?ó??ü??
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
#include "AIWebSocketClient.h"
#include <QDb.h>
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"

#include "LY_AgriVideoPlayer.h"
#include "LY_AgriVideoPlayerInterface.h"

#include "LY_AgriVideoPlayerActivator.h"
#include "IFunctionInterface.h"
#include "gdd/WebSocketClient.h"
#include "UAVAgriVideoDialogList.h"
#include "IAgriVideoPlayerStateDialog.h"
#include "ISelectYoloModelDialog.h"
#include "AIWebSocketClient.h"
#include "DataResult.h"
#include "StatisticsCard.h"
//#include "IAgriVideoPlayerDJIDialog.h"
#include "PartMap.h"
Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)


qnzkna::framework::IRegistry           *LY_AgriVideoPlayer::s_IRegistryImpl = nullptr;


LY_AgriVideoPlayer:: LY_AgriVideoPlayer()
	:m_videoConfig(nullptr)
{
	LY_AgriVideoPlayerActivator::Get();

}

LY_AgriVideoPlayer::~LY_AgriVideoPlayer()
{
	// 设置全局退出标志，关闭连接
	AIWebSocketClient::setApplicationShuttingDown();
	AIWebSocketClient::CleanupAllConnections();

	// 等待处理完成（考虑更健壮的方式）
	QThread::msleep(500);

	// 删除需要手动管理的资源
	delete m_pDataResult;
	delete m_pEventRecord;
	delete m_pAIWebSocketClient;   
	delete m_pView1;                
	delete m_pView2;
	delete m_pView3;
	delete m_pView4;
	delete m_pView5;

	m_videoWidget.clear();

	// 清理 m_videoConfig（只删一次）
	delete m_videoConfig;
	m_videoConfig = nullptr;

	qDebug() << "LY_AgriVideoPlayer 析构完成";
}

//LY_AgriVideoPlayer::~LY_AgriVideoPlayer()
//{
//	m_videoConfig = nullptr;
//	delete m_videoConfig;
//	qDebug() << "LY_AgriVideoPlayer 开始析构";
//
//	// 1. 先设置全局退出标志（需要在 AIWebSocketClient 中添加）
//	AIWebSocketClient::setApplicationShuttingDown();
//
//	// 2. 先关闭 WebSocket 连接，停止接收新消息
//	AIWebSocketClient::CleanupAllConnections();
//
//	// 3. 给一点时间让正在处理的消息完成
//	QThread::msleep(500);
//
//	// 4. 主动通知 DataResult 关闭
//	if (m_pDataResult) {
//		m_pDataResult->shutdown();  // 提前清理
//		delete m_pDataResult;  // 安全删除
//		m_pDataResult = nullptr;
//	}
//
//	// 5. 同样处理 EventRecord
//	if (m_pEventRecord) {
//		delete m_pEventRecord;
//		m_pEventRecord = nullptr;
//	}
//
//	// 6. 清理其他资源
//	delete m_videoConfig;
//	m_videoConfig = nullptr;
//
//	qDebug() << "LY_AgriVideoPlayer 析构完成";
//}


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
	case 433387:
	{
		if (m_pUAVAgriVideoDialogList)
		{
			m_pUAVAgriVideoDialogList->show();
			m_pUAVAgriVideoDialogList->hide();
		}
		
		if (m_statisticsCard) m_statisticsCard->show();
		// ?? 关键：当显示UAVAgriVideoDialogList时，隐藏独立窗口
		if (m_pView1) m_pView1->show();
		if (m_pView2) m_pView2->hide();
		if (m_pView3) m_pView3->hide();
		if (m_pView4) m_pView4->hide();
		if (m_pView5) m_pView5->hide();

		if (m_pUAVAgriVideoDialogList)
		{
			m_pUAVAgriVideoDialogList->show();
		}
		if (m_pIAgriVideoPlayerStateDialog)
		{
			m_pIAgriVideoPlayerStateDialog->Init(0);
			m_pIAgriVideoPlayerStateDialog->show();
			m_pIAgriVideoPlayerStateDialog->hide();
		}
		if (m_pISelectYoloModelDialog)
		{
			m_pISelectYoloModelDialog->show();
		}
		if (m_p2DMapView)
		{
			m_p2DMapView->show();
		}
// 		if (m_pIAgriVideoPlayerDJIDialog)
// 		{
// 			m_pIAgriVideoPlayerDJIDialog->Init();
// 			m_pIAgriVideoPlayerDJIDialog->show();
// 
// 		}
		if (m_pEventRecord)
		{
			m_pEventRecord->show();
		}
		if (m_pDataResult)
		{
			m_pDataResult->show();
		}
	}
	break;
    default:
	{
		// 隐藏视频时，两者都隐藏
		if (m_pUAVAgriVideoDialogList)
		{
			m_pUAVAgriVideoDialogList->hide();
		}



		if (m_pUAVAgriVideoDialogList)
		{
			m_pUAVAgriVideoDialogList->hide();
		}
		if (m_pIAgriVideoPlayerStateDialog)
		{
			m_pIAgriVideoPlayerStateDialog->hide();
		}
		if (m_pISelectYoloModelDialog)
		{
			m_pISelectYoloModelDialog->hide();
		}
		if (m_p2DMapView)
        {
            m_p2DMapView->hide();
        }
// 		if (m_pIAgriVideoPlayerDJIDialog)
// 		{
// // 			m_pIAgriVideoPlayerDJIDialog->hide();
// 		}
		if (m_pEventRecord)
		{
			m_pEventRecord->hide();
		}
		if (m_pDataResult)
		{
			m_pDataResult->hide();
		}
	}
        break;
    }
    return false;
}

bool LY_AgriVideoPlayer::OnCreate(int wParam, QWidget *pWidget)
{
	QSize screenSize = qApp->property("ly_screen_xy").toSize();
	if (m_videoConfig == nullptr)
	{
		m_videoConfig = new VideoConfig();
	}

    switch (wParam)
    {
	case 0:
	{

	}
	break;
    case 1:   
    {
		
    }
        break;
	case 2:   //é?
	{

	}
	break;
	case 3:   //??
	{

	}
	break;
	case 6:
    {
		if (m_p2DMapView)
		{
			
		}
        
    }
    break;
	case 1000:
	{

	if (m_pEventRecord == nullptr)
		{
			m_pEventRecord = new EventRecord(pWidget);
			m_pEventRecord->move(QPoint(0, screenSize.height() / 3));
			m_pEventRecord->resize(QSize(screenSize.width() / 4, screenSize.height() / 3));
			m_pEventRecord->hide();

		}
	if (m_pDataResult == nullptr) {
		m_pDataResult = new DataResult(pWidget);
		m_pDataResult->move(QPoint(0, screenSize.height() *2 / 3));
		m_pDataResult->resize(QSize(screenSize.width() / 4, screenSize.height() / 3));
		m_pDataResult->hide();

	}
	
	if (m_statisticsCard == nullptr) {
		m_statisticsCard = new StatisticsCard(pWidget);
		m_statisticsCard->move(QPoint(screenSize.width() * 17 / 20, screenSize.height() / 3));
		m_statisticsCard->resize(QSize(screenSize.width() * 3 / 20, screenSize.height() / 3));
		m_statisticsCard->hide();

	}
		if (m_p2DMapView == nullptr)
		{
			m_p2DMapView = new QWidget(pWidget, Qt::Window | Qt::FramelessWindowHint);
			QVBoxLayout* layout = new QVBoxLayout(m_p2DMapView);

			//layout->addWidget(viewWidget);
			layout->addWidget(CPartMap::Get());
			m_p2DMapView->setLayout(layout);
			m_p2DMapView->move(QPoint(screenSize.width() - screenSize.width() / 8 - 30, screenSize.height() / 5));
			m_p2DMapView->resize(QSize(screenSize.width() / 8, screenSize.height() / 8/*1160, 640*/));
			m_p2DMapView->hide();

			layout->setContentsMargins(0, 0, 0, 0);  // 左、上、右、下
			layout->setSpacing(0);                   // 控件之间的间距
		}

#define GDD
		if (m_pView == nullptr)
		{
#ifdef GDD
			//初始化界面数据
			m_pView1 = new CameraWidget(110506);
			m_pView2 = new CameraWidget(110507);
			m_pView3 = new CameraWidget(110508);
			m_pView4 = new CameraWidget(110509);
			m_pView5 = new CameraWidget(110510);
			QObject::connect(m_pView1, &CameraWidget::boxClicked, this, &LY_AgriVideoPlayer::onBoxClicked);
			QObject::connect(m_pView2, &CameraWidget::boxClicked, this, &LY_AgriVideoPlayer::onBoxClicked);
			QObject::connect(m_pView3, &CameraWidget::boxClicked, this, &LY_AgriVideoPlayer::onBoxClicked);
			QObject::connect(m_pView4, &CameraWidget::boxClicked, this, &LY_AgriVideoPlayer::onBoxClicked);
			QObject::connect(m_pView5, &CameraWidget::boxClicked, this, &LY_AgriVideoPlayer::onBoxClicked);

			//StartGdd("192.168.2.100");
			//StartAI("192.168.1.138");
			m_pAIWebSocketClient = new AIWebSocketClient();
			m_pAIWebSocketClient->StartMultiStreamAI("192.168.1.138");

#endif // GDD
		}

		if (m_videoConfig)
		{
			m_videoWidget.clear();
			m_videoPlayerMap.clear();

			for (const auto &InfoData : VideoInfoVec)
			{
				CameraWidget *videoPlay = new CameraWidget(InfoData.id);
				m_videoPlayerMap[InfoData.id] = videoPlay;
				m_videoWidget.push_back(videoPlay); 
			}
#ifdef GDD
			//CameraWidget *videoPlay = new CameraWidget(110505);

			// 存储映射关系
			m_streamIdMap[110506] = "v1";
			m_streamIdMap[110507] = "v2";
			m_streamIdMap[110508] = "v3";
			m_streamIdMap[110509] = "v4";
			m_streamIdMap[110510] = "v5";

			// 反向映射
			m_cameraIdMap["v1"] = 110506;
			m_cameraIdMap["v2"] = 110507;
			m_cameraIdMap["v3"] = 110508;
			m_cameraIdMap["v4"] = 110509;
			m_cameraIdMap["v5"] = 110510;

			m_videoPlayerMap[110506] = m_pView1;
			m_videoPlayerMap[110507] = m_pView2;
			m_videoPlayerMap[110508] = m_pView3;
			m_videoPlayerMap[110509] = m_pView4;
			m_videoPlayerMap[110510] = m_pView5;

			m_videoWidget.push_back(m_pView1);
			m_videoWidget.push_back(m_pView2);
			m_videoWidget.push_back(m_pView3);
			m_videoWidget.push_back(m_pView4);
			m_videoWidget.push_back(m_pView5);

#endif // GDD	
			
		}
		

		if (m_pISelectYoloModelDialog == nullptr)
		{
			m_pISelectYoloModelDialog = new ISelectYoloModelDialog(pWidget);
			m_pISelectYoloModelDialog->move(QPoint(10, 80));
			m_pISelectYoloModelDialog->resize(QSize(screenSize.width() * 1 / 4 - 10, screenSize.height() * 1 / 4));
			m_pISelectYoloModelDialog->hide();
		}
		if (m_pUAVAgriVideoDialogList == nullptr)
		{
			m_pUAVAgriVideoDialogList = new UAVAgriVideoDialogList(pWidget);

			if (m_videoWidget.size() > 0)
			{
				m_pUAVAgriVideoDialogList->InitDialog(m_videoWidget);
			}

			m_pUAVAgriVideoDialogList->move(QPoint(screenSize.width() * 1 / 4, 80));
			m_pUAVAgriVideoDialogList->resize(QSize(screenSize.width() * 3/5, screenSize.height() *2/ 3/*1160, 640*/));
			m_pUAVAgriVideoDialogList->hide();
		}
		if (nullptr == m_pIAgriVideoPlayerStateDialog)
		{
			m_pIAgriVideoPlayerStateDialog = new IAgriVideoPlayerStateDialog(pWidget);
			m_pIAgriVideoPlayerStateDialog->move(QPoint(screenSize.width() * 1 / 4 + 300, 90));
			m_pIAgriVideoPlayerStateDialog->resize(QSize(screenSize.width() * 3 / 4 - 300,60));
			m_pIAgriVideoPlayerStateDialog->hide();
		}
// 		if (nullptr == m_pIAgriVideoPlayerDJIDialog)
// 		{
// // 			m_pIAgriVideoPlayerDJIDialog = new IAgriVideoPlayerDJIDialog(pWidget);
// // 			m_pIAgriVideoPlayerDJIDialog->move(QPoint(screenSize.width() - screenSize.width() / 8 - 30, screenSize.height() / 2));
// // 			m_pIAgriVideoPlayerDJIDialog->resize(QSize(screenSize.width() / 8, screenSize.height() / 8/*1160, 640*/));
// // 			m_pIAgriVideoPlayerDJIDialog->hide();
// 		}
		
	}
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
	return m_pEventRecord;
}

// LY_AgriVideoPlayer.cpp
void LY_AgriVideoPlayer::updateDetectionData(const WS::DetectionData& data)
{
	// 调用setData
	auto configs = AIWebSocketClient::getCurrentStreamConfigs();
		 if (m_pView1 && data.metadata.videoid == configs[0].streamId.toStdString()) m_pView1->setData(data);
	else if (m_pView2 && data.metadata.videoid == configs[1].streamId.toStdString()) m_pView2->setData(data);
	else if (m_pView3 && data.metadata.videoid == configs[2].streamId.toStdString()) m_pView3->setData(data);
	else if (m_pView4 && data.metadata.videoid == configs[3].streamId.toStdString()) m_pView4->setData(data);
	else if (m_pView5 && data.metadata.videoid == configs[4].streamId.toStdString()) m_pView5->setData(data);

	//更新统计卡片
	if (m_statisticsCard && m_statisticsCard->isVisible()) {
		StatisticsData stats;

		// 从 DetectionData 中提取统计数据
		for (const auto& bbox : data.bboxes) {
			// 解析类别名（从 "coco.pt:car" 提取 "car"）
			QString label = QString::fromStdString(bbox.label);
			QString className = label.contains(":") ? label.split(":").last() : label;
			qDebug() << className;
			stats.classCounts[className]++;
			stats.avgConfidence[className] += bbox.score;
			stats.totalCount++;
		}


		// 添加模型信息
		stats.modelNames << QString::fromStdString(data.metadata.type);  

																		 // 更新统计卡片
		//m_statisticsCard->updateStatistics(stats);
	}
}

void LY_AgriVideoPlayer::updateEventData(const WS::EventData& data)
{
	if (m_pEventRecord) {
		m_pEventRecord->setEventData(data);
	}
}


void LY_AgriVideoPlayer::updateDataResult(const WS::EventData& data)
{
	if (m_pDataResult) {
		m_pDataResult->addImage(data);
	}
}

//void LY_AgriVideoPlayer::updateRectimage(QPainter p) {
//	if (m_pDataResult) {
//		m_pDataResult->addRect(p);
//	}
//}

void LY_AgriVideoPlayer::updatejson(const QString vid, const QString& modelName, bool enable)
{
	QJsonObject json;
	//json["video"] = vid;
	//json["model"] = modelName;
	//json["enable"] = enable;
	//json["model(pt)"] = modelName + ".pt";
	json["op"] = "toggle";
	json["model"] = modelName + ".pt"; 
	json["enabled"] = enable;  

	m_pAIWebSocketClient->SendWsMsg(vid, QJsonDocument(json).toJson());
}

void LY_AgriVideoPlayer::onBoxClicked(int videoId, const BoxInfo& boxInfo)
{
	qDebug() << "收到框点击信号 - 视频:" << videoId;
	qDebug() << "  类别:" << boxInfo.className;
	qDebug() << "  置信度:" << boxInfo.score;
	qDebug() << "  trackId:" << boxInfo.trackId;
	qDebug() << "  完整标签:" << QString::fromStdString(boxInfo.bbox.label);

	// 创建事件记录
	if (m_pEventRecord) {
		WS::EventData eventData;
		eventData.time = QTime::currentTime();
		eventData.type = 2;  // 自定义类型：框点击事件
		eventData.videoid = std::to_string(videoId);

		// 生成事件内容
		eventData.eventcontent = QString("点击检测框 - %1 (置信度:%2% 跟踪ID:%3)")
			.arg(boxInfo.className)
			.arg(int(boxInfo.score * 100))
			.arg(boxInfo.trackId);

		m_pEventRecord->setEventData(eventData);
	}

	// 如果有状态栏对话框，显示信息
	if (m_pIAgriVideoPlayerStateDialog) {
		// 注意：IAgriVideoPlayerStateDialog 可能没有 setStatusText 方法
		// 如果有其他方法可以显示状态，就用那个方法
		// 或者通过信号槽的方式传递信息
	}
}


