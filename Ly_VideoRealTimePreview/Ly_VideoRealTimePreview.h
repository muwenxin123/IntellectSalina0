
#ifndef LY_VIDEOREALTIMEPREVIEW_H
#define LY_VIDEOREALTIMEPREVIEW_H

#include "Ly_VideoRealTimePreview_global.h"
#include <QCString.h>
#include "IVideoRealTimePreviewActivator.h"	
#include <QMap>
#include <atomic>
#include <QMutex>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class QThreadPool;
class QListView;
class QTreeView;
class PopForm;
class QWidget;
class VedioRealTimeConfigureDlg;
class Ly_VideoRealTimePreviewWidget;
class VideoRealTimePreviewGraphicWidget;

namespace VideoRealTime {
	// 视图的位置
	enum EViewLocation {
		ViewTop,
		ViewBottom,
		ViewLeft,
		ViewRight,
	};

	struct VideoRealTimeConfig
	{
		VideoRealTimeConfig() {}

		~VideoRealTimeConfig() {}

		// 从文件读取数据
		void Sync();

		// 保存数据到文件
		void Save();

		EViewLocation viewLocation;					 // 视图的位置
		bool showFlag = false;						 // 显示标识
		bool useSortByPosAndYawFlag = false;		 // 是否使用根据位置方位排序
		double dYaw = 0.0;							 // 方位角
		double dLon = 0.0;							 // 经度
		double dLat = 0.0;							 // 纬度
		bool useFriendTrackFlag = false;			 // 是否使用目标追踪
		long long lObserverIdentity = 0;			 // 目标追踪对象ID
		double detailOccupyValue = 0;				 // 占比
	};
};

class LY_VIDEOREALTIMEPREVIEWSHARED_EXPORT Ly_VideoRealTimePreview
{
public:
    Ly_VideoRealTimePreview();
    inline static Ly_VideoRealTimePreview&Get();

	void Startup();
	void Shutdown();
	void Reset();
	VideoRealTime::VideoRealTimeConfig* getConfig();
	QMutex& getConfigMutex();

	long long GetCurComponentID();

	void NotifyConfigureChanged();

	double GetHeight(double lon , double lat);

	void onFocalZoomChanged(int imageZoom);
	void CameraAttributeChanged(long long componentID, double yawValue, double pitchValue, double rollValue);

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QWidget *pWidget);
    bool OnTimer(int wParam);
	void On2d3dChanged(int wParamID);
    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    };
    inline const QCString GetDataDir() const { return m_strDataDir; };
    inline const QCString GetConfDir() const { return m_strConfDir; };
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    };
    inline void SetConfDir(const char *lpszDir)
    {
        m_strConfDir = lpszDir;
    };

private:
	QMutex m_configMutex;
	QListView	*m_pList;
    QTreeView	*m_pTree;
    PopForm		*m_pPopForm;
    IProj       *m_pProj;
    QWidget		*m_pWnd;
    QCString	m_strDataDir;
    QCString	m_strConfDir;
	VedioRealTimeConfigureDlg* m_pDlg = nullptr;
#ifdef USE_GRAPHICS_VIEW
	VideoRealTimePreviewGraphicWidget* m_VideoRealTimePreviewWidget;
#else
	Ly_VideoRealTimePreviewWidget* m_VideoRealTimePreviewWidget;
#endif // USE_GRAPHICS_VIEW

	QWidget* m_p2DView = nullptr;
	QWidget* m_p3DView = nullptr;
	VideoRealTime::VideoRealTimeConfig m_config;
};

inline Ly_VideoRealTimePreview &Ly_VideoRealTimePreview::Get()
{
    static Ly_VideoRealTimePreview sLy_VideoRealTimePreview;
    return sLy_VideoRealTimePreview;
}


#endif // LY_VIDEOREALTIMEPREVIEW_H
