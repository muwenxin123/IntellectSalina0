#ifndef LY_IPLATFORMSTATEMONITORING_H
#define LY_IPLATFORMSTATEMONITORING_H

#include <QMetaType>

#include "LY_IPlatformStateMonitoring_global.h"
#include "UAVPlaneStatesDialog.h"
#include "UAVPlatformStateDialog.h"
#ifdef QWT_CONFIG_ENABLE_DEFINES
#include "UAVPlanRealTimeAlititudeDialog.h"
#else
class UAVPlanRealTimeAlititudeDialog;
#endif
#include "UAVPlanSystemState.h"

#include <QCString.h>

#include "uavplanestare_forfire.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class UAVPlaneStare_ForFire;

class LY_IPLATFORMSTATEMONITORINGSHARED_EXPORT LY_IPlatformStateMonitoring
{
public:
    LY_IPlatformStateMonitoring();
    inline static LY_IPlatformStateMonitoring &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
	bool OnCommand(int nID, void *lParam = nullptr);
    bool OnCreate(int wParam, QWidget *pTab);
    bool OnTimer(int wParam);
    bool OnNetRecv(int nLength, char *lpData);
    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    }
    inline const QCString GetDataDir() const
    {
        return m_strDataDir;
    }
    inline const QCString GetConfDir() const
    {
        return m_strConfDir;
    }
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    }
    inline void SetConfDir(const char *lpszDir)
    {
        m_strConfDir = lpszDir;
    }
	inline IProj* GetProj() const
	{
		return m_pProj;
	}
    UAVPlaneStatesDialog *getPUAVPlaneStatesDialog() const;

    UAVPlatformStateDialog *getPUAVPlatformStateDialog() const;

    UAVPlanRealTimeAlititudeDialog *getPUAVPlanRealTimeAlititudeDialog() const;

    UAVPlanSystemState *getPUAVPlanSystemState() const;

	UAVPlaneStare_ForFire* getPUAVPlanStare_ForFire() const;

    void SetScreenCenter(const double dLon, const double dLat);

private:
    IProj       *m_pProj;
    QCString	m_strDataDir;
    QCString	m_strConfDir;

private:
    UAVPlaneStatesDialog             *m_pUAVPlaneStatesDialog;
    UAVPlatformStateDialog           *m_pUAVPlatformStateDialog;
    UAVPlanRealTimeAlititudeDialog   *m_pUAVPlanRealTimeAlititudeDialog;
    UAVPlanSystemState               *m_pUAVPlanSystemState;
	UAVPlaneStare_ForFire*			  m_pUAVPlaneStare_ForFire;
};

inline LY_IPlatformStateMonitoring &LY_IPlatformStateMonitoring::Get()
{
    static LY_IPlatformStateMonitoring sLY_IPlatformStateMonitoring;
    return sLY_IPlatformStateMonitoring;
}

#endif
