
#ifndef LY_AgriQuickSprayService_H
#define LY_AgriQuickSprayService_H

#include "EventRecord.h"
#include "DataResult.h"
#include "LY_AgriVideoPlayer_global.h"
#include <QCString.h>
#include <QMouseEvent>
#include "ServiceFramework/IRegistry.h"
#include "camerawidget.h"
#include "videoconfig.h"
#include <memory>
//#include "gdd/GddVedioContainer.h"
//#include "gdd/WebSocketClient.h"  
//#include "AIWebSocketClient.h"
#include "ISelectYoloModelDialog.h"
class QPainter;
class IProj;
class UAVAgriVideoDialogList;
class IAgriVideoPlayerStateDialog;
class ISelectYoloModelDialog;
class IAgriVideoPlayerDJIDialog;
class AIWebSocketClient;

class LY_AGRIVIDEOPLAYERSHARED_EXPORT LY_AgriVideoPlayer
{ 

public:
	LY_AgriVideoPlayer();
    ~LY_AgriVideoPlayer();
    inline static LY_AgriVideoPlayer&Get();

	void Startup();
	void Shutdown();
	void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QWidget *pWidget);
    bool OnTimer(int wParam);
	bool OnNetRecv(int nLength, char *lpData);

	inline void SetDataDir(const char *lpszDir)
	{
		m_strDataDir = lpszDir;
	}
	inline void SetConfDir(const char *lpszDir)
	{
		m_strConfDir = lpszDir;
	}
	inline void SetCoordinateTransformation(IProj *pProj)
	{
		m_pProj = pProj;
	}
	inline UAVAgriVideoDialogList* GetUAVAgriVideoDialogList()
	{
		return m_pUAVAgriVideoDialogList;
	}
	inline IAgriVideoPlayerStateDialog* GetIAgriVideoPlayerStateDialog()
	{
		return m_pIAgriVideoPlayerStateDialog;
	}
	inline ISelectYoloModelDialog* GetISelectYoloModelDialog()
	{
		return m_pISelectYoloModelDialog;
	}

// 	inline IAgriVideoPlayerDJIDialog* GetIAgriVideoPlayerDJIDialog()
// 	{
// 		return m_pIAgriVideoPlayerDJIDialog;
// 	}
	VideoConfig *GetVideoConfig() const;
	std::map<int, CameraWidget*> GetVideoPlayerMap();
	EventRecord *GetEventRecord();
	void updateDetectionData(const WS::DetectionData& data);
	void updateEventData(const WS::EventData& data);
	void updateDataResult(const WS::EventData& data);
	void updatejson(const QString vid, const QString& modelName, bool enable);
	void updateDataResultShared(std::shared_ptr<WS::EventData> event);
	//bool StartMultipleAI(const char *lpszHost);
	//void updateRectimage(QPainter p);
private:
	// 摄像头ID <-> 视频流ID 映射
	QMap<int, QString> m_streamIdMap;      // 摄像头ID -> 流ID
	QMap<QString, int> m_cameraIdMap;      // 流ID -> 摄像头ID
private:
	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;

	bool         m_IsShowViedoPlayWidget = true;
	EventRecord *m_eventrecord = nullptr;
	VideoConfig *m_videoConfig = nullptr;
	CameraWidget* m_pView = nullptr;
	CameraWidget* m_pView1 = nullptr;
	CameraWidget* m_pView2 = nullptr;
	CameraWidget* m_pView3 = nullptr;
	CameraWidget* m_pView4 = nullptr;
	CameraWidget* m_pView5 = nullptr;

public:
	AIWebSocketClient *m_pAIWebSocketClient = nullptr;
	ISelectYoloModelDialog *m_pISelectYoloModelDialog = nullptr;
	EventRecord *m_pEventRecord = nullptr;
	DataResult *m_pDataResult = nullptr;
	
	// // 新增：根据窗口ID更新数据
 //   void updateDetectionDataByWindowId(const WS::DetectionData& data, int windowId);
	//// 🔴 添加新的数据接收函数
	//void updateDetectionDataByVideoId(const WS::DetectionData& data, int windowId, const QString& videoId);
private:
	static qnzkna::framework::IRegistry             *s_IRegistryImpl;
	std::vector<CameraWidget*> m_videoWidget;
	std::map<int, CameraWidget*> m_videoPlayerMap;
	UAVAgriVideoDialogList*							m_pUAVAgriVideoDialogList;
	IAgriVideoPlayerStateDialog*					m_pIAgriVideoPlayerStateDialog;
	
// 	IAgriVideoPlayerDJIDialog*						m_pIAgriVideoPlayerDJIDialog;

//// 🔴 添加视频ID到窗口的映射
//	std::map<QString, CameraWidget*> m_videoToWindowMap;
    QWidget* m_p2DMapView;//二维地图小界面
};

inline LY_AgriVideoPlayer &LY_AgriVideoPlayer::Get()
{
    static LY_AgriVideoPlayer sLY_MonitorMoudle;
    return sLY_MonitorMoudle;
}

#endif
