#ifndef LY_UAVREALTIMESTATEMANAGESERVICE_H
#define LY_UAVREALTIMESTATEMANAGESERVICE_H

#include <QMetaType>

#include "IUAVRealTimeStateManageActivator.h"
#include "LY_UAVRealTimeStateManageService_global.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_UAVREALTIMESTATEMANAGESERVICESHARED_EXPORT LY_UAVRealTimeStateManageService
{
public:
    LY_UAVRealTimeStateManageService();
    inline static LY_UAVRealTimeStateManageService &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QTabWidget *pTab);
    bool OnTimer(int wParam);
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

private:
    IProj       *m_pProj;
    QCString	m_strDataDir;
    QCString	m_strConfDir;

private:
    IUAVRealTimeStateManageActivator m_IUAVRealTimeStateManageActivator;
};

inline LY_UAVRealTimeStateManageService &LY_UAVRealTimeStateManageService::Get()
{
    static LY_UAVRealTimeStateManageService sLY_UAVRealTimeStateManageService;
    return sLY_UAVRealTimeStateManageService;
}

#endif
