#include "LY_SensorManageService.h"
#include "LY_SensorManageServiceInterface.h"
#include <IFunctionInterface.h>

qnzkna::framework::IRegistry           *LY_SensorManageService::s_IRegistryImpl = nullptr;
qnzkna::SensorManage::ISensorManageImpl LY_SensorManageService::s_ISensorManageServiceImpl;

LY_SensorManageService::LY_SensorManageService()
{

}

void LY_SensorManageService::Startup()
{
    IFunctionInterface &ifi = LY_SensorManageServiceInterface::Get().GetFunctionInterface();
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
        s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.ISensorManageService", &(s_ISensorManageServiceImpl)));
    }
}

void LY_SensorManageService::Shutdown()
{
    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.ISensorManageService", &(s_ISensorManageServiceImpl)));
    }
}

void LY_SensorManageService::Reset()
{

}

bool LY_SensorManageService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_SensorManageService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_SensorManageService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_SensorManageService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_SensorManageService::OnCommand(int nID)
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

bool LY_SensorManageService::OnCreate(int wParam, QTabWidget *pTab)
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

bool LY_SensorManageService::OnTimer(int wParam)
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
