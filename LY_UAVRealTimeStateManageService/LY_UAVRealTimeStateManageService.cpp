#include "LY_UAVRealTimeStateManageService.h"
#include "LY_UAVRealTimeStateManageServiceInterface.h"
#include <IFunctionInterface.h>

#include <QDebug>

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

LY_UAVRealTimeStateManageService::LY_UAVRealTimeStateManageService()
{

}

void LY_UAVRealTimeStateManageService::Startup()
{
    IFunctionInterface &ifi = LY_UAVRealTimeStateManageServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            m_IUAVRealTimeStateManageActivator.start(pIRegistry);
        }
    }
}

void LY_UAVRealTimeStateManageService::Shutdown()
{
    IFunctionInterface &ifi = LY_UAVRealTimeStateManageServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            m_IUAVRealTimeStateManageActivator.stop(pIRegistry);
        }
    }
}

void LY_UAVRealTimeStateManageService::Reset()
{

}

bool LY_UAVRealTimeStateManageService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_UAVRealTimeStateManageService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_UAVRealTimeStateManageService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_UAVRealTimeStateManageService::OnMouseDoubleClick(QMouseEvent *e)
{
    qDebug("LY_UAVRealTimeStateManageService OnMouseDoubleClick");
    return false;
}

bool LY_UAVRealTimeStateManageService::OnCommand(int nID)
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
    return true;
    case 44778:
    {

    }
    break;
    default:
        break;
    }
    return false;
}

bool LY_UAVRealTimeStateManageService::OnCreate(int wParam, QTabWidget *pTab)
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

bool LY_UAVRealTimeStateManageService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        break;
    case 2:
        m_IUAVRealTimeStateManageActivator.OnTimer(wParam);
        break;
    case 3:
        break;
    }

    return false;
}
