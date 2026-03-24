#include "LY_PositionManageService.h"
#include "LY_PositionManageServiceInterface.h"
#include <IFunctionInterface.h>

#include <QDebug>

qnzkna::framework::IRegistry         *LY_PositionManageService::s_IRegistryImpl = nullptr;
qnzkna::PositionManage::IPositionServiceImpl LY_PositionManageService::s_IPositionServiceImpl;

LY_PositionManageService::LY_PositionManageService()
{

}

void LY_PositionManageService::Startup()
{
    IFunctionInterface &ifi = LY_PositionManageServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            s_IRegistryImpl = pIRegistry;
        }
    }

    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IPositionManageService", &(s_IPositionServiceImpl)));
    }
}

void LY_PositionManageService::Shutdown()
{
    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IPositionManageService", &(s_IPositionServiceImpl)));
    }
}

void LY_PositionManageService::Reset()
{

}

bool LY_PositionManageService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_PositionManageService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_PositionManageService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_PositionManageService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_PositionManageService::OnCommand(int nID)
{
    switch (nID)
    {
    case 43385:
    {

    }
    break;
    case 44777:
    {

    }
    break;
    case 44778:
    {

    }
    break;
    default:
        break;
    }
    return false;
}

bool LY_PositionManageService::OnCreate(int wParam, QTabWidget *pTab)
{
    switch (wParam)
    {
    case 0:
    {

    }
    break;
    case 1:
    {

    }
    break;
    case 2:
    {

    }
    break;
    case 3:
    {

    }
    break;
    case 4:
        break;
    }
    return false;
}

bool LY_PositionManageService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return false;
}
