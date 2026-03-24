
#ifndef LY_AGRICULTUREHOMEPAGE_H
#define LY_AGRICULTUREHOMEPAGE_H

#include "LY_AgricultureHomepage_global.h"
#include <QCString.h>
#include <QMouseEvent>

class QPainter;
class IProj;
#include "ffmpeguntil.h"
#include "ly_moudlemanagewidget.h"
#include "ly_mointorviewmodel.h"
#include "ly_mointeraddwidget.h"
#include "ly_agriculhomepagewidget.h"
#include "ly_harmmanagerwidget.h"
class LY_AGRICULTUREHOMEPAGESHARED_EXPORT LY_AgricultureHomepage
{ 

public:
    LY_AgricultureHomepage();
    ~LY_AgricultureHomepage();
    inline static LY_AgricultureHomepage&Get();

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

	void OnEarthMessage(int wParam, void *lParam);

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
	void LY_AgricultureHomepage::L_X(double l, double b, double *x, double *y)
	{
		float x1 = 0, y1 = 0;
		if (m_pProj != nullptr)
		{
			m_pProj->jwxy(l, b, x, y);
		}
	}
	inline IProj *GetProj() const
	{
		return m_pProj;
	}
	inline const QCString GetDataDir() const { return m_strDataDir; };
	void setupWidgetLocal(QWidget *tempWidget, const char * _value, const char * _element);



	 LY_MoudleManageWidget * GetMoudleManagerWidget() const;
	 LY_AgriculHomePageWidget *GetHomePageWidget() const;
	 LY_MointerAddWidget *GetAddMonitorWidget() const;

	 LY_MointorViewModel *GetViewModel() const;
	 LY_HarmManagerWidget *GetHarmManangerWidget() const;
	 FFmpegUntil *GetFFmpegUntil() const;

private:
	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;

	LY_AgriculHomePageWidget *m_homePageWidget;

	LY_MoudleManageWidget * m_moudleManagerWidget;

	LY_MointorViewModel *m_viewModel;
	LY_MointerAddWidget *m_addMonitorWidget;
	LY_HarmManagerWidget *m_harmManagerWidget;
	FFmpegUntil *m_ffmpegUntil;
};

inline LY_AgricultureHomepage &LY_AgricultureHomepage::Get()
{
    static LY_AgricultureHomepage sLY_MonitorMoudle;
    return sLY_MonitorMoudle;
}


#endif // LY_AGRICULTUREHOMEPAGE_H
