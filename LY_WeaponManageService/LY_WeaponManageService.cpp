#include "LY_WeaponManageService.h"
#include "LY_WeaponManageServiceInterface.h"
#include <IFunctionInterface.h>

#include <QDebug>

qnzkna::framework::IRegistry           *LY_WeaponManageService::s_IRegistryImpl = nullptr;
qnzkna::WeaponManage::IWeaponManageServiceImpl LY_WeaponManageService::s_IWeaponManageServiceImpl;

LY_WeaponManageService::LY_WeaponManageService()
{

}

void LY_WeaponManageService::Startup()
{
    IFunctionInterface &ifi = LY_WeaponManageServiceInterface::Get().GetFunctionInterface();
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
        s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IWeaponManageService", &(s_IWeaponManageServiceImpl)));
    }
}

void LY_WeaponManageService::Shutdown()
{
    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IWeaponManageService", &(s_IWeaponManageServiceImpl)));
    }
}

void LY_WeaponManageService::Reset()
{

}

bool LY_WeaponManageService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_WeaponManageService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_WeaponManageService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_WeaponManageService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_WeaponManageService::OnCommand(int nID)
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

bool LY_WeaponManageService::OnCreate(int wParam, QTabWidget *pTab)
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

bool LY_WeaponManageService::OnTimer(int wParam)
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
