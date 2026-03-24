#include "LY_SystemMaintenanceService.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
    #include <QtWidgets>
#else
    #include <QtGui>
#endif
#include "LY_SystemMaintenanceServiceInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
#include "ISystemMaintenanceActivator.h"
#include "IFunctionInterface.h"

qnzkna::framework::IRegistry           *LY_SystemMaintenanceService::m_pIRegistryImpl = nullptr;
LY_SystemMaintenanceService::LY_SystemMaintenanceService()
{
    s_SystemMaintenanceImpl.m_pActivator = &ISystemMaintenanceActivator::Get();
}

void LY_SystemMaintenanceService::Startup()
{
    IFunctionInterface &ifi = LY_SystemMaintenanceServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            m_pIRegistryImpl = pIRegistry;
            ISystemMaintenanceActivator::Get().start(m_pIRegistryImpl);
        }
    }

    if (m_pIRegistryImpl)
    {
        m_pIRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.ISystemMaintenanceService", &(s_SystemMaintenanceImpl)));
    }
}

void LY_SystemMaintenanceService::Shutdown()
{
    if (m_pIRegistryImpl != nullptr)
    {
        m_pIRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.ISystemMaintenanceService", &(s_SystemMaintenanceImpl)));
        ISystemMaintenanceActivator::Get().stop(m_pIRegistryImpl);
    }
}

void LY_SystemMaintenanceService::Reset()
{

}

bool LY_SystemMaintenanceService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_SystemMaintenanceService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_SystemMaintenanceService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;

    return bReturn;
}

bool LY_SystemMaintenanceService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_SystemMaintenanceService::OnCommand(int nID)
{
    return false;
}

bool LY_SystemMaintenanceService::OnCreate(int wParam, QTabWidget *pTab)
{
    return false;
}

bool LY_SystemMaintenanceService::OnTimer(int wParam)
{
    return true;
}
