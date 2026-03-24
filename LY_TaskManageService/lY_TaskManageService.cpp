#include "lY_TaskManageService.h"
#include "lY_TaskManageServiceInterface.h"
#include <IFunctionInterface.h>

qnzkna::framework::IRegistry*           LY_TaskManageService::s_IRegistryImpl = nullptr;
qnzkna::TaskManage::ITaskManageImpl		LY_TaskManageService::s_IFormManageImpl;

LY_TaskManageService::LY_TaskManageService()
{
    m_pProj = NULL;
}

void LY_TaskManageService::Startup()
{
	IFunctionInterface &ifi = LY_TaskManageServiceInterface::Get().GetFunctionInterface();
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
		s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.ITaskManageService", &(s_IFormManageImpl)));
	}
}

void LY_TaskManageService::Shutdown()
{
	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.ITaskManageService", &(s_IFormManageImpl)));
	}
}

void LY_TaskManageService::Reset()
{

}

bool LY_TaskManageService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_TaskManageService::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return true;
}

bool LY_TaskManageService::OnMouseButtonDown( QMouseEvent *e )
{
    bool bReturn = false;
    return bReturn;
}

bool LY_TaskManageService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_TaskManageService::OnCommand( int nID )
{
      return false;
}

bool LY_TaskManageService::OnCreate( int wParam, QTabWidget *pTab )
{
    return false;
}

bool LY_TaskManageService::OnTimer( int wParam )
{
    switch (wParam)
    {
    case 1:
        LY_TaskManageServiceInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}
