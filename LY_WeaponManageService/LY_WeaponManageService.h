#ifndef LY_WEAPONMANAGESERVICE_H
#define LY_WEAPONMANAGESERVICE_H

#include <QMetaType>
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "WeaponManage/IWeaponManageServiceImpl.h"

#include "LY_WeaponManageService_global.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry*)

class LY_WEAPONMANAGESERVICESHARED_EXPORT LY_WeaponManageService
{
public:
    LY_WeaponManageService();
    inline static LY_WeaponManageService &Get();

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
    static qnzkna::framework::IRegistry*           s_IRegistryImpl;
    static qnzkna::WeaponManage::IWeaponManageServiceImpl s_IWeaponManageServiceImpl;
};

inline LY_WeaponManageService &LY_WeaponManageService::Get()
{
    static LY_WeaponManageService sLY_WeaponManageService;
    return sLY_WeaponManageService;
}

#endif
