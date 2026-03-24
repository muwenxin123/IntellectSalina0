#ifndef LY_CONFIGBUNDLECONTROLSERVICE_H
#define LY_CONFIGBUNDLECONTROLSERVICE_H

#include <QMetaType>
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "SystemConfig/IConfigBundleControlImpl.h"
#include "SystemConfig/IConfigProjectControlImpl.h"

#include "LY_ConfigBundleControlService_global.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

class LY_CONFIGBUNDLECONTROLSERVICESHARED_EXPORT LY_ConfigBundleControlService
{
public:
    LY_ConfigBundleControlService();
    inline static LY_ConfigBundleControlService &Get();

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
    static qnzkna::framework::IRegistry            *s_IRegistryImpl;
    static qnzkna::SystemConfig::IConfigBundleControlImpl  s_IConfigBundleControlImpl;
    static qnzkna::SystemConfig::IConfigProjectControlImpl s_IConfigProjectControlImpl;
};

inline LY_ConfigBundleControlService &LY_ConfigBundleControlService::Get()
{
    static LY_ConfigBundleControlService sLY_ConfigBundleControlService;
    return sLY_ConfigBundleControlService;
}

#endif
