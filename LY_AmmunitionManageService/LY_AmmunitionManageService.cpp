#include "LY_AmmunitionManageService.h"
#include "LY_AmmunitionManageServiceInterface.h"
#include <IFunctionInterface.h>

qnzkna::framework::IRegistry*							LY_AmmunitionManageService::s_IRegistryImpl = nullptr;
qnzkna::AmmunitionManage::IAmmunitionManageImpl			LY_AmmunitionManageService::s_IServerManageImpl;

LY_AmmunitionManageService:: LY_AmmunitionManageService()
{
    m_pProj = NULL;
}

void LY_AmmunitionManageService::Startup()
{
	IFunctionInterface &ifi = LY_AmmunitionManageServiceInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry*>())
	{
		qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
		if (pIRegistry != nullptr)
		{
			s_IRegistryImpl = pIRegistry;
		}
	}

	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IAmmunitionManageService", &(s_IServerManageImpl)));
	}
}

void LY_AmmunitionManageService::Shutdown()
{
	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IAmmunitionManageService", &(s_IServerManageImpl)));
	}
}

void LY_AmmunitionManageService::Reset()
{

}

bool LY_AmmunitionManageService::DrawGraph(QPainter &painter, IProj *pProj)
{
	return false;
}

bool LY_AmmunitionManageService::DrawDynamic(QPainter &painter, IProj *pProj)
{
	return false;
}

bool LY_AmmunitionManageService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_AmmunitionManageService::OnMouseDoubleClick(QMouseEvent *e)
{
	return false;
}

bool LY_AmmunitionManageService::OnCommand(int nID)
{
    switch (nID)
    {
    case 43385:
    {

    }
        break;
    default:
        break;
    }
    return false;
}

bool LY_AmmunitionManageService::OnCreate(int wParam, QWidget *pWidget)
{
	return false;
}

bool LY_AmmunitionManageService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        LY_AmmunitionManageServiceInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}
