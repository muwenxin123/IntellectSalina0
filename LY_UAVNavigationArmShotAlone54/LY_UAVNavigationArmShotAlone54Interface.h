#ifndef LY_UAVNAVIGATIONARMSHOTALONE54INTERFACE_H
#define LY_UAVNAVIGATIONARMSHOTALONE54INTERFACE_H

#include <IInterface.h>

class LY_UAVNavigationArmShotAlone54Interface : public IInterface
{
public:
    static LY_UAVNavigationArmShotAlone54Interface &Get();

    virtual void Startup();

    virtual void Shutdown();

    virtual void Reset();

    virtual bool OnNetRecv(int nLength, char *lpData);

    virtual bool DrawGraph(QPainter &painter, IProj *pProj);
    virtual bool DrawDynamic(QPainter &painter, IProj *pProj);
    virtual bool OnMessage(unsigned int uiMessage, int wParam, void *lParam);
    virtual bool OnMessageBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return false;
    }
    virtual bool OnServiceBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return false;
    }
};

#endif
