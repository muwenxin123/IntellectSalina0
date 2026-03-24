#ifndef LY_AMMUNITIONMANAGESERVICE_H
#define LY_AMMUNITIONMANAGESERVICE_H

#include "LY_AmmunitionManageService_global.h"
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "IAmmunitionManageImpl.h"
#include <QCString.h>
#include <QMetaType>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry*)

class LY_AMMUNITIONMANAGESERVICESHARED_EXPORT LY_AmmunitionManageService
{
public:
    LY_AmmunitionManageService();
    inline static LY_AmmunitionManageService&Get();

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
    inline void SetCoordinateTransformation(IProj *pProj){ m_pProj = pProj; };
    inline const QCString GetDataDir() const { return m_strDataDir; };
    inline const QCString GetConfDir() const { return m_strConfDir; };
    inline void SetDataDir(const char *lpszDir){ m_strDataDir = lpszDir; };
    inline void SetConfDir(const char *lpszDir){ m_strConfDir = lpszDir; };

public:
	static qnzkna::framework::IRegistry*								s_IRegistryImpl;
	static qnzkna::AmmunitionManage::IAmmunitionManageImpl				s_IServerManageImpl;

private:
    IProj       *m_pProj;
    QWidget		*m_pWnd;
    QCString	m_strDataDir;
    QCString	m_strConfDir;
};

inline LY_AmmunitionManageService &LY_AmmunitionManageService::Get()
{
    static LY_AmmunitionManageService sLY_AmmunitionManageService;
    return sLY_AmmunitionManageService;
}

#endif
