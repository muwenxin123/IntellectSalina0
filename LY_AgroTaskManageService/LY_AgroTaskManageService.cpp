#include "LY_AgroTaskManageService.h"
#include "LY_AgroTaskManageServiceInterface.h"
#include <IFunctionInterface.h>

qnzkna::framework::IRegistry           *LY_AgroTaskManageService::s_IRegistryImpl = nullptr;
qnzkna::AgroTask::IAgroTaskImpl LY_AgroTaskManageService::s_IAgroTaskImpl;

LY_AgroTaskManageService:: LY_AgroTaskManageService()
{
    m_pProj = NULL;
}

void LY_AgroTaskManageService::Startup()
{
    IFunctionInterface &ifi = LY_AgroTaskManageServiceInterface::Get().GetFunctionInterface();
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
        s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IAgroTaskService", &(s_IAgroTaskImpl)));
    }
}

void LY_AgroTaskManageService::Shutdown()
{
    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IAgroTaskService", &(s_IAgroTaskImpl)));
    }
}

void LY_AgroTaskManageService::Reset()
{

}
bool LY_AgroTaskManageService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_AgroTaskManageService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    if (&s_IRegistryImpl != nullptr)
    {
        s_IAgroTaskImpl.DrawDynamic(&painter);
    }
    return true;
}

bool LY_AgroTaskManageService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_AgroTaskManageService::OnMouseDoubleClick(QMouseEvent *e)
{
    qDebug("OnMouseDoubleClick");
    return false;
}

bool LY_AgroTaskManageService::OnCommand(int nID)
{
    return false;
}

bool LY_AgroTaskManageService::OnCreate(int wParam, QWidget *pWidget)
{

    return false;
}

bool LY_AgroTaskManageService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1://200ms
        LY_AgroTaskManageServiceInterface::Get().Refresh(1);
        break;
    case 2://1s
        break;
    case 3://60s
        break;
    }
    return true;
}

void LY_AgroTaskManageService::X_S(double x, double y, float *l, float *b)
{
    if (m_pProj != nullptr && x < 1000000000. && x > -1000000000.)
    {
        int px, py;
        m_pProj->xyLtoD(x, y, &px, &py);
        *l = (float)px;
        *b = (float)py;
    }
}

void LY_AgroTaskManageService::S_X(float x, float y, double *xx, double *yy)
{
    QPoint pt;
    pt.setX((int)x);
    pt.setY((int)y);
    if (m_pProj != nullptr)
    {
        m_pProj->Screen2Meter(pt, *xx, *yy);
    }
}

void LY_AgroTaskManageService::L_X(double l, double b, double *x, double *y)
{
    float x1 = 0, y1 = 0;
    if (m_pProj != nullptr)
    {
        m_pProj->jwxy(l, b, x, y);
    }
}

void LY_AgroTaskManageService::X_L(double x, double y, double *l, double *b)
{
    float l1 = 0, b1 = 0;
    if (m_pProj != nullptr)
    {
        m_pProj->xyjw(x, y, l, b);
    }
}

void LY_AgroTaskManageService::L_S(double x, double y, int *sx, int *sy)
{
    if (m_pProj == nullptr)
    {
        return;
    }
    double zx, zy;
    m_pProj->jwxy((float)x, (float)y, &zx, &zy);
    m_pProj->xyLtoD((float)zx, (float)zy, sx, sy);
}

void LY_AgroTaskManageService::S_L(int sx, int sy, double *x, double *y)
{
    QPoint pt;
    pt.setX((int)sx);
    pt.setY((int)sy);
    double x1 = 0, y1 = 0;
    if (m_pProj != nullptr)
    {
        m_pProj->Screen2Meter(pt, x1, y1);
        m_pProj->xyjw((float)x1, (float)y1, x, y);
    }
    else
    {
        return;
    }
}
