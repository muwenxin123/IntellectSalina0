#ifndef _LY_ZONEMANAGER_Interface_H
#define _LY_ZONEMANAGER_Interface_H

#include <IInterface.h>

class LY_ZoneManagerInterface : public IInterface
{
public:

    static LY_ZoneManagerInterface &Get();

    virtual void Startup();

    virtual void Shutdown();

    virtual void Reset();
    virtual bool DrawGraph(QPainter &painter, IProj *pProj);
    virtual bool DrawDynamic(QPainter &painter, IProj *pProj);

    virtual bool OnMessage(unsigned int uiMessage, int wParam, void *lParam);
    virtual bool OnMessageBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return true;
    };
    virtual bool OnServiceBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return true;
    };
};

#endif
