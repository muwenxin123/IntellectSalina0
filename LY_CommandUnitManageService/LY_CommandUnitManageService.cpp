#include "LY_CommandUnitManageService.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
    #include <QtWidgets>
#else
    #include <QtGui>
#endif
#include "LY_CommandUnitManageServiceInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
#include <QVariant>
#include "IFunctionInterface.h"

qnzkna::framework::IRegistry    *LY_CommandUnitManageService::m_pIRegistryImpl = nullptr;

LY_CommandUnitManageService::LY_CommandUnitManageService()
{
    m_pPopForm	= NULL;
    m_pProj		= NULL;
}

void LY_CommandUnitManageService::Startup()
{
    QStringList bundlePluginList =
    {
        "LyUcisCommandUnitManageService",
    };

    IFunctionInterface &ifi = LY_CommandUnitManageServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            m_pIRegistryImpl = pIRegistry;
        }
    }

    if (m_pIRegistryImpl)
    {
        m_pIRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.ICommandUnitManageService", &(s_commandunitServiceImpl)));
    }
}

void LY_CommandUnitManageService::Shutdown()
{
    if (m_pIRegistryImpl != nullptr)
    {
        m_pIRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.ICommandUnitManageService", &(s_commandunitServiceImpl)));
    }
}

void LY_CommandUnitManageService::Reset()
{

}

bool LY_CommandUnitManageService::DrawGraph(QPainter &painter, IProj *pProj)
{

    return true;
}

bool LY_CommandUnitManageService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_CommandUnitManageService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;

    return bReturn;
}

bool LY_CommandUnitManageService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_CommandUnitManageService::OnCommand(int nID)
{

    return false;
}

bool LY_CommandUnitManageService::OnCreate(int wParam, QTabWidget *pTab)
{
    return false;
}

bool LY_CommandUnitManageService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        LY_CommandUnitManageServiceInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}
