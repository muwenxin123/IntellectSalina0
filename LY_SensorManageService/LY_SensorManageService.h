#ifndef LY_SENSORMANAGESERVICE_H
#define LY_SENSORMANAGESERVICE_H

#include <QMetaType>
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "SensorManage/ISensorManageImpl.h"

#include "LY_SensorManageService_global.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

class LY_SENSORMANAGESERVICESHARED_EXPORT LY_SensorManageService
{
public:
    LY_SensorManageService();
    inline static LY_SensorManageService &Get();

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
    static qnzkna::framework::IRegistry             *s_IRegistryImpl;
    static qnzkna::SensorManage::ISensorManageImpl   s_ISensorManageServiceImpl;
};

inline LY_SensorManageService &LY_SensorManageService::Get()
{
    static LY_SensorManageService sLY_SensorManageService;
    return sLY_SensorManageService;
}

#endif
