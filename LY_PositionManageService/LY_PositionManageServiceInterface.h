#ifndef LY_POSITIONMANAGESERVICEINTERFACE_H
#define LY_POSITIONMANAGESERVICEINTERFACE_H

#include <IInterface.h>

class LY_PositionManageServiceInterface : public IInterface
{
public:

    static LY_PositionManageServiceInterface& Get();

    virtual void Startup();

    virtual void Shutdown();

    virtual void Reset();

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
