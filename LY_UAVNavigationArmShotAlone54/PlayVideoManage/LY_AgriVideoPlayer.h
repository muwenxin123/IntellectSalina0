
#ifndef LY_AgriQuickSprayService_H
#define LY_AgriQuickSprayService_H

#include "LY_AgriVideoPlayer_global.h"
#include <QCString.h>
#include <QMouseEvent>
#include "ServiceFramework/IRegistry.h"
#include "camerawidget.h"
#include "videoconfig.h"
#include "EventRecord.h"
class QPainter;
class IProj;


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

	VideoConfig *GetVideoConfig() const;
	std::map<int, CameraWidget*> GetVideoPlayerMap();
	EventRecord *GetEventRecord();

private:
	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;

	bool         m_IsShowViedoPlayWidget = true;
	EventRecord *m_eventrecord = nullptr;
	VideoConfig *m_videoConfig = nullptr;

private:
	static qnzkna::framework::IRegistry             *s_IRegistryImpl;
	std::vector<CameraWidget*> m_videoWidget;
	std::map<int, CameraWidget*> m_videoPlayerMap;
};

inline LY_AgriVideoPlayer &LY_AgriVideoPlayer::Get()
{
    static LY_AgriVideoPlayer sLY_MonitorMoudle;
    return sLY_MonitorMoudle;
}


#endif


