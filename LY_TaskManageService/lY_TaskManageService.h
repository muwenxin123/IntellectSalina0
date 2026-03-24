#ifndef LY_TASKMANAGESERVICE_H
#define LY_TASKMANAGESERVICE_H

#include <QMetaType>
#include "lY_TaskManageService_global.h"
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ITaskManageImpl.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

class LY_TASKMANAGESERVICESHARED_EXPORT LY_TaskManageService
{
public:
    LY_TaskManageService();
    inline static LY_TaskManageService &Get();

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
    };
    inline const QCString GetDataDir() const
    {
        return m_strDataDir;
    };
    inline const QCString GetConfDir() const
    {
        return m_strConfDir;
    };
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    };
    inline void SetConfDir(const char *lpszDir)
    {
        m_strConfDir = lpszDir;
    };

public:

private:
    IProj       *m_pProj;
    QWidget		*m_pWnd;
    QCString	m_strDataDir;
    QCString	m_strConfDir;

private:
    static qnzkna::framework::IRegistry			*s_IRegistryImpl;
    static qnzkna::TaskManage::ITaskManageImpl		s_IFormManageImpl;
};

inline LY_TaskManageService &LY_TaskManageService::Get()
{
    static LY_TaskManageService sLY_TaskManageService;
    return sLY_TaskManageService;
}

#endif
