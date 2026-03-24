#include "lyUcisPluginFrameWork.h"
#include "lyUcisPluginFrameWorkInterface.h"
#include <IFunctionInterface.h>

#include <QDebug>

Q_GLOBAL_STATIC(qnzkna::framework::IRegistryImpl, s_lyUcisPluginFrameWorkIRegistryImpl)

LyUcisPluginFrameWork::LyUcisPluginFrameWork()
{
}

void LyUcisPluginFrameWork::Startup()
{

    IFunctionInterface &ifi = LyUcisPluginFrameWorkInterface::Get().GetFunctionInterface();

    class IRegistryImplFunction : public IFunction
    {
    public:
        IRegistryImplFunction(void) {}
        virtual ~IRegistryImplFunction(void) {}

        virtual QVariant Handle(const QVariant &value)
        {
            Q_UNUSED(value)
            qnzkna::framework::IRegistry *pIRegistryImpl = s_lyUcisPluginFrameWorkIRegistryImpl;
            return qVariantFromValue(pIRegistryImpl);
        }
    };

    struFunInfo funInfoServiceInterfaceTracker;
    funInfoServiceInterfaceTracker.strName   = "Ly.IRegistryImpl";
    funInfoServiceInterfaceTracker.pFunction = new IRegistryImplFunction;
    ifi.Register(funInfoServiceInterfaceTracker);

}

void LyUcisPluginFrameWork::Shutdown()
{
    if (s_lyUcisPluginFrameWorkIRegistryImpl != nullptr)
    {
        s_lyUcisPluginFrameWorkIRegistryImpl->stopAllMessageListener();
        s_lyUcisPluginFrameWorkIRegistryImpl->unregisterAllMessage();
        s_lyUcisPluginFrameWorkIRegistryImpl->unregisterAllService();
    }
}

void LyUcisPluginFrameWork::Reset()
{

}

bool LyUcisPluginFrameWork::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LyUcisPluginFrameWork::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LyUcisPluginFrameWork::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LyUcisPluginFrameWork::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LyUcisPluginFrameWork::OnCommand(int nID)
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

bool LyUcisPluginFrameWork::OnCreate(int wParam, QWidget *pTab)
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

bool LyUcisPluginFrameWork::OnTimer(int wParam)
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
    return true;
}
