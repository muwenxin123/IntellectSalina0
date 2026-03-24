#ifndef LY_SYSTEMMAINTENANCESERVICE_H
#define LY_SYSTEMMAINTENANCESERVICE_H
#include <QMetaType>
#include "ServiceFramework/IRegistry.h"
#include "LY_SystemMaintenanceService_global.h"
#include "ISystemMaintenanceImpl.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class QListView;
class QTreeView;
class PopForm;
class QWidget;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)
class LY_SYSTEMMAINTENANCESERVICESHARED_EXPORT LY_SystemMaintenanceService
{
public:
    LY_SystemMaintenanceService();
    inline static LY_SystemMaintenanceService &Get();

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

private:
    static qnzkna::framework::IRegistry				*m_pIRegistryImpl;
    qnzkna::SystemMaintenance::ISystemMaintenanceImpl	s_SystemMaintenanceImpl;

};

inline LY_SystemMaintenanceService &LY_SystemMaintenanceService::Get()
{
    static LY_SystemMaintenanceService sLY_SystemMaintenanceService;
    return sLY_SystemMaintenanceService;
}

#endif
