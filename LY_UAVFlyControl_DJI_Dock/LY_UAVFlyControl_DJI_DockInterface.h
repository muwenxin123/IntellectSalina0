#ifndef LY_UAVFLYCONTROL_DJI_DOCKINTERFACE_H
#define LY_UAVFLYCONTROL_DJI_DOCKINTERFACE_H

#include <IInterface.h>

class LY_UAVFlyControl_DJI_DockInterface : public IInterface
{
public:

    static LY_UAVFlyControl_DJI_DockInterface& Get();

    virtual void Startup();

    virtual void Shutdown();

    virtual void Reset();

    virtual bool OnNetRecv(int nLength, char *lpData);

    virtual bool DrawGraph(QPainter &painter, IProj *pProj);
    virtual bool DrawDynamic(QPainter &painter, IProj *pProj);
    virtual bool OnMessage(unsigned int uiMessage, int wParam, void *lParam);
    virtual bool OnMessageBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        Q_UNUSED(uiMessage)
        Q_UNUSED(wParam)
        Q_UNUSED(lParam)

        return false;
    }
    virtual bool OnServiceBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        Q_UNUSED(uiMessage)
        Q_UNUSED(wParam)
        Q_UNUSED(lParam)

        return false;
    }
};

#endif
