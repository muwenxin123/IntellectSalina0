#ifndef LY_AgricultureHomepageServiceInterface_H
#define LY_AgricultureHomepageServiceInterface_H

#include <IInterface.h>

class LY_AgricultureHomepageServiceInterface : public IInterface
{
public:

    static LY_AgricultureHomepageServiceInterface& Get();

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
