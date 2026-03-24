#ifndef LY_SENSORMANAGESERVICEINTERFACE_H
#define LY_SENSORMANAGESERVICEINTERFACE_H

#include <IInterface.h>

class LY_SensorManageServiceInterface : public IInterface
{
public:

    static LY_SensorManageServiceInterface& Get();

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
